/*
 * Copyright (C) 2011 Tommi Maekitalo
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

#ifndef CXXTOOLS_JSON_RPCSERVER_H
#define CXXTOOLS_JSON_RPCSERVER_H

#include <string>
#include <cxxtools/signal.h>
#include <cxxtools/serviceregistry.h>

namespace cxxtools
{
    class EventLoopBase;

    namespace json
    {
        class Responder;
        class RpcServerImpl;

        class RpcServer : public ServiceRegistry
        {
                friend class Responder;

            public:
                RpcServer(EventLoopBase& eventLoop);
                RpcServer(EventLoopBase& eventLoop, const std::string& ip, unsigned short int port, int backlog = 64);
                RpcServer(EventLoopBase& eventLoop, unsigned short int port, int backlog = 64);
                ~RpcServer();

                void listen(const std::string& ip, unsigned short int port, int backlog = 64);
                void listen(unsigned short int port, int backlog = 64);

                void addService(const std::string& praefix, const ServiceRegistry& service);

                unsigned minThreads() const;
                void minThreads(unsigned m);

                unsigned maxThreads() const;
                void maxThreads(unsigned m);

                enum Runmode {
                  Stopped,
                  Starting,
                  Running,
                  Terminating,
                  Failed
                };

                Signal<Runmode> runmodeChanged;

            private:
                RpcServerImpl* _impl;
        };
    }
}

#endif // CXXTOOLS_JSON_RPCSERVER_H
