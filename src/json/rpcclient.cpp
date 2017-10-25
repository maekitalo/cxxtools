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

#include <cxxtools/json/rpcclient.h>
#include <cxxtools/net/addrinfo.h>
#include <cxxtools/net/uri.h>
#include "rpcclientimpl.h"

#include "config.h"

namespace cxxtools
{
namespace json
{

RpcClientImpl* RpcClient::getImpl()
{
    if (_impl == 0)
    {
        _impl = new RpcClientImpl();
        _impl->addRef();
    }

    return _impl;
}

RpcClient::RpcClient(const RpcClient& other)
: _impl(other._impl)
{
    if (_impl)
        _impl->addRef();
}

RpcClient& RpcClient::operator= (const RpcClient& other)
{
    if (_impl == other._impl)
        return *this;

    if (_impl && _impl->release() <= 0)
        delete _impl;

    _impl = other._impl;

    if (_impl)
        _impl->addRef();

    return *this;
}

RpcClient::~RpcClient()
{
    if (_impl && _impl->release() <= 0)
        delete _impl;
}

void RpcClient::prepareConnect(const net::AddrInfo& addrinfo, bool ssl)
{
    getImpl()->prepareConnect(addrinfo, std::string());
    getImpl()->ssl(ssl);
}

void RpcClient::prepareConnect(const net::AddrInfo& addrinfo, const std::string& sslCertificate)
{
    getImpl()->prepareConnect(addrinfo, sslCertificate);
}

void RpcClient::prepareConnect(const std::string& host, unsigned short int port, bool ssl)
{
    prepareConnect(net::AddrInfo(host, port), ssl);
}

void RpcClient::prepareConnect(const std::string& host, unsigned short int port, const std::string& sslCertificate)
{
    prepareConnect(net::AddrInfo(host, port), sslCertificate);
}

void RpcClient::prepareConnect(const net::Uri& uri)
{
#ifdef WITH_SSL
    if (uri.protocol() != "json" && uri.protocol() != "jsons")
        throw std::runtime_error("only protocols \"json\" and \"jsons\" are supported by json rpc client");
    prepareConnect(net::AddrInfo(uri.host(), uri.port()), uri.protocol() == "jsons");
#else
    if (uri.protocol() != "json")
        throw std::runtime_error("only protocol \"json\" is supported by json rpc client");
    prepareConnect(net::AddrInfo(uri.host(), uri.port()));
#endif
}

void RpcClient::prepareConnect(const net::Uri& uri, const std::string& sslCertificate)
{
#ifdef WITH_SSL
    if (uri.protocol() != "json" && uri.protocol() != "jsons")
        throw std::runtime_error("only protocols \"json\" and \"jsons\" are supported by json rpc client");
    prepareConnect(net::AddrInfo(uri.host(), uri.port()), sslCertificate);
#else
    if (uri.protocol() != "json")
        throw std::runtime_error("only protocol \"json\" is supported by json rpc client");
    prepareConnect(net::AddrInfo(uri.host(), uri.port()));
#endif
}

void RpcClient::connect()
{
    getImpl()->connect();
}

void RpcClient::close()
{
    if (_impl)
        _impl->close();
}

void RpcClient::setSelector(SelectorBase* selector)
{
    getImpl()->setSelector(selector);
}

void RpcClient::setSelector(SelectorBase& selector)
{
    getImpl()->setSelector(&selector);
}

void RpcClient::beginCall(IComposer& r, IRemoteProcedure& method, IDecomposer** argv, unsigned argc)
{
    _impl->beginCall(r, method, argv, argc);
}

void RpcClient::endCall()
{
    _impl->endCall();
}

void RpcClient::call(IComposer& r, IRemoteProcedure& method, IDecomposer** argv, unsigned argc)
{
    _impl->call(r, method, argv, argc);
}

Milliseconds RpcClient::timeout() const
{
    return getImpl()->timeout();
}

void RpcClient::timeout(Milliseconds t)
{
    getImpl()->timeout(t);
}

Milliseconds RpcClient::connectTimeout() const
{
    return getImpl()->connectTimeout();
}

void RpcClient::connectTimeout(Milliseconds t)
{
    getImpl()->connectTimeout(t);
}

const IRemoteProcedure* RpcClient::activeProcedure() const
{
    return _impl == 0 ? 0 : _impl->activeProcedure();
}

void RpcClient::cancel()
{
    if (_impl)
        _impl->cancel();
}

void RpcClient::wait(Milliseconds msecs)
{
    _impl->wait(msecs);
}

const std::string& RpcClient::prefix() const
{
    return getImpl()->prefix();
}

void RpcClient::prefix(const std::string& p)
{
    getImpl()->prefix(p);
}

Delegate<bool, const SslCertificate&>& RpcClient::acceptSslCertificate()
{
    return getImpl()->socket().acceptSslCertificate;
}

}
}
