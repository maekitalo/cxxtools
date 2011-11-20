/*
 * Copyright (C) 2011 Tommi Maekitalo
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * is provided AS IS, WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, and
 * NON-INFRINGEMENT.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 *
 */

#include "rpcserverimpl.h"
#include "listener.h"
#include "socket.h"
#include "worker.h"

#include <cxxtools/eventloop.h>
#include <cxxtools/net/tcpserver.h>
#include <cxxtools/log.h>

#include <signal.h>

log_define("cxxtools.bin.rpcserver.impl")

namespace cxxtools
{
namespace bin
{

// Sent from the worker thread when a socket is idle.
// The server will take that socket to the event loop.
class IdleSocketEvent : public BasicEvent<IdleSocketEvent>
{
        Socket* _socket;

    public:
        explicit IdleSocketEvent(Socket* socket)
            : _socket(socket)
            { }

        Socket* socket() const   { return _socket; }

};

// Sent from the server when constructed, so that the server
// knows, when the event loop is running.
class ServerStartEvent : public BasicEvent<ServerStartEvent>
{
        const RpcServerImpl* _server;

    public:
        explicit ServerStartEvent(const RpcServerImpl* server)
            : _server(server)
            { }

        const RpcServerImpl* server() const   { return _server; }

};

// Sent from a worker, when a job was fetched from the queue and
// no further threads are left for subsequent jobs.
class NoWaitingThreadsEvent : public BasicEvent<NoWaitingThreadsEvent>
{
};

// Sent from the worker, when he decidid to stop, because there are
// enough idle threads waiting on the queue already.
class ThreadTerminatedEvent : public BasicEvent<ThreadTerminatedEvent>
{
        Worker* _worker;

    public:
        explicit ThreadTerminatedEvent(Worker* worker)
            : _worker(worker)
            { }

