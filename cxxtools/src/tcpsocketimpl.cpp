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
: IODeviceImpl(socket)
, _socket(socket)
, _isConnected(false)
{
}


TcpSocketImpl::~TcpSocketImpl()
{
    assert(_pfd == 0);
}


void TcpSocketImpl::close()
{
    log_debug("close socket " << _fd);
    IODeviceImpl::close();
    _isConnected = false;
}


std::string TcpSocketImpl::getSockAddr() const
{
    struct sockaddr_storage addr;

    socklen_t slen = sizeof(addr);
    if (::getsockname(fd(), reinterpret_cast<struct sockaddr*>(&addr), &slen) < 0)
        throw SystemError("getsockname");

    std::string ret;
    formatIp(addr, ret);
    return ret;
}


std::string TcpSocketImpl::getPeerAddr() const
{
    std::string ret;
    formatIp(_peeraddr, ret);
    return ret;
}


void TcpSocketImpl::connect(const std::string& ipaddr, unsigned short int port)
{
    log_debug("connect to " << ipaddr << " port " << port);
    this->beginConnect(ipaddr, port);
    this->endConnect();
}


bool TcpSocketImpl::beginConnect(const std::string& ipaddr, unsigned short int port)
{
    log_debug("begin connect to " << ipaddr << " port " << port);

    AddrInfo ai(ipaddr, port);

    log_debug("checking address information");
    for (AddrInfo::const_iterator it = ai.begin(); it != ai.end(); ++it)
    {
        int fd = ::socket(it->ai_family, SOCK_STREAM, 0);
        if (fd < 0)
            continue;

        IODeviceImpl::open(fd, true);

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

    if(_pfd && ! _socket.wbuf())
    {
        _pfd->events &= ~POLLOUT;
    }

    if( ! _isConnected )
    {
        try
        {
            pollfd pfd;
            pfd.fd = this->fd();
            pfd.revents = 0;
            pfd.events = POLLOUT;

            log_debug("wait " << timeout() << " ms");
            bool ret = this->wait(this->timeout(), pfd);
            if(false == ret)
            {
                log_debug("timeout");
                throw IOTimeout();
            }

            log_debug("connect");
            if( ::connect(this->fd(), reinterpret_cast<const sockaddr*>(&_peeraddr), sizeof(_peeraddr)) == 0 )
            {
                _isConnected = true;
                log_debug("connected successfuly");
                return;
            }

            log_debug("get error");

            int sockerr;
            socklen_t optlen = sizeof(sockerr);
            if( ::getsockopt(this->fd(), SOL_SOCKET, SO_ERROR, &sockerr, &optlen) != 0 )
            {
                throw SystemError("getsockopt");
            }

            throw SystemError("connect");
        }
        catch(...)
        {
            close();
            throw;
        }
    }
}


void TcpSocketImpl::accept(const TcpServer& server)
{
    socklen_t peeraddr_len = sizeof(_peeraddr);

    log_debug( "accept " << server.impl().fd() );
    _fd = ::accept(server.impl().fd(), reinterpret_cast <struct sockaddr*>(&_peeraddr), &peeraddr_len);
    if( _fd < 0 )
      throw SystemError("accept");


    IODeviceImpl::open(_fd, true);
    //TODO ECONNABORTED EINTR EPERM

    _isConnected = true;
    log_debug( "accepted " << server.impl().fd() << " => " << _fd );
}


void TcpSocketImpl::initWait(pollfd& pfd)
{
    IODeviceImpl::initWait(pfd);

    if( ! _isConnected )
    {
        log_debug("not connected, setting POLLOUT ");
        pfd.events = POLLOUT;
    }
}


std::size_t TcpSocketImpl::initializePoll(pollfd* pfd, std::size_t pollSize)
{
    assert(pfd != 0);
    assert(pollSize >= 1);

    log_debug("TcpSocketImpl::initializePoll " << pollSize);

    std::size_t ret = IODeviceImpl::initializePoll(pfd, pollSize);
    assert(ret == 1);

    if( ! _isConnected )
    {
        log_debug("not connected, setting POLLOUT ");
        pfd->events = POLLOUT;
    }

    return ret;
}


bool TcpSocketImpl::checkPollEvent(pollfd& pfd)
{
    log_debug("checkPollEvent " << pfd.revents);

    if( pfd.revents & POLLOUT )
    {
        if( ! _isConnected )
        {
            _socket.connected.send(_socket);
            return true;
        }
    }

    return IODeviceImpl::checkPollEvent(pfd);
}

} // namespace net

} // namespace cxxtools
