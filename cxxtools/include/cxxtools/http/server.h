/*
 * Copyright (C) 2009 by Marc Boris Duerner, Tommi Maekitalo
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

#ifndef cxxtools_Http_Server_h
#define cxxtools_Http_Server_h

#include <cxxtools/http/api.h>
#include <cxxtools/http/responder.h>
#include <cxxtools/net/tcpserver.h>
#include <cxxtools/net/tcpsocket.h>
#include <cxxtools/connectable.h>
#include <cxxtools/condition.h>
#include <cxxtools/mutex.h>
#include <cxxtools/thread.h>
#include <cxxtools/atomicity.h>
#include <cstddef>
#include <map>
#include <set>
#include <list>

namespace cxxtools {

namespace http {

class Responder;
class Request;
class Socket;
class Service;

class CXXTOOLS_HTTP_API Server : public net::TcpServer, public Connectable
{
    public:
        Server(const std::string& ip, unsigned short int port);
        ~Server()  { terminate(); }

        void addService(const std::string& url, Service& service);
        void removeService(Service& service);

        Responder* getResponder(const Request& request);
        Responder* getDefaultResponder(const Request& request)
            { return _defaultService.createResponder(request); }

        void onConnect(TcpServer& server);

        std::size_t readTimeout() const       { return _readTimeout; }
        std::size_t writeTimeout() const      { return _writeTimeout; }
        std::size_t keepAliveTimeout() const  { return _keepAliveTimeout; }

        void readTimeout(std::size_t ms)      { _readTimeout = ms; }
        void writeTimeout(std::size_t ms)     { _writeTimeout = ms; }
        void keepAliveTimeout(std::size_t ms) { _keepAliveTimeout = ms; }

        void terminate();

        void run();

    private:
        std::string _ip;
        unsigned short int _port;

        typedef std::multimap<std::string, Service*> ServicesType;
        ServicesType _service;
        Selector _selector;
        NotFoundService _defaultService;

        std::size_t _readTimeout;
        std::size_t _writeTimeout;
        std::size_t _keepAliveTimeout;

        void serverThread();

        Mutex _threadMutex;
        Mutex _createThreadMutex;
        Mutex _selectorMutex;
        Condition _threadRunning;
        AttachedThread* _startingThread;
        unsigned _minThreads;
        unsigned _maxThreads;
        atomic_t _waitingThreads;
        enum {
          Stopped,
          Starting,
          Running,
          Terminating
        } _runmode;
        Condition _terminated;
        Condition _threadTerminated;

        typedef std::set<AttachedThread*> Threads;
        Threads _threads;
        Threads _terminatedThreads;

        typedef std::list<Socket*> ServerSockets;
        ServerSockets _readySockets;
        Mutex _idleSocketsMutex;
        ServerSockets _idleSockets;

        bool hasReplyToDo() const  { return !_readySockets.empty(); }
        friend class Socket;
        void addReadySockets(Socket* s)
            { _readySockets.push_back(s); }

        void createThread();
};


} // namespace http

} // namespace cxxtools

#endif
