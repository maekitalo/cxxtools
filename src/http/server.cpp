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

#include <cxxtools/http/server.h>
#include <cxxtools/eventloop.h>
#include <cxxtools/log.h>
#include "serverimpl.h"

log_define("cxxtools.http.server")

namespace cxxtools {

namespace http {

Server::Server(EventLoopBase& eventLoop)
    : _impl(new ServerImpl(eventLoop, runmodeChanged))
{
}

Server::Server(EventLoopBase& eventLoop, const std::string& ip, unsigned short int port, const std::string& certificateFile, const std::string& privateKeyFile)
    : _impl(new ServerImpl(eventLoop, runmodeChanged))
{
    listen(ip, port, certificateFile, privateKeyFile);
}

Server::Server(EventLoopBase& eventLoop, unsigned short int port, const std::string& certificateFile, const std::string& privateKeyFile)
    : _impl(new ServerImpl(eventLoop, runmodeChanged))
{
    listen(port, certificateFile, privateKeyFile);
}

Server::~Server()
{
    if (!_impl)
        return;

    if (_impl->runmode() == Running)
        _impl->terminate();

    delete _impl;
}

void Server::listen(const std::string& ip, unsigned short int port, const std::string& certificateFile, const std::string& privateKeyFile)
{
    log_info_if(certificateFile.empty(), "listen ip=" << ip << " port=" << port);
    log_info_if(!certificateFile.empty(), "listen ip=" << ip << " port=" << port << " certificate: \"" << certificateFile << "\" private key: \"" << privateKeyFile << '"');
    _impl->listen(ip, port, certificateFile, privateKeyFile);
}

void Server::listen(unsigned short int port, const std::string& certificateFile, const std::string& privateKeyFile)
{
    log_info_if(certificateFile.empty(), "listen port=" << port);
    log_info_if(!certificateFile.empty(), "listen port=" << port << " certificate: \"" << certificateFile << "\" private key: \"" << privateKeyFile << '"');
    _impl->listen(std::string(), port, certificateFile, privateKeyFile);
}

void Server::addService(const std::string& url, Service& service)
{
    _impl->addService(url, service);
}

void Server::addService(const Regex& url, Service& service)
{
    _impl->addService(url, service);
}

void Server::removeService(Service& service)
{
    _impl->removeService(service);
}

Milliseconds Server::readTimeout() const
{
    return _impl->readTimeout();
}

Milliseconds Server::writeTimeout() const
{
    return _impl->writeTimeout();
}

Milliseconds Server::keepAliveTimeout() const
{
    return _impl->keepAliveTimeout();
}

void Server::readTimeout(Milliseconds ms)
{
    _impl->readTimeout(ms);
}

void Server::writeTimeout(Milliseconds ms)
{
    _impl->writeTimeout(ms);
}

void Server::keepAliveTimeout(Milliseconds ms)
{
    _impl->keepAliveTimeout(ms);
}

unsigned Server::minThreads() const
{
    return _impl->minThreads();
}

void Server::minThreads(unsigned m)
{
    _impl->minThreads(m);
}

unsigned Server::maxThreads() const
{
    return _impl->maxThreads();
}

void Server::maxThreads(unsigned m)
{
    _impl->maxThreads(m);
}

} // namespace http

} // namespace cxxtools
