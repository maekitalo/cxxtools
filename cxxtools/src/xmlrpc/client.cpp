/*
 * Copyright (C) 2009 by Dr. Marc Boris Duerner
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

#include "cxxtools/xmlrpc/client.h"
#include "clientimpl.h"

namespace cxxtools {

namespace xmlrpc {

Client::Client()
: _impl(new ClientImpl())
{
}


Client::Client(SelectorBase& selector, const std::string& server,
                             unsigned short port, const std::string& url)
: _impl(new ClientImpl(selector, server, port, url))
{
}


Client::Client(const std::string& server, unsigned short port, const std::string& url)
: _impl(new ClientImpl(server, port, url))
{
}


Client::~Client()
{
    delete _impl;
}

void Client::connect(const std::string& addr, unsigned short port, const std::string& url)
{
    _impl->connect(addr, port, url);
}

void Client::auth(const std::string& username, const std::string& password)
{
    _impl->auth(username, password);
}

void Client::clearAuth()
{
    _impl->clearAuth();
}

void Client::beginCall(IDeserializer& r, IRemoteProcedure& method, ISerializer** argv, unsigned argc)
{
    _impl->beginCall(r, method, argv, argc);
}

void Client::call(IDeserializer& r, IRemoteProcedure& method, ISerializer** argv, unsigned argc)
{
    _impl->call(r, method, argv, argc);
}

std::size_t Client::timeout() const
{
    return _impl->timeout();
}

void Client::timeout(std::size_t t)
{
    _impl->timeout(t);
}

std::string Client::url() const
{
    return _impl->url();
}

}

}
