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
#include "httpclientimpl.h"

namespace cxxtools
{

namespace xmlrpc
{

HttpClient::HttpClient()
: _impl(new HttpClientImpl())
{
  impl(_impl);
}


HttpClient::HttpClient(SelectorBase& selector, const std::string& server,
                             unsigned short port, const std::string& url)
: _impl(new HttpClientImpl(selector, server, port, url))
{
    impl(_impl);
}


HttpClient::HttpClient(const std::string& server, unsigned short port, const std::string& url)
: _impl(new HttpClientImpl(server, port, url))
{
    impl(_impl);
}


HttpClient::~HttpClient()
{
    delete _impl;
}

void HttpClient::connect(const net::AddrInfo& addrinfo, const std::string& url)
{
    _impl->connect(addrinfo, url);
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

}

}
