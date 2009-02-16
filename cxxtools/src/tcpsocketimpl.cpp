/*
 * Copyright (C) 2009 Marc Boris Duerner, Tommi Maekitalo
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
#include "tcpsocketimpl.h"
#include "tcpserverimpl.h"
#include "cxxtools/tcpserver.h"
#include "cxxtools/tcpsocket.h"
#include "cxxtools/systemerror.h"
#include "cxxtools/ioerror.h"
#include "cxxtools/log.h"
#include "config.h"
#include <cerrno>
#include <cstring>
#include <cassert>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>

log_define("cxxtools.net.tcp")

namespace {

    void formatIp(const sockaddr_storage& addr, std::string& str)
    {
#ifndef HAVE_INET_NTOP
        static cxxtools::Mutex monitor;
        cxxtools::MutexLock lock(monitor);

        const sockaddr_in* sa = reinterpret_cast<const sockaddr_in*>(&addr);
        const char* p = inet_ntoa(sa->sin_addr);
        if (p)
            str = p;
        else
            str.clear();
#else
        const sockaddr_in* sa = reinterpret_cast<const sockaddr_in*>(&addr);
        char strbuf[INET6_ADDRSTRLEN + 1];
        const char* p = inet_ntop(sa->sin_family, &sa->sin_addr, strbuf, sizeof(strbuf));
        str = (p == 0 ? "-" : strbuf);
#endif
    }
}

namespace cxxtools {

namespace net {

TcpSocketImpl::TcpSocketImpl(TcpSocket& socket)
: _socket(socket)
, _isConnected(false)
, _fd(-1)
, _timeout(Selectable::WaitInfinite)
, _pfd(0)
{
}


TcpSocketImpl::~TcpSocketImpl()
{
    assert(_pfd == 0);
}


void TcpSocketImpl::close()
{
  if (_fd != -1)
  {
    log_debug("close socket " << _fd);
    ::close(_fd);
    _fd = -1;
    _isConnected = false;
    _pfd = 0;
  }
}


std::string TcpSocketImpl::getSockAddr() const
{
    return "";
}


void TcpSocketImpl::connect(const std::string& ipaddr, unsigned short int port)
{
    // TODO: decide whether this should send the "connected" signal
    log_debug("connect to " << ipaddr << " port " << port);

    bool isConnected = this->beginConnect(ipaddr, port);
    if( ! isConnected )
    {
        bool ret = this->wait(_timeout);
        if(false == ret)
        {
            close();
            throw IOTimeout();
        }

        this->endConnect();
    }
}


bool TcpSocketImpl::beginConnect(const std::string& ipaddr, unsigned short int port)
{
    log_debug("begin connect to " << ipaddr << " port " << port);

    AddrInfo ai(ipaddr, port);

    log_debug("checking address information");
    for (AddrInfo::const_iterator it = ai.begin(); it != ai.end(); ++it)
    {
        _fd = ::socket(it->ai_family, SOCK_STREAM, 0);
        if (_fd < 0)
            continue;

        int flags = fcntl(_fd, F_GETFL);
        flags |= O_NONBLOCK ;
        fcntl(_fd, F_SETFL, O_NONBLOCK);

        std::memmove(&_peeraddr, it->ai_addr, it->ai_addrlen);

        log_debug("created socket " << _fd << " max: " << FD_SETSIZE);

        if( ::connect(this->fd(), it->ai_addr, it->ai_addrlen) == 0 )
        {
            _isConnected = true;
            log_debug("connected successfuly");
            return true;
        }

        if(errno != EINPROGRESS)
        {
            close();
            throw SystemError("connect failed");
        }

        log_debug("connect in progress");
        memmove(&_peeraddr, it->ai_addr, it->ai_addrlen);
        return false;
    }

    throw SystemError("invalid address information");
}


void TcpSocketImpl::endConnect()
{
    log_debug("ending connect");

    if(_pfd)
    {
        _pfd->events &= ~POLLOUT;
    }

    int sockerr;
    socklen_t optlen = sizeof(sockerr);
    if( ::getsockopt(this->fd(), SOL_SOCKET, SO_ERROR, &sockerr, &optlen) != 0 )
    {
        this->close();
        throw SystemError("getsockopt");
    }

    if(sockerr != 0)
    {
        this->close();
        throw SystemError("connect");
    }

    _isConnected = true;
}


void TcpSocketImpl::accept(TcpServer& server)
{
    socklen_t peeraddr_len = sizeof(_peeraddr);

    log_debug( "accept " << server.impl().fd() );
    _fd = ::accept(server.impl().fd(), reinterpret_cast <struct sockaddr*>(&_peeraddr), &peeraddr_len);
    if( _fd < 0 )
      throw SystemError("accept");

    //TODO ECONNABORTED EINTR EPERM

    log_debug( "accepted " << server.impl().fd() << " => " << _fd );
}


size_t TcpSocketImpl::beginRead(char* buffer, size_t n, bool& eof)
{
    return 0;
}


size_t TcpSocketImpl::endRead(bool& eof)
{
    return 0;
}


size_t TcpSocketImpl::read(char* buffer, size_t count, bool& eof)
{
    return 0;
}


size_t TcpSocketImpl::beginWrite(const char* buffer, size_t n)
{
    return 0;
}


size_t TcpSocketImpl::endWrite()
{
    return 0;
}


size_t TcpSocketImpl::write(const char* buffer, size_t count)
{
    return 0;
}


bool TcpSocketImpl::wait(std::size_t msecs)
{
    log_debug("wait " << msecs);

    if( this->fd() > FD_SETSIZE )
    {
        throw IOError( CXXTOOLS_ERROR_MSG("FD_SETSIZE too small for fd") );
    }

    struct timeval* timeout = 0;
    struct timeval tv;
    if(msecs != Selector::WaitInfinite)
    {
        tv.tv_sec = msecs / 1000;
        tv.tv_usec = (msecs % 1000) * 1000;
        timeout = &tv;
    }

    fd_set rfds;
    fd_set wfds;
    fd_set efds;
    FD_ZERO(&rfds);
    FD_ZERO(&wfds);
    FD_ZERO(&efds);
    if( this->fd() > 0 )
    {
        FD_SET(this->fd(), &wfds);
    }

    while( true )
    {
        int ret = ::select(this->fd() + 1, 0, &wfds, 0, timeout);
        if( ret != -1 )
            break;

        if( errno != EINTR )
            throw IOError( "select failed" );
    }

    if( FD_ISSET(this->fd(), &wfds) )
    {
        if( ! _isConnected )
        {
            _socket.connected.send(_socket);
            return true;
        }
    }

    return false;
}


void TcpSocketImpl::attach(SelectorBase& sb)
{
    log_debug("attach to selector");
}


void TcpSocketImpl::detach(SelectorBase& sb)
{
    log_debug("detach from selector " << _fd);
    if(_pfd)
        _pfd = 0;
}


std::size_t TcpSocketImpl::pollSize() const
{
    return 1;
}


std::size_t TcpSocketImpl::initializePoll(pollfd* pfd, std::size_t pollSize)
{
    assert(pfd != 0);
    assert(pollSize >= 1);

    log_debug("initializePoll " << pollSize);

    pfd->fd = this->fd();
    pfd->revents = 0;

    if( ! _isConnected )
    {
        log_debug("not connected, setting POLLOUT ");
        pfd->events = POLLOUT;
    }

    _pfd = pfd;
    return 1;
}


bool TcpSocketImpl::checkPollEvent()
{
    assert(_pfd != 0);

    log_debug("checkPollEvent " << _pfd->revents);

    if( _pfd->revents & POLLOUT )
    {
        if( ! _isConnected )
        {
            _socket.connected.send(_socket);
            return true;
        }
    }

    return false;
}

} // namespace net

} // namespace cxxtools
