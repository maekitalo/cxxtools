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

#ifndef CXXTOOLS_BIN_RPCSERVER_H
#define CXXTOOLS_BIN_RPCSERVER_H

#include <string>
#include <cxxtools/signal.h>
#include <cxxtools/delegate.h>
#include <cxxtools/serviceregistry.h>

namespace cxxtools
{
class EventLoopBase;
class SslCertificate;
class SslCtx;

namespace bin
{
class Responder;
class RpcServerImpl;

class RpcServer : public ServiceRegistry
{
        friend class Responder;
        RpcServerImpl* newImpl(EventLoopBase& eventLoop);

    public:
        explicit RpcServer(EventLoopBase& eventLoop)
            : _impl(newImpl(eventLoop))
            { }

        RpcServer(EventLoopBase& eventLoop, const std::string& ip, unsigned short int port)
            : _impl(newImpl(eventLoop))
            { listen(ip, port); }

        RpcServer(EventLoopBase& eventLoop, const std::string& ip, unsigned short int port, const SslCtx& sslCtx)
            : _impl(newImpl(eventLoop))
            { listen(ip, port, sslCtx); }

        RpcServer(EventLoopBase& eventLoop, unsigned short int port)
            : _impl(newImpl(eventLoop))
            { listen(std::string(), port); }

        RpcServer(EventLoopBase& eventLoop, unsigned short int port, const SslCtx& sslCtx)
            : _impl(newImpl(eventLoop))
            { listen(std::string(), port, sslCtx); }

        ~RpcServer();

        /** Listen to the specified ip and port.
         *
         *  When a certificate file is given, ssl is enabled. If no private key file is given, the private key
         *  is expected to be in the certificate file.
         *
         *  Multiple listen calls can be made to listen on multiple interfaces or different settings.
         *
         */
        void listen(const std::string& ip, unsigned short int port);

        /** Listen to the specified ip and port with optionally ssl enabled.
         *
         */
        void listen(const std::string& ip, unsigned short int port, const SslCtx& sslCtx);

        void listen(unsigned short int port)
        { listen(std::string(), port); }
        void listen(unsigned short int port, const SslCtx& sslCtx)
        { listen(std::string(), port, sslCtx); }

        void addService(const ServiceRegistry& service);
        void addService(const std::string& domain, const ServiceRegistry& service);

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

        Delegate<bool, const SslCertificate&>& acceptSslCertificate();

        /// helper function to build a full function name from domain and function
        static std::string function(const std::string& domain, const std::string& fn);
        /// extract domain from full function name
        static std::string domain(const std::string& fn);
        /// extract just function name from full function name
        static std::string function(const std::string& fn);

    private:
        RpcServerImpl* _impl;
};
}
}

#endif // CXXTOOLS_BIN_RPCSERVER_H
