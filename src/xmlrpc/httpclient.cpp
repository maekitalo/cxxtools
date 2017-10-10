/*
 * Copyright (C) 2009 by Tommi Meakitalo
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

#include "cxxtools/xmlrpc/httpclient.h"
#include "cxxtools/net/uri.h"
#include "cxxtools/net/addrinfo.h"
#include "httpclientimpl.h"
#include <stdexcept>

namespace cxxtools
{

namespace xmlrpc
{

HttpClientImpl* HttpClient::getImpl()
{
    if (_impl == 0)
    {
        _impl = new HttpClientImpl();
        _impl->addRef();
        impl(_impl);
    }

    return _impl;
}

HttpClient::HttpClient(SelectorBase& selector, const std::string& server,
                             unsigned short port, const std::string& url, bool ssl)
: _impl(0)
{
    prepareConnect(net::AddrInfo(server, port), url, ssl);
    setSelector(selector);
}


HttpClient::HttpClient(SelectorBase& selector, const net::Uri& uri)
: _impl(0)
{
    prepareConnect(net::AddrInfo(uri.host(), uri.port()), uri.path());
    setSelector(selector);
    auth(uri.user(), uri.password());
}


HttpClient::HttpClient(const std::string& server, unsigned short port, const std::string& url, bool ssl)
: _impl(0)
{
    prepareConnect(net::AddrInfo(server, port), url, ssl);
}


HttpClient::HttpClient(const net::Uri& uri)
: _impl(0)
{
    prepareConnect(uri);
    auth(uri.user(), uri.password());
}

HttpClient::HttpClient(const HttpClient& other)
: Client(*this),
  _impl(other._impl)
{
    if (_impl)
    {
        _impl->addRef();
        impl(_impl);
    }
}

HttpClient& HttpClient::operator= (const HttpClient& other)
{
    if (_impl == other._impl)
        return *this;

    if (_impl && _impl->release() <= 0)
        delete _impl;

    _impl = other._impl;

    if (_impl)
        _impl->addRef();

    impl(_impl);

    return *this;
}

HttpClient::~HttpClient()
{
    if (_impl && _impl->release() <= 0)
        delete _impl;
}

void HttpClient::prepareConnect(const net::AddrInfo& addrinfo, const std::string& url, bool ssl)
{
    getImpl()->prepareConnect(addrinfo, url, ssl);
}

void HttpClient::prepareConnect(const net::Uri& uri)
{
    if (uri.protocol() != "http" && uri.protocol() != "https")
        throw std::runtime_error("only protocols http and https are supported by http client");
    prepareConnect(net::AddrInfo(uri.host(), uri.port()), uri.path(), uri.protocol() == "https");
    auth(uri.user(), uri.password());
}

void HttpClient::prepareConnect(const std::string& host, unsigned short port,
             const std::string& url, bool ssl)
{
    prepareConnect(net::AddrInfo(host, port), url, ssl);
}

void HttpClient::connect()
{
    getImpl()->connect();
}

void HttpClient::url(const std::string& url)
{
    getImpl()->url(url);
}

void HttpClient::auth(const std::string& username, const std::string& password)
{
    getImpl()->auth(username, password);
}

void HttpClient::clearAuth()
{
    getImpl()->clearAuth();
}

void HttpClient::setSelector(SelectorBase* selector)
{
    getImpl()->setSelector(selector);
}

void HttpClient::setSelector(SelectorBase& selector)
{
    getImpl()->setSelector(&selector);
}

void HttpClient::wait(Milliseconds msecs)
{
    getImpl()->wait(msecs);
}

}

}
