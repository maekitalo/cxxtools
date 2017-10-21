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

#include <cxxtools/json/rpcserver.h>
#include "rpcserverimpl.h"

namespace cxxtools
{
namespace json
{
RpcServerImpl* RpcServer::newImpl(EventLoopBase& eventLoop)
{
    return new RpcServerImpl(eventLoop, runmodeChanged, *this);
}

RpcServer::~RpcServer()
{
    delete _impl;
}

void RpcServer::listen(const std::string& ip, unsigned short int port, const std::string& certificateFile, const std::string& privateKeyFile, int sslVerifyLevel, const std::string& sslCa)
{
    _impl->listen(ip, port, certificateFile, privateKeyFile, sslVerifyLevel, sslCa);
}

void RpcServer::addService(const std::string& prefix, const ServiceRegistry& service)
{
    std::vector<std::string> procs = service.getProcedureNames();

    for (std::vector<std::string>::const_iterator it = procs.begin(); it != procs.end(); ++it)
    {
        registerProcedure(prefix + *it, service.getProcedure(*it));
    }
}

unsigned RpcServer::minThreads() const
{
    return _impl->minThreads();
}

void RpcServer::minThreads(unsigned m)
{
    _impl->minThreads(m);
}

unsigned RpcServer::maxThreads() const
{
    return _impl->maxThreads();
}

void RpcServer::maxThreads(unsigned m)
{
    _impl->maxThreads(m);
}

Delegate<bool, const SslCertificate&>& RpcServer::acceptSslCertificate()
{
    return _impl->acceptSslCertificate;
}

}
}