        Worker* worker() const   { return _worker; }
};


RpcServerImpl::RpcServerImpl(EventLoopBase& eventLoop, Signal<RpcServer::Runmode>& runmodeChanged)
    : _runmode(RpcServer::Stopped),
      _runmodeChanged(runmodeChanged),
      _eventLoop(eventLoop),
      inputSlot(slot(*this, &RpcServerImpl::onInput)),
      _minThreads(5),
      _maxThreads(200),
      _idleTimeout(200)
{
    _eventLoop.event.subscribe(slot(*this, &RpcServerImpl::onIdleSocket));
    _eventLoop.event.subscribe(slot(*this, &RpcServerImpl::onNoWaitingThreads));
    _eventLoop.event.subscribe(slot(*this, &RpcServerImpl::onThreadTerminated));
    _eventLoop.event.subscribe(slot(*this, &RpcServerImpl::onServerStart));

    connect(_eventLoop.exited, *this, &RpcServerImpl::terminate);

    _eventLoop.commitEvent(ServerStartEvent(this));
}

RpcServerImpl::~RpcServerImpl()
{
    if (runmode() == RpcServer::Running)
    {
        try
        {
            terminate();
        }
        catch (const std::exception& e)
        {
            log_fatal("failed to terminate rpc server: " << e.what());
        }
    }

    for (ProcedureMap::iterator it = _procedures.begin(); it != _procedures.end(); ++it)
    {
        delete it->second;
    }
}

void RpcServerImpl::listen(const std::string& ip, unsigned short int port, int backlog)
{
    log_debug("listen on " << ip << " port " << port);
    Listener* listener = new Listener(ip, port, backlog, net::TcpServer::DEFER_ACCEPT);
    try
    {
        _listener.push_back(listener);
        _queue.put(new Socket(*this, *listener));
    }
    catch (...)
    {
        delete listener;
        throw;
    }

}

void RpcServerImpl::start()
{
    log_trace("start server");
    runmode(RpcServer::Starting);

    // SIGPIPE must be ignored
    ::signal(SIGPIPE, SIG_IGN);

    MutexLock lock(_threadMutex);
    while (_threads.size() < minThreads())
    {
        Worker* worker = new Worker(*this);
        _threads.insert(worker);
        worker->start();
    }

    runmode(RpcServer::Running);
}


void RpcServerImpl::terminate()
{
    MutexLock lock(_threadMutex);

    runmode(RpcServer::Terminating);

    try
    {
        for (unsigned n = 0; n < _listener.size(); ++n)
            _listener[n]->wakeConnect();

        _queue.put(0);

        while (!_threads.empty() || !_terminatedThreads.empty())
        {
            if (!_threads.empty())
            {
                _threadTerminated.wait(lock);
            }

            for (Threads::iterator it = _terminatedThreads.begin(); it != _terminatedThreads.end(); ++it)
                delete *it;

            _terminatedThreads.clear();
        }

        for (unsigned n = 0; n < _listener.size(); ++n)
            delete _listener[n];
        _listener.clear();

        while (!_queue.empty())
            delete _queue.get();

        for (IdleSocket::iterator it = _idleSocket.begin(); it != _idleSocket.end(); ++it)
            delete *it;

        _idleSocket.clear();

        runmode(RpcServer::Stopped);
    }
    catch (const std::exception& e)
    {
        runmode(RpcServer::Failed);
    }
}

void RpcServerImpl::noWaitingThreads()
{
    if (runmode() == RpcServer::Running)
        _eventLoop.commitEvent(NoWaitingThreadsEvent());
}


void RpcServerImpl::threadTerminated(Worker* worker)
{
    MutexLock lock(_threadMutex);

    _threads.erase(worker);
    if (runmode() == RpcServer::Running)
    {
        _eventLoop.commitEvent(ThreadTerminatedEvent(worker));
    }
    else
    {
        _terminatedThreads.insert(worker);
        _threadTerminated.signal();
    }
}

void RpcServerImpl::addIdleSocket(Socket* socket)
{
    log_debug("add idle socket " << static_cast<void*>(socket));

    if (runmode() == RpcServer::Running)
    {
        _eventLoop.commitEvent(IdleSocketEvent(socket));
    }
    else
    {
        log_debug("server not running; delete " << static_cast<void*>(socket));
        delete socket;
    }
}

void RpcServerImpl::onIdleSocket(const IdleSocketEvent& event)
{
    Socket* socket = event.socket();

    log_debug("add idle socket " << static_cast<void*>(socket) << " to selector");

    _idleSocket.insert(socket);
    socket->setSelector(&_eventLoop);
    socket->inputConnection = connect(socket->inputReady, inputSlot);
}

void RpcServerImpl::onNoWaitingThreads(const NoWaitingThreadsEvent& event)
{
    MutexLock lock(_threadMutex);

    if (_threads.size() >= maxThreads())
    {
        log_warn("thread limit " << maxThreads() << " reached");
        return;
    }

    try
    {
        Worker* worker = new Worker(*this);
        try
        {
            log_debug("create thread " << static_cast<void*>(worker) << "; running threads=" << _threads.size());
            worker->start();
            _threads.insert(worker);

            log_debug(_threads.size() << " threads running");
        }
        catch (const std::exception&)
        {
            delete worker;
            throw;
        }
    }
    catch (const std::exception& e)
    {
        log_warn("failed to create thread: " << e.what());
    }
}

void RpcServerImpl::onThreadTerminated(const ThreadTerminatedEvent& event)
{
    MutexLock lock(_threadMutex);
    log_debug("thread terminated (" << static_cast<void*>(event.worker()) << ") " << _threads.size() << " threads left");
    try
    {
        event.worker()->join();
    }
    catch (const std::exception& e)
    {
        log_error("failed to join thread: " << e.what());
    }

    delete event.worker();
}

void RpcServerImpl::onServerStart(const ServerStartEvent& event)
{
    if (event.server() == this)
    {
        start();
    }
}

void RpcServerImpl::onInput(Socket& socket)
{
    socket.removeSelector();
    log_debug("search socket " << static_cast<void*>(&socket) << " in idle socket");
    _idleSocket.erase(&socket);

    if (socket.isConnected())
    {
        socket.inputConnection.close();
        _queue.put(&socket);
    }
    else
    {
        log_debug("onInput; delete " << static_cast<void*>(&socket));
        delete &socket;
    }
}

ServiceProcedure* RpcServerImpl::getProcedure(const std::string& name)
{
    ProcedureMap::iterator it = _procedures.find( name );
    if( it == _procedures.end() )
    {
        return 0;
    }

    return it->second->clone();
}


void RpcServerImpl::releaseProcedure(ServiceProcedure* proc)
{
    delete proc;
}


void RpcServerImpl::registerProcedure(const std::string& name, ServiceProcedure* proc)
{
    ProcedureMap::iterator it = _procedures.find(name);
    if (it == _procedures.end())
    {
        std::pair<const std::string, ServiceProcedure*> p( name, proc );
        _procedures.insert( p );
    }
    else
    {
        delete it->second;
        it->second = proc;
    }
}

}
}
