/*
 * Copyright (C) 2009 Tommi Maekitalo
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

#ifndef CXXTOOLS_HTTP_SERVERIMPL_H
#define CXXTOOLS_HTTP_SERVERIMPL_H

#include <cstddef>
#include <set>
#include <cxxtools/queue.h>
#include <cxxtools/selector.h>
#include <cxxtools/iodevice.h>
#include <cxxtools/connectable.h>
#include <cxxtools/event.h>
#include <cxxtools/http/server.h>
#include <cxxtools/http/service.h>
#include <cxxtools/http/responder.h>
#include "socket.h"

namespace cxxtools
{

class EventLoop;

namespace http
{

class Worker;
class Listener;
class ServerImpl;

class IdleSocketEvent : public Event
{
        Socket* _socket;

    public:
        explicit IdleSocketEvent(Socket* socket)
            : _socket(socket)
            { }

        Event& clone(Allocator& allocator) const;

        void destroy(Allocator& allocator);

        const std::type_info& typeInfo() const;

        Socket* socket() const   { return _socket; }

};

class ServerStartEvent : public Event
{
        const ServerImpl* _server;

    public:
        explicit ServerStartEvent(ServerImpl* server)
            : _server(server)
            { }

        Event& clone(Allocator& allocator) const;

        void destroy(Allocator& allocator);

        const std::type_info& typeInfo() const;

        const ServerImpl* server() const   { return _server; }

};

class ServerImpl : public Connectable
{
    public:
        ServerImpl(EventLoop& eventLoop, Signal<Server::Runmode>& runmodeChanged);
        ~ServerImpl();

        void listen(const std::string& ip, unsigned short int port);
        void createThread();

        void addService(const std::string& url, Service& service);
        void removeService(Service& service);

        Responder* getResponder(const Request& request);
        Responder* getDefaultResponder(const Request& request)
            { return _defaultService.createResponder(request); }

        std::size_t readTimeout() const       { return _readTimeout; }
        std::size_t writeTimeout() const      { return _writeTimeout; }
        std::size_t keepAliveTimeout() const  { return _keepAliveTimeout; }
        std::size_t idleTimeout() const       { return _idleTimeout; }

        void readTimeout(std::size_t ms)      { _readTimeout = ms; }
        void writeTimeout(std::size_t ms)     { _writeTimeout = ms; }
        void keepAliveTimeout(std::size_t ms) { _keepAliveTimeout = ms; }
        void idleTimeout(std::size_t ms)      { _idleTimeout = ms; }

        unsigned minThreads() const           { return _minThreads; }
        void minThreads(unsigned m)           { _minThreads = m; }

        unsigned maxThreads() const           { return _maxThreads; }
        void maxThreads(unsigned m)           { _maxThreads = m; }

        void onInput(Socket& _socket);

        bool isTerminating() const
        { return _runmode == Server::Terminating; }

    private:
        void runmode(Server::Runmode runmode)
        {
            _runmode = runmode;
            _runmodeChanged(runmode);
        }

        void addIdleSocket(Socket* _socket);
        void onEvent(const Event& event);
        void start();
        void terminate();

        friend class Worker;

        ////////////////////////////////////////////////////
        EventLoop& _eventLoop;

        std::size_t _readTimeout;
        std::size_t _writeTimeout;
        std::size_t _keepAliveTimeout;
        std::size_t _idleTimeout;
        unsigned _minThreads;
        unsigned _maxThreads;
        atomic_t _waitingThreads;

        Signal<Server::Runmode>& _runmodeChanged;
        Server::Runmode _runmode;

        Queue<Socket*> _queue;
        std::set<Socket*> _idleSockets;

        ////////////////////////////////////////////////////
        typedef std::vector<Listener*> ListenerType;
        ListenerType _listener;

        ////////////////////////////////////////////////////
        typedef std::set<Worker*> Threads;
        Threads _threads;
        Threads _terminatedThreads;
        Mutex _threadMutex;
        Condition _threadTerminated;
        void threadTerminated(Worker* worker);

        ////////////////////////////////////////////////////
        typedef std::multimap<std::string, Service*> ServicesType;
        Mutex _serviceMutex;
        ServicesType _service;
        NotFoundService _defaultService;
        NotAuthenticatedService _noAuthService;
};

}
}

#endif // CXXTOOLS_HTTP_SERVERIMPL_H

