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

#include <cxxtools/bin/rpcclient.h>
#include "rpcclientimpl.h"

namespace cxxtools
{
namespace bin
{

RpcClient::RpcClient(SelectorBase& selector, const std::string& addr, unsigned short port)
    : _impl(new RpcClientImpl(this, selector, addr, port))
{ 
}

RpcClient::RpcClient(const std::string& addr, unsigned short port)
    : _impl(new RpcClientImpl(this, addr, port))
{ 
}

RpcClient::~RpcClient()
{
    delete _impl;
}

void RpcClient::setSelector(SelectorBase& selector)
{
    _impl->setSelector(selector);
}

void RpcClient::connect(const std::string& addr, unsigned short port)
{
    _impl->connect(addr, port);
}

void RpcClient::close()
{
    _impl->close();
}

void RpcClient::beginCall(IDeserializer& r, IRemoteProcedure& method, ISerializer** argv, unsigned argc)
{
    _impl->beginCall(r, method, argv, argc);
}

void RpcClient::endCall()
{
    _impl->endCall();
}

void RpcClient::call(IDeserializer& r, IRemoteProcedure& method, ISerializer** argv, unsigned argc)
{
    _impl->call(r, method, argv, argc);
}

const IRemoteProcedure* RpcClient::activeProcedure() const
{
    return _impl->activeProcedure();
}

void RpcClient::cancel()
{
    _impl->cancel();
}

}
}
