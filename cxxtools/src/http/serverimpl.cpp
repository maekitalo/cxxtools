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
#include <cxxtools/eventloop.h>
#include <cxxtools/log.h>
#include "listener.h"

log_define("cxxtools.http.server.impl")

namespace cxxtools
{
namespace http
{

ServerImpl::ServerImpl(EventLoopBase& eventLoop, Signal<Server::Runmode>& runmodeChanged)
    : _eventLoop(eventLoop),
      inputSlot(slot(*this, &ServerImpl::onInput)),
      timeoutSlot(slot(*this, &ServerImpl::onTimeout)),
      _readTimeout(20000),
      _writeTimeout(20000),
      _keepAliveTimeout(30000),
      _idleTimeout(100),
      _minThreads(5),
      _maxThreads(200),
      _waitingThreads(0),
      _runmodeChanged(runmodeChanged),
      _runmode(Server::Stopped)
{
    _eventLoop.event.subscribe(slot(*this, &ServerImpl::onIdleSocket));
    _eventLoop.event.subscribe(slot(*this, &ServerImpl::onKeepAliveTimeout));
    _eventLoop.event.subscribe(slot(*this, &ServerImpl::onNoWaitingThreads));
    _eventLoop.event.subscribe(slot(*this, &ServerImpl::onThreadTerminated));
    _eventLoop.event.subscribe(slot(*this, &ServerImpl::onServerStart));

    connect(_eventLoop.exited, *this, &ServerImpl::terminate);

    _eventLoop.commitEvent(ServerStartEvent(this));
}

ServerImpl::~ServerImpl()
{
    if (_runmode == Server::Running)
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

void ServerImpl::listen(const std::string& ip, unsigned short int port)
{
    log_debug("listen on " << ip << " port " << port);
    Listener* listener = new Listener(ip, port);
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
    while (_threads.size() < _minThreads)
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

    MutexLock lock(_threadMutex);

    runmode(Server::Terminating);

    try
    {
        log_debug("wake " << _listener.size() << " listeners");
        for (ServerImpl::ListenerType::iterator it = _listener.begin(); it != _listener.end(); ++it)
            (*it)->wakeConnect();

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

        runmode(Server::Stopped);
    }
    catch (const std::exception& e)
    {
        runmode(Server::Failed);
    }
}

void ServerImpl::noWaitingThreads()
{
    if (_runmode == Server::Running)
        _eventLoop.commitEvent(NoWaitingThreadsEvent());
}

void ServerImpl::threadTerminated(Worker* worker)
{
    MutexLock lock(_threadMutex);

    _threads.erase(worker);
    if (_runmode == Server::Running)
    {
        _eventLoop.commitEvent(ThreadTerminatedEvent(worker));
    }
    else
    {
        _terminatedThreads.insert(worker);
        _threadTerminated.signal();
    }
}

void ServerImpl::addIdleSocket(Socket* _socket)
{
    log_debug("add idle socket " << static_cast<void*>(_socket));

    if (_runmode == Server::Running)
    {
        _eventLoop.commitEvent(IdleSocketEvent(_socket));
    }
    else
    {
        log_debug("server not running; delete " << static_cast<void*>(_socket));
        delete _socket;
    }
}

void ServerImpl::onIdleSocket(const IdleSocketEvent& event)
{
    Socket* socket = event.socket();

    log_debug("add idle socket " << static_cast<void*>(socket) << " to selector");

    _idleSockets.insert(socket);
    socket->setSelector(&_eventLoop);
    connect(socket->inputReady, inputSlot);
    connect(socket->timeout, timeoutSlot);
}

void ServerImpl::onNoWaitingThreads(const NoWaitingThreadsEvent& event)
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
        disconnect(socket.inputReady, inputSlot);
        disconnect(socket.timeout, timeoutSlot);
        _queue.put(&socket);
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

void ServerImpl::addService(const std::string& url, Service& service)
{
    log_debug("add service for url <" << url << '>');

    cxxtools::MutexLock serviceLock(_serviceMutex);
    _service.insert(ServicesType::value_type(url, &service));
}

void ServerImpl::removeService(Service& service)
{
    cxxtools::MutexLock serviceLock(_serviceMutex);

    ServicesType::iterator it = _service.begin();
    while (it != _service.end())
    {
        if (it->second == &service)
        {
            _service.erase(it++);
        }
        else
        {
            ++it;
        }
    }
}

Responder* ServerImpl::getResponder(const Request& request)
{
    log_debug("get responder for url <" << request.url() << '>');

    cxxtools::MutexLock serviceLock(_serviceMutex);

    for (ServicesType::const_iterator it = _service.lower_bound(request.url());
        it != _service.end() && it->first == request.url(); ++it)
    {
        if (!it->second->checkAuth(request))
        {
            return _noAuthService.createResponder(request, it->second->realm(), it->second->authContent());
        }

        Responder* resp = it->second->createResponder(request);
        if (resp)
        {
            log_debug("got responder");
            return resp;
        }
    }

    log_debug("use default responder");
    return _defaultService.createResponder(request);
}


}
}
