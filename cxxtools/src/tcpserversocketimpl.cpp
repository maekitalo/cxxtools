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

#include "addrinfo.h"
#include "tcpserversocketimpl.h"
#include <cxxtools/tcpserversocket.h>
#include <cxxtools/systemerror.h>
#include <cxxtools/selector.h>
#include <cxxtools/net.h> // AddrInUse
#include <cxxtools/log.h>
#include <cerrno>
#include <cassert>
#include <cstring>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/poll.h>
#include <sys/select.h>

log_define("cxxtools.net.tcp")

namespace cxxtools {

namespace net {

TcpServerSocketImpl::TcpServerSocketImpl(TcpServerSocket& server)
: m_fd(-1)
, _pfd(0)
, _server(server)
{

}


void TcpServerSocketImpl::create(int domain, int type, int protocol)
{
  log_debug("create socket");
  int m_fd = ::socket(domain, type, protocol);
  if (m_fd < 0)
    throw SystemError("socket");
}


void TcpServerSocketImpl::close()
{
  if (m_fd >= 0)
  {
    log_debug("close socket");
    ::close(m_fd);
    m_fd = -1;
     _pfd = 0;
  }
}


void TcpServerSocketImpl::listen(const std::string& ipaddr, unsigned short int port, int backlog)
{
  log_debug("listen on " << ipaddr << " port " << port << " backlog " << backlog);

  Addrinfo ai(ipaddr, port);

  int reuseAddr = 1;

  // getaddrinfo() may return more than one addrinfo structure, so work
  // them all out, until we find a pretty useable one
  for (Addrinfo::const_iterator it = ai.begin(); it != ai.end(); ++it)
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
    if (::setsockopt(m_fd, SOL_SOCKET, SO_REUSEADDR, &reuseAddr, sizeof(reuseAddr)) < 0)
      throw SystemError("setsockopt");

    log_debug("bind");
    if (::bind(m_fd, it->ai_addr, it->ai_addrlen) == 0)
    {
      // save our information
      std::memmove(&servaddr, it->ai_addr, it->ai_addrlen);

      log_debug("listen");
      if (::listen(m_fd, backlog) < 0)
      {
        if (errno == EADDRINUSE)
          throw AddressInUse();
        else
          throw SystemError("listen");
      }

    return;
    }
  }

  throw SystemError("bind");
}


bool TcpServerSocketImpl::wait(std::size_t msecs)
{
    log_debug("wait " << msecs);

    fd_set rfds;
    fd_set efds;
    FD_ZERO(&rfds);
    FD_ZERO(&efds);

    struct timeval* timeout = 0;
    struct timeval tv;
    if(msecs != Selector::WaitInfinite)
    {
        tv.tv_sec = msecs / 1000;
        tv.tv_usec = (msecs % 1000) * 1000;
        timeout = &tv;
    }

    if( this->fd() > 0 )
    {
        FD_SET(this->fd(), &rfds);
        FD_SET(this->fd(), &efds);
    }

    while( true )
    {
        int ret = ::select(this->fd() + 1, &rfds, 0, &efds, timeout);
        if( ret != -1 )
            break;

        if( errno != EINTR )
            throw IOError( "select failed" );
    }

    int avail = 0;

    // TODO: this can only be OOB data
    //if ( FD_ISSET(this->fd(), &efds) )
    //{
    //    ++avail;
    //}

    if( FD_ISSET(this->fd(), &rfds) )
    {
        _server.connectionPending.send(_server);
        ++avail;
    }

    return avail != 0;
}


void TcpServerSocketImpl::attach(SelectorBase& s)
{
    log_debug("attach to selector");
}


void TcpServerSocketImpl::detach(SelectorBase& s)
{
    log_debug("detach from selector");

    if(_pfd)
        _pfd = 0;
}


std::size_t TcpServerSocketImpl::pollSize() const
{
    return 1;
}


std::size_t TcpServerSocketImpl::initializePoll(pollfd* pfd, std::size_t pollSize)
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


bool TcpServerSocketImpl::checkPollEvent()
{
    assert(_pfd != 0);

    log_debug("checkPollEvent " << _pfd->revents);

    // only for output
    //if( _pfd->revents & (POLLERR | POLLNVAL) )
    //{
    //    return true;
    //}

    if( _pfd->revents & POLLIN )
    {
        _server.connectionPending.send(_server);
        return true;
    }

    return false;
}

} // namespace net

} // namespace cxxtools
