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

#ifndef CXXTOOLS_HTTP_SERVERIMPLBASE_H
#define CXXTOOLS_HTTP_SERVERIMPLBASE_H

#include <cxxtools/http/server.h>
#include <cxxtools/timespan.h>
#include "mapper.h"

namespace cxxtools
{

class EventLoopBase;

namespace http
{

class ServerImplBase
{
#if __cplusplus >= 201103L
        ServerImplBase(const ServerImplBase&) = delete;
        ServerImplBase& operator=(const ServerImplBase&) = delete;
#else
        ServerImplBase(const ServerImplBase&);
        ServerImplBase& operator=(const ServerImplBase&);
#endif

    public:
        ServerImplBase(EventLoopBase& eventLoop, Signal<Server::Runmode>& runmodeChanged)
            : _eventLoop(eventLoop),
              _readTimeout(Seconds(20)),
              _writeTimeout(Seconds(20)),
              _keepAliveTimeout(Seconds(30)),
              _minThreads(5),
              _maxThreads(200),
              _runmodeChanged(runmodeChanged),
              _runmode(Server::Stopped)
        { }

        virtual ~ServerImplBase() { }

        virtual void listen(const std::string& ip, unsigned short int port, const std::string& certificateFile, const std::string& privateKeyFile, int sslVerifyLevel, const std::string& sslCa) = 0;

        void addService(const std::string& url, Service& service)
        { _mapper.addService(url, service); }
        void addService(const Regex& url, Service& service)
        { _mapper.addService(url, service); }
        void removeService(Service& service)
        { _mapper.removeService(service); }

        Responder* getResponder(const Request& request)
            { return _mapper.getResponder(request); }
        Responder* getDefaultResponder(const Request& request)
            { return _mapper.getDefaultResponder(request); }

        Milliseconds readTimeout() const       { return _readTimeout; }
        Milliseconds writeTimeout() const      { return _writeTimeout; }
        Milliseconds keepAliveTimeout() const  { return _keepAliveTimeout; }

        void readTimeout(Milliseconds ms)      { _readTimeout = ms; }
        void writeTimeout(Milliseconds ms)     { _writeTimeout = ms; }
        void keepAliveTimeout(Milliseconds ms) { _keepAliveTimeout = ms; }

        unsigned minThreads() const           { return _minThreads; }
        void minThreads(unsigned m)           { _minThreads = m; }

        unsigned maxThreads() const           { return _maxThreads; }
        void maxThreads(unsigned m)           { _maxThreads = m; }

        virtual void terminate()              { }
        Server::Runmode runmode() const
        { return _runmode; }

        Delegate<bool, const SslCertificate&> acceptSslCertificate;

    protected:
        void runmode(Server::Runmode runmode)
        {
            _runmode = runmode;
            _runmodeChanged(runmode);
        }

        EventLoopBase& _eventLoop;

    private:
        Milliseconds _readTimeout;
        Milliseconds _writeTimeout;
        Milliseconds _keepAliveTimeout;

        unsigned _minThreads;
        unsigned _maxThreads;

        Signal<Server::Runmode>& _runmodeChanged;
        Server::Runmode _runmode;

        Mapper _mapper;
};

}
}

#endif // CXXTOOLS_HTTP_SERVERIMPL_H

