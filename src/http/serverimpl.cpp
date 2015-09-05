/*
 * Copyright (C) 2010 Tommi Maekitalo
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * As a special exception, you may use this file as part of a free
 * software library without restriction. Specifically, if other files
 * instantiate templates or use macros or inline functions from this
 * file, or you compile this file and link it with other files to
 * produce an executable, this file does not by itself cause the
 * resulting executable to be covered by the GNU General Public
 * License. This exception does not however invalidate any other
 * reasons why the executable file might be covered by the GNU Library
 * General Public License.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "serverimpl.h"
#include "worker.h"
#include "socket.h"

#include <cxxtools/eventloop.h>
#include <cxxtools/log.h>
#include <cxxtools/net/tcpserver.h>

#include <signal.h>

log_define("cxxtools.http.server.impl")

namespace cxxtools
{
namespace http
{

class IdleSocketEvent : public BasicEvent<IdleSocketEvent>
{
        Socket* _socket;

    public:
        explicit IdleSocketEvent(Socket* socket)
            : _socket(socket)
            { }

        Socket* socket() const   { return _socket; }

};

class KeepAliveTimeoutEvent : public BasicEvent<KeepAliveTimeoutEvent>
{
        Socket* _socket;

    public:
        explicit KeepAliveTimeoutEvent(Socket* socket)
            : _socket(socket)
            { }

        Socket* socket() const   { return _socket; }

};

class ServerStartEvent : public BasicEvent<ServerStartEvent>
{
        const ServerImpl* _server;

    public:
        explicit ServerStartEvent(const ServerImpl* server)
            : _server(server)
            { }

        const ServerImpl* server() const   { return _server; }

};

class NoWaitingThreadsEvent : public BasicEvent<NoWaitingThreadsEvent>
{
};

class ThreadTerminatedEvent : public BasicEvent<ThreadTerminatedEvent>
{
        Worker* _worker;

    public:
        explicit ThreadTerminatedEvent(Worker* worker)
            : _worker(worker)
            { }

        Worker* worker() const   { return _worker; }
};

class ActiveSocketEvent : public BasicEvent<ActiveSocketEvent>
{
        Socket* _socket;

    public:
        explicit ActiveSocketEvent(Socket* socket)
            : _socket(socket)
            { }

        Socket* socket() const   { return _socket; }

};


ServerImpl::ServerImpl(EventLoopBase& eventLoop, Signal<Server::Runmode>& runmodeChanged)
    : ServerImplBase(eventLoop, runmodeChanged),
      inputSlot(slot(*this, &ServerImpl::onInput)),
      timeoutSlot(slot(*this, &ServerImpl::onTimeout))
{
    _eventLoop.event.subscribe(slot(*this, &ServerImpl::onIdleSocket));
    _eventLoop.event.subscribe(slot(*this, &ServerImpl::onActiveSocket));
    _eventLoop.event.subscribe(slot(*this, &ServerImpl::onKeepAliveTimeout));
    _eventLoop.event.subscribe(slot(*this, &ServerImpl::onNoWaitingThreads));
    _eventLoop.event.subscribe(slot(*this, &ServerImpl::onThreadTerminated));
    _eventLoop.event.subscribe(slot(*this, &ServerImpl::onServerStart));

    connect(_eventLoop.exited, *this, &ServerImpl::terminate);

    _eventLoop.commitEvent(ServerStartEvent(this));
}

ServerImpl::~ServerImpl()
{
    if (runmode() == Server::Running)
    {
        try
        {
            terminate();
        }
        catch (const std::exception& e)
        {
            log_fatal("exception in http-server termination occured: " << e.what());
        }
    }
}

void ServerImpl::listen(const std::string& ip, unsigned short int port, int backlog)
{
    log_debug("listen on " << ip << " port " << port);
    net::TcpServer* listener = new net::TcpServer(ip, port, backlog,
        net::TcpServer::DEFER_ACCEPT|net::TcpServer::REUSEADDR);

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

void ServerImpl::start()
{
    log_trace("start server");
    runmode(Server::Starting);

    MutexLock lock(_threadMutex);
    while (_threads.size() < minThreads())
    {
        Worker* worker = new Worker(*this);
        _threads.insert(worker);
        worker->start();
    }

    runmode(Server::Running);
}

void ServerImpl::terminate()
{
    log_trace("terminate");

    _eventLoop.processEvents();

    MutexLock lock(_threadMutex);

    runmode(Server::Terminating);

    try
    {
        log_debug("wake " << _listener.size() << " listeners");
        for (ServerImpl::ListenerType::iterator it = _listener.begin(); it != _listener.end(); ++it)
            (*it)->terminateAccept();
        _queue.put(0);

        log_debug("terminate " << _threads.size() << " threads");
        while (!_threads.empty() || !_terminatedThreads.empty())
        {
            if (!_threads.empty())
            {
                log_debug("wait for terminated thread");
                _threadTerminated.wait(lock);
            }

            for (Threads::iterator it = _terminatedThreads.begin();
                it != _terminatedThreads.end(); ++it)
            {
                log_debug("join thread");
                (*it)->join();
                delete *it;
            }

            _terminatedThreads.clear();
        }

        log_debug("delete " << _listener.size() << " listeners");
        for (ServerImpl::ListenerType::iterator it = _listener.begin(); it != _listener.end(); ++it)
            delete *it;
        _listener.clear();

        while (!_queue.empty())
            delete _queue.get();

        for (std::set<Socket*>::iterator it = _idleSockets.begin(); it != _idleSockets.end(); ++it)
            delete *it;
        _idleSockets.clear();

        runmode(Server::Stopped);
    }
    catch (const std::exception& e)
    {
        runmode(Server::Failed);
    }
}

void ServerImpl::noWaitingThreads()
{
    MutexLock lock(_threadMutex);
    if (runmode() == Server::Running)
        _eventLoop.commitEvent(NoWaitingThreadsEvent());
}

void ServerImpl::threadTerminated(Worker* worker)
{
    MutexLock lock(_threadMutex);

    _threads.erase(worker);
    if (runmode() == Server::Running)
    {
        _eventLoop.commitEvent(ThreadTerminatedEvent(worker));
    }
    else
    {
        _terminatedThreads.insert(worker);
        _threadTerminated.signal();
    }
}

void ServerImpl::addIdleSocket(Socket* socket)
{
    log_debug("add idle socket " << static_cast<void*>(socket));

    if (runmode() == Server::Running)
    {
        _eventLoop.commitEvent(IdleSocketEvent(socket));
    }
    else
    {
        log_debug("server not running; delete " << static_cast<void*>(socket));
        delete socket;
    }
}

void ServerImpl::onIdleSocket(const IdleSocketEvent& event)
{
    Socket* socket = event.socket();

    log_debug("add idle socket " << static_cast<void*>(socket) << " to selector");

    _idleSockets.insert(socket);
    socket->setSelector(&_eventLoop);
    socket->inputConnection = connect(socket->inputReady, inputSlot);
    socket->timeoutConnection = connect(socket->timeout, timeoutSlot);
}

void ServerImpl::onActiveSocket(const ActiveSocketEvent& event)
{
    _queue.put(event.socket());
}

void ServerImpl::onNoWaitingThreads(const NoWaitingThreadsEvent& /*event*/)
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

void ServerImpl::onThreadTerminated(const ThreadTerminatedEvent& event)
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

void ServerImpl::onServerStart(const ServerStartEvent& event)
{
    if (event.server() == this)
    {
        start();
    }
}

void ServerImpl::onInput(Socket& socket)
{
    socket.removeSelector();
    log_debug("search socket " << static_cast<void*>(&socket) << " in idle sockets");
    _idleSockets.erase(&socket);

    if (socket.isConnected())
    {
        socket.inputConnection.close();
        socket.timeoutConnection.close();
        _eventLoop.commitEvent(ActiveSocketEvent(&socket));
    }
    else
    {
        log_debug("onInput; delete " << static_cast<void*>(&socket));
        delete &socket;
    }
}

void ServerImpl::onTimeout(Socket& socket)
{
    log_debug("timeout; socket " << static_cast<void*>(&socket));

    _eventLoop.commitEvent(KeepAliveTimeoutEvent(&socket));
}

void ServerImpl::onKeepAliveTimeout(const KeepAliveTimeoutEvent& event)
{
    Socket* socket = event.socket();
    _idleSockets.erase(socket);
    log_debug("onKeepAliveTimeout; delete " << static_cast<void*>(&socket));
    delete socket;
}


}
}
