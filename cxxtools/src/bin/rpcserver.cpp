/*
 * Copyright (C) 2011 Tommi Maekitalo
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * is provided AS IS, WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, and
 * NON-INFRINGEMENT.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 *
 */

#include <cxxtools/bin/rpcserver.h>
#include "rpcserverimpl.h"

namespace cxxtools
{
namespace bin
{
RpcServer::RpcServer(EventLoopBase& eventLoop)
    : _impl(new RpcServerImpl(eventLoop, runmodeChanged))
{ }

RpcServer::RpcServer(EventLoopBase& eventLoop, const std::string& ip, unsigned short int port, int backlog)
    : _impl(new RpcServerImpl(eventLoop, runmodeChanged))
{
    listen(ip, port, backlog);
}

RpcServer::~RpcServer()
{
    delete _impl;
}

ServiceProcedure* RpcServer::getProcedure(const std::string& name)
{
    return _impl->getProcedure(name);
}

void RpcServer::releaseProcedure(ServiceProcedure* proc)
{
    return _impl->releaseProcedure(proc);
}

void RpcServer::registerProcedure(const std::string& name, ServiceProcedure* proc)
{
    _impl->registerProcedure(name, proc);
}

void RpcServer::listen(const std::string& ip, unsigned short int port, int backlog)
{
    _impl->listen(ip, port, backlog);
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

std::size_t RpcServer::idleTimeout() const
{
    return _impl->idleTimeout();
}

void RpcServer::idleTimeout(std::size_t m)
{
    _impl->idleTimeout(m);
}


}
}
