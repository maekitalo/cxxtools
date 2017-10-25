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

#include <cxxtools/signal.h>
#include <cxxtools/delegate.h>
#include <cxxtools/timespan.h>
#include <string>

namespace cxxtools
{

class EventLoopBase;
class SslCertificate;
class Regex;

namespace http
{

class Request;
class Service;
class ServerImplBase;

class Server
{
#if __cplusplus >= 201103L
        Server(const Server& server) = delete;
        Server& operator=(const Server& server) = delete;
#else
        Server(const Server&) { }
        Server& operator=(const Server&) { return *this; }
#endif
        ServerImplBase* newImpl(EventLoopBase& eventLoop);

    public:
        explicit Server(EventLoopBase& eventLoop)
            : _impl(newImpl(eventLoop))
            { }

        Server(EventLoopBase& eventLoop, const std::string& ip, unsigned short int port, const std::string& certificateFile = std::string(), const std::string& privateKeyFile = std::string(), int sslVerifyLevel = 0, const std::string& sslCa = std::string())
            : _impl(newImpl(eventLoop))
            { listen(ip, port, certificateFile, privateKeyFile, sslVerifyLevel, sslCa); }

        Server(EventLoopBase& eventLoop, const std::string& ip, unsigned short int port, const std::string& certificateFile, int sslVerifyLevel, const std::string& sslCa = std::string())
            : _impl(newImpl(eventLoop))
            { listen(ip, port, certificateFile, certificateFile, sslVerifyLevel, sslCa); }

        Server(EventLoopBase& eventLoop, unsigned short int port, const std::string& certificateFile = std::string(), const std::string& privateKeyFile = std::string(), int sslVerifyLevel = 0, const std::string& sslCa = std::string())
            { listen(port, certificateFile, privateKeyFile, sslVerifyLevel, sslCa); }

        Server(EventLoopBase& eventLoop, unsigned short int port, const std::string& certificateFile, int sslVerifyLevel, const std::string& sslCa = std::string())
            { listen(port, certificateFile, certificateFile, sslVerifyLevel, sslCa); }

        ~Server();

        /** Listen to the specified ip and port.
         *
         *  When a certificate file is given, ssl is enabled. If no private key file is given, the private key
         *  is expected to be in the certificate file.
         *
         *  Multiple listen calls can be made to listen on multiple interfaces or different settings.
         */
        void listen(const std::string& ip, unsigned short int port, const std::string& certificateFile = std::string(), const std::string& privateKeyFile = std::string(), int sslVerifyLevel = 0, const std::string& sslCa = std::string());
        void listen(unsigned short int port, const std::string& certificateFile = std::string(), const std::string& privateKeyFile = std::string(), int sslVerifyLevel = 0, const std::string& sslCa = std::string())
            { listen(std::string(), port, certificateFile, privateKeyFile, sslVerifyLevel, sslCa); }

        void addService(const std::string& url, Service& service);
        void addService(const Regex& url, Service& service);
        void removeService(Service& service);

        Milliseconds readTimeout() const;
        Milliseconds writeTimeout() const;
        Milliseconds keepAliveTimeout() const;

        void readTimeout(Milliseconds ms);
        void writeTimeout(Milliseconds ms);
        void keepAliveTimeout(Milliseconds ms);

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

    private:
        ServerImplBase* _impl;
};


} // namespace http

} // namespace cxxtools

#endif
