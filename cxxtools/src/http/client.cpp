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

#include <cxxtools/http/client.h>
#include <cxxtools/net/addrinfo.h>
#include "clientimpl.h"

namespace cxxtools {

namespace http {

Client::Client()
: _impl(new ClientImpl(this))
{
}

Client::Client(const net::AddrInfo& addrinfo)
: _impl(new ClientImpl(this, addrinfo))
{
}

Client::Client(const std::string& host, unsigned short int port)
: _impl(new ClientImpl(this, net::AddrInfo(host, port)))
{
}


Client::Client(SelectorBase& selector, const net::AddrInfo& addrinfo)
: _impl(new ClientImpl(this, selector, addrinfo))
{
}

Client::Client(SelectorBase& selector, const std::string& host, unsigned short int port)
: _impl(new ClientImpl(this, selector, net::AddrInfo(host, port)))
{
}

Client::~Client()
{
  delete _impl;
}

void Client::connect(const net::AddrInfo& addrinfo)
{
    _impl->connect(addrinfo);
}

void Client::connect(const std::string& host, unsigned short int port)
{
    _impl->connect(net::AddrInfo(host, port));
}

const ReplyHeader& Client::execute(const Request& request, std::size_t timeout)
{
    try
    {
        return _impl->execute(request, timeout);
    }
    catch (...)
    {
        cancel();
        throw;
    }
}

const ReplyHeader& Client::header()
{
    return _impl->header();
}

void Client::readBody(std::string& s)
{
    _impl->readBody(s);
}

std::string Client::get(const std::string& url, std::size_t timeout)
{
    return _impl->get(url, timeout);
}

void Client::beginExecute(const Request& request)
{
    _impl->beginExecute(request);
}

void Client::setSelector(SelectorBase& selector)
{
    _impl->setSelector(selector);
}

void Client::wait(std::size_t msecs)
{
    _impl->wait(msecs);
}

std::istream& Client::in()
{
    return _impl->in();
}

const std::string& Client::host() const
{
    return _impl->host();
}

unsigned short int Client::port() const
{
    return _impl->port();
}

void Client::auth(const std::string& username, const std::string& password)
{
    _impl->auth(username, password);
}

void Client::clearAuth()
{
    _impl->clearAuth();
}

void Client::cancel()
{
    _impl->cancel();
}

} // namespace http

} // namespace cxxtools
