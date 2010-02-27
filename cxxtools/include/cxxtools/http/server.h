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
#include <cxxtools/signal.h>
#include <cxxtools/noncopyable.h>
#include <string>
#include <cstddef>

namespace cxxtools {

class EventLoop;

namespace http {

class Request;
class Service;
class ServerImpl;

class CXXTOOLS_HTTP_API Server : private cxxtools::NonCopyable
{
    public:
        explicit Server(EventLoop& eventLoop);
        Server(EventLoop& eventLoop, const std::string& ip, unsigned short int port);
        ~Server();

        void listen(const std::string& ip, unsigned short int port);

        void addService(const std::string& url, Service& service);
        void removeService(Service& service);

        std::size_t readTimeout() const;
        std::size_t writeTimeout() const;
        std::size_t keepAliveTimeout() const;

        void readTimeout(std::size_t ms);
        void writeTimeout(std::size_t ms);
        void keepAliveTimeout(std::size_t ms);

        unsigned minThreads() const;
        void minThreads(unsigned m);

        unsigned maxThreads() const;
        void maxThreads(unsigned m);

        enum Runmode {
          Stopped,
          Starting,
          Running,
          Terminating
        };

        Signal<Runmode> runmodeChanged;

    private:
        ServerImpl* _impl;
};


} // namespace http

} // namespace cxxtools

#endif
