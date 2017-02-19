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
#include <cxxtools/http/request.h>
#include <cxxtools/net/addrinfo.h>
#include <cxxtools/net/uri.h>
#include "clientimpl.h"

namespace cxxtools {

namespace http {

ClientImpl* Client::getImpl()
{
    if (!_impl)
    {
        _impl = new ClientImpl(this);
        _impl->addRef();
    }

    return _impl;
}

Client::Client(const net::AddrInfo& addrinfo, bool ssl)
: _impl(0)
{
    prepareConnect(addrinfo, ssl);
}

Client::Client(const net::Uri& uri)
: _impl(0)
{
    prepareConnect(uri);
}

Client::Client(const std::string& host, unsigned short int port, bool ssl)
: _impl(0)
{
    prepareConnect(host, port, ssl);
}


Client::Client(SelectorBase& selector, const std::string& host, unsigned short int port, bool ssl)
: _impl(0)
{
    prepareConnect(host, port, ssl);
    setSelector(selector);
}

Client::Client(SelectorBase& selector, const net::AddrInfo& addrinfo, bool ssl)
: _impl(0)
{
    prepareConnect(addrinfo, ssl);
    setSelector(selector);
}

Client::Client(SelectorBase& selector, const net::Uri& uri)
: _impl(0)
{
    prepareConnect(uri);
    setSelector(selector);
}

Client::Client(const Client& other)
: _impl(other._impl)
{
    if (_impl)
        _impl->addRef();
}

Client& Client::operator= (const Client& other)
{
    if (_impl && _impl->release() <= 0)
        delete _impl;

    _impl = other._impl;

    if (_impl)
        _impl->addRef();

    return *this;
}

Client::~Client()
{
    if (_impl && _impl->release() <= 0)
        delete _impl;
}

void Client::prepareConnect(const net::AddrInfo& addrinfo, bool ssl)
{
    getImpl()->prepareConnect(addrinfo, ssl);
}

void Client::prepareConnect(const std::string& host, unsigned short int port, bool ssl)
{
    prepareConnect(net::AddrInfo(host, port, ssl));
}

void Client::prepareConnect(const net::Uri& uri)
{
    if (uri.protocol() != "http" && uri.protocol() != "https")
        throw std::runtime_error("only protocols http and https are supported by http client");
    prepareConnect(net::AddrInfo(uri.host(), uri.port()), uri.protocol() == "https");
    auth(uri.user(), uri.password());
}

void Client::connect()
{
    getImpl()->connect();
}

void Client::close()
{
    if (_impl)
        _impl->close();
}

const ReplyHeader& Client::execute(const Request& request, Milliseconds timeout, Milliseconds connectTimeout)
{
    try
    {
        return _impl->execute(request, timeout, connectTimeout);
    }
    catch (...)
    {
        cancel();
        throw;
    }
}

const Reply& Client::reply() const
{
    return _impl->reply();
}

Reply& Client::reply()
{
    return _impl->reply();
}

const Reply& Client::readBody()
{
    _impl->readBody();
    return reply();
}

const Reply& Client::get(const std::string& url, const QueryParams& qparams, Milliseconds timeout, Milliseconds connectTimeout)
{
    Request request(url);
    request.method("GET");
    request.qparams(qparams.getUrl());
    execute(request, timeout, connectTimeout);
    readBody();
    return _impl->reply();
}

const Reply& Client::post(const std::string& url, const QueryParams& qparams, Milliseconds timeout, Milliseconds connectTimeout)
{
    Request request(url);
    request.method("POST");
    request.body() << qparams.getUrl();
    request.addHeader("Content-Type", "application/x-www-form-urlencoded");
    execute(request, timeout, connectTimeout);
    readBody();
    return _impl->reply();
}

void Client::beginExecute(const Request& request)
{
    _impl->beginExecute(request);
}

void Client::endExecute()
{
    _impl->endExecute();
}

void Client::setSelector(SelectorBase& selector)
{
    getImpl()->setSelector(selector);
}

SelectorBase* Client::selector()
{
    return _impl ? _impl->selector() : 0;
}

bool Client::wait(Milliseconds msecs)
{
    return _impl->wait(msecs);
}

std::istream& Client::in()
{
    return _impl->in();
}

const std::string& Client::host() const
{
    return getImpl()->host();
}

unsigned short int Client::port() const
{
    return getImpl()->port();
}

void Client::auth(const std::string& username, const std::string& password)
{
    getImpl()->auth(username, password);
}

void Client::clearAuth()
{
    getImpl()->clearAuth();
}

void Client::cancel()
{
    if (_impl)
        _impl->cancel();
}

} // namespace http

} // namespace cxxtools
