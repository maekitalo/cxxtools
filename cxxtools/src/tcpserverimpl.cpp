/*
 * Copyright (C) 2003 Tommi Maekitalo
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

#include <cxxtools/net/addrinfo.h>
#include "tcpserverimpl.h"
#include "addrinfoimpl.h"
#include <cxxtools/net/tcpserver.h>
#include <cxxtools/net/net.h>
#include <cxxtools/systemerror.h>
#include <cxxtools/selector.h>
#include <cxxtools/log.h>
#include <cerrno>
#include <cassert>
#include <cstring>
#include <sys/poll.h>
#include <unistd.h>

log_define("cxxtools.net.tcpserverimpl")

namespace cxxtools {

namespace net {

TcpServerImpl::TcpServerImpl(TcpServer& server)
: _server(server)
, _fd(-1)
, _pfd(0)
{

}


void TcpServerImpl::create(int domain, int type, int protocol)
{
  log_debug("create socket");
  _fd = ::socket(domain, type, protocol);
  if (_fd < 0)
    throw SystemError("socket");
}


void TcpServerImpl::close()
{
  if (_fd >= 0)
  {
    log_debug("close socket");
    ::close(_fd);
    _fd = -1;
     _pfd = 0;
  }
}


void TcpServerImpl::listen(const std::string& ipaddr, unsigned short int port, int backlog)
{
    log_debug("listen on " << ipaddr << " port " << port << " backlog " << backlog);

    AddrInfo ai(ipaddr, port);

    static const int reuseAddr = 1;

    // getaddrinfo() may return more than one addrinfo structure, so work
    // them all out, until we find a pretty useable one
    for (AddrInfoImpl::const_iterator it = ai.impl()->begin(); it != ai.impl()->end(); ++it)
    {
        try
        {
            this->create(it->ai_family, SOCK_STREAM, 0);
        }
        catch (const SystemError&)
        {
            continue;
        }

        log_debug("setsockopt SO_REUSEADDR");
        if (::setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, &reuseAddr, sizeof(reuseAddr)) < 0)
        {
            close();
            throw SystemError("setsockopt");
        }

        log_debug("bind");
        if (::bind(_fd, it->ai_addr, it->ai_addrlen) == 0)
        {
            // save our information
            std::memmove(&_servaddr, it->ai_addr, it->ai_addrlen);

            log_debug("listen");
            if( ::listen(_fd, backlog) < 0 )
            {
                close();

                if (errno == EADDRINUSE)
                    throw AddressInUse();
                else
                    throw SystemError("listen");
            }

            return;
        }
    }

    close();

    if (errno == EADDRINUSE)
        throw AddressInUse();
    else
        throw SystemError("bind");
}


bool TcpServerImpl::wait(std::size_t msecs)
{
    log_debug("wait " << msecs);

    struct pollfd fds;
    fds.fd = fd();
    fds.events = POLLIN;

    log_debug("poll timeout " << msecs);

    while (true)
    {
        int p = ::poll(&fds, 1, msecs);

        log_debug("poll returns " << p << " revents " << fds.revents);

        if (p > 0)
        {
            break;
        }
        else if (p < 0)
        {
            if (errno != EINTR)
            {
                log_error("error in poll; errno=" << errno);
                throw SystemError("poll");
            }
        }
        else if (p == 0)
        {
            log_debug("poll timeout (" << msecs << ')');
            throw Timeout();
        }
    }

    if (fds.revents | POLLIN)
        _server.connectionPending.send(_server);

    return fds.revents;
}


void TcpServerImpl::attach(SelectorBase& s)
{
    log_debug("attach to selector");
}


void TcpServerImpl::detach(SelectorBase& s)
{
    log_debug("detach from selector");

    if(_pfd)
        _pfd = 0;
}


std::size_t TcpServerImpl::pollSize() const
{
    return 1;
}


std::size_t TcpServerImpl::initializePoll(pollfd* pfd, std::size_t pollSize)
{
    assert(pfd != 0);
    assert(pollSize >= 1);

    log_debug("initializePoll " << pollSize);

    pfd->fd = this->fd();
    pfd->revents = 0;
    pfd->events = POLLIN;

    _pfd = pfd;

    return 1;
}


bool TcpServerImpl::checkPollEvent()
{
    assert(_pfd != 0);

    log_debug("checkPollEvent " << _pfd->revents);

    if( _pfd->revents & POLLIN )
    {
        _server.connectionPending.send(_server);
        return true;
    }

    return false;
}

} // namespace net

} // namespace cxxtools
