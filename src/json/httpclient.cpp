/*
 * Copyright (C) 2011 by Tommi Meakitalo
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

#include "cxxtools/json/httpclient.h"
#include "cxxtools/net/uri.h"
#include "httpclientimpl.h"

namespace cxxtools
{

namespace json
{

HttpClient::HttpClient()
: _impl(new HttpClientImpl())
{
}


HttpClient::HttpClient(SelectorBase& selector, const std::string& server,
                             unsigned short port, const std::string& url)
: _impl(new HttpClientImpl())
{
    _impl->addRef();
    _impl->setSelector(selector);
    _impl->connect(server, port, url);
}


HttpClient::HttpClient(SelectorBase& selector, const net::Uri& uri)
: _impl(new HttpClientImpl())
{
    _impl->addRef();
    _impl->setSelector(selector);
    _impl->connect(uri.host(), uri.port(), uri.path());
    auth(uri.user(), uri.password());
}


HttpClient::HttpClient(const std::string& server, unsigned short port, const std::string& url)
: _impl(new HttpClientImpl())
{
    _impl->addRef();
    _impl->connect(server, port, url);
}


HttpClient::HttpClient(const net::Uri& uri)
: _impl(new HttpClientImpl())
{
    _impl->addRef();
    _impl->connect(uri.host(), uri.port(), uri.path());
    auth(uri.user(), uri.password());
}


HttpClient::HttpClient(HttpClient& other)
: _impl(other._impl)
{
    if (_impl)
        _impl->addRef();
}

HttpClient& HttpClient::operator= (const HttpClient& other)
{
    if (_impl && _impl->release() <= 0)
        delete _impl;

    _impl = other._impl;

    if (_impl)
        _impl->addRef();

    return *this;
}

HttpClient::~HttpClient()
{
    if (_impl && _impl->release() <= 0)
        delete _impl;
}

void HttpClient::connect(const net::AddrInfo& addrinfo, const std::string& url)
{
    _impl->connect(addrinfo, url);
}

void HttpClient::connect(const net::Uri& uri)
{
    _impl->connect(uri.host(), uri.port(), uri.path());
    auth(uri.user(), uri.password());
}

void HttpClient::connect(const std::string& addr, unsigned short port, const std::string& url)
{
    _impl->connect(addr, port, url);
}

void HttpClient::url(const std::string& url)
{
    _impl->url(url);
}

void HttpClient::auth(const std::string& username, const std::string& password)
{
    _impl->auth(username, password);
}

void HttpClient::clearAuth()
{
    _impl->clearAuth();
}

void HttpClient::setSelector(SelectorBase& selector)
{
    _impl->setSelector(selector);
}

void HttpClient::beginCall(IComposer& r, IRemoteProcedure& method, IDecomposer** argv, unsigned argc)
{
    _impl->beginCall(r, method, argv, argc);
}

void HttpClient::endCall()
{
    _impl->endCall();
}

void HttpClient::call(IComposer& r, IRemoteProcedure& method, IDecomposer** argv, unsigned argc)
{
    _impl->call(r, method, argv, argc);
}

std::size_t HttpClient::timeout() const
{
    return _impl->timeout();
}

void HttpClient::timeout(std::size_t t)
{
    _impl->timeout(t);
}

const std::string& HttpClient::url() const
{
    return _impl->url();
}

const IRemoteProcedure* HttpClient::activeProcedure() const
{
    return _impl->activeProcedure();
}

void HttpClient::cancel()
{
    _impl->cancel();
}

void HttpClient::wait(std::size_t msecs)
{
    _impl->wait(msecs);
}

}

}
