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

#include "config.h"
#ifdef HAVE_ACCEPT4
#include <sys/socket.h>
#endif

#include "tcpsocketimpl.h"
#include "tcpserverimpl.h"
#include "cxxtools/net/tcpserver.h"
#include "cxxtools/net/tcpsocket.h"
#include "cxxtools/systemerror.h"
#include "cxxtools/ioerror.h"
#include "cxxtools/log.h"
#include "config.h"
#include "error.h"
#include <cerrno>
#include <cstring>
#include <cassert>
#include <fcntl.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>

log_define("cxxtools.net.tcpsocketimpl")

namespace {

    void formatIp(const sockaddr_storage& addr, std::string& str)
    {
#ifdef HAVE_INET_NTOP
        const sockaddr_in* sa = reinterpret_cast<const sockaddr_in*>(&addr);
        char strbuf[INET6_ADDRSTRLEN + 1];
        const char* p = inet_ntop(sa->sin_family, &sa->sin_addr, strbuf, sizeof(strbuf));
        str = (p == 0 ? "-" : strbuf);
#else
        static cxxtools::Mutex monitor;
        cxxtools::MutexLock lock(monitor);

        const sockaddr_in* sa = reinterpret_cast<const sockaddr_in*>(&addr);
        const char* p = inet_ntoa(sa->sin_addr);
        if (p)
            str = p;
        else
            str.clear();
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


void TcpSocketImpl::connect(const AddrInfo& addrInfo)
{
    log_debug("connect");
    this->beginConnect(addrInfo);
    this->endConnect();
}


int TcpSocketImpl::checkConnect()
{
    log_trace("checkConnect");

    int sockerr;
    socklen_t optlen = sizeof(sockerr);

    // check for socket error
    if( ::getsockopt(this->fd(), SOL_SOCKET, SO_ERROR, &sockerr, &optlen) != 0 )
    {
        // getsockopt failed
        int e = errno;
        close();
        throw SystemError(e, "getsockopt");
    }

    if (sockerr == 0)
    {
        log_debug("connected successfully");
        _isConnected = true;
    }

    return sockerr;
}

void TcpSocketImpl::checkPendingError()
{
    if (_connectResult.second)
    {
        if (_connectResult.first)
        {
            throw IOError(getErrnoString(_connectResult.first, _connectResult.second).c_str());
        }
        else
        {
            throw IOError("invalid address information");
        }
    }
}


std::pair<int, const char*> TcpSocketImpl::tryConnect()
{
    log_trace("tryConnect");

    if (_addrInfoPtr == _addrInfo.impl()->end())
    {
        log_debug("no more address informations");
        return std::pair<int, const char*>(0, "invalid address information");
    }

    while (true)
    {
        int fd;
        while (true)
        {
            log_debug("create socket");
            fd = ::socket(_addrInfoPtr->ai_family, SOCK_STREAM, 0);
            if (fd >= 0)
                break;

            if (++_addrInfoPtr == _addrInfo.impl()->end())
                return std::pair<int, const char*>(errno, "socket");
        }

        IODeviceImpl::open(fd, true, false);

        std::memmove(&_peeraddr, _addrInfoPtr->ai_addr, _addrInfoPtr->ai_addrlen);

        log_debug("created socket " << _fd << " max: " << FD_SETSIZE);

        if( ::connect(this->fd(), _addrInfoPtr->ai_addr, _addrInfoPtr->ai_addrlen) == 0 )
        {
            _isConnected = true;
            log_debug("connected successfully");
            break;
        }

        if (errno == EINPROGRESS)
        {
            log_debug("connect in progress");
            memmove(&_peeraddr, _addrInfoPtr->ai_addr, _addrInfoPtr->ai_addrlen);
            break;
        }

        close();
        if (++_addrInfoPtr == _addrInfo.impl()->end())
            return std::pair<int, const char*>(errno, "socket");
    }

    return std::pair<int, const char*>(0, 0);
}


bool TcpSocketImpl::beginConnect(const AddrInfo& addrInfo)
{
    log_trace("begin connect");

    if (_isConnected)
    {
        // TODO throw exception instead?
        close();
        _isConnected = false;
    }

    _addrInfo = addrInfo;
    _addrInfoPtr = _addrInfo.impl()->begin();
    _connectResult = tryConnect();
    checkPendingError();
    return _isConnected;
}


void TcpSocketImpl::endConnect()
{
    log_trace("ending connect");

    if(_pfd && ! _socket.wbuf())
    {
        _pfd->events &= ~POLLOUT;
    }

    if( ! _isConnected )
    {
        try
        {
            checkPendingError();

            while (true)
            {
                int sockerr = checkConnect();
                if (_isConnected)
                    return;

                if (sockerr != EINPROGRESS)
                {
                    // something went wrong - look for next addrInfo
                    log_debug("sockerr is " << sockerr << " try next");
                    if (++_addrInfoPtr == _addrInfo.impl()->end())
                    {
                        // no more addrInfo - propagate error
                        throw IOError(getErrnoString(sockerr, "connect").c_str());
                    }

                    _connectResult = tryConnect();
                    if (_isConnected)
                        return;
                    checkPendingError();
                }

                pollfd pfd;
                pfd.fd = this->fd();
                pfd.revents = 0;
                pfd.events = POLLOUT;

                log_debug("wait " << timeout() << " ms");
                bool ret = this->wait(this->timeout(), pfd);
                if(false == ret)
                {
                    // nothing has happened in time
                    log_debug("timeout");
                    throw IOTimeout();
                }
            }
        }
        catch(...)
        {
            close();
            throw;
        }
    }
}


void TcpSocketImpl::accept(const TcpServer& server, bool closeOnExec)
{
    socklen_t peeraddr_len = sizeof(_peeraddr);

    log_debug( "accept " << server.impl().fd() );

#ifdef HAVE_ACCEPT4
    int fd = SOCK_NONBLOCK;
    if (closeOnExec)
      fd |= SOCK_CLOEXEC;
    _fd = ::accept4(server.impl().fd(), reinterpret_cast <struct sockaddr*>(&_peeraddr), &peeraddr_len, SOCK_NONBLOCK | SOCK_CLOEXEC);
#else
    _fd = ::accept(server.impl().fd(), reinterpret_cast <struct sockaddr*>(&_peeraddr), &peeraddr_len);
#endif

    if( _fd < 0 )
      throw SystemError("accept");

#ifdef HAVE_ACCEPT4
    IODeviceImpl::open(_fd, false, false);
#else
    IODeviceImpl::open(_fd, true, closeOnExec);
#endif
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

    if( ! _isConnected )
    {
        if ( pfd.revents & POLLERR )
        {
            AddrInfoImpl::const_iterator ptr = _addrInfoPtr;
            if (++ptr == _addrInfo.impl()->end())
            {
                // not really connected but error
                // end of addrinfo list means that no working addrinfo was found
                _socket.connected.send(_socket);
                return true;
            }
            else
            {
                _addrInfoPtr = ptr;

                close();
                _connectResult = tryConnect();

                if (_isConnected || _connectResult.second)
                    // immediate success or error
                    _socket.connected.send(_socket);
                else
                    // by closing the previous file handle _pfd is set to 0.
                    // creating a new socket in tryConnect may also change the value of fd.
                    initializePoll(&pfd, 1);

                return _isConnected;
            }
        }
        else if( pfd.revents & POLLOUT )
        {
            _socket.connected.send(_socket);
            return true;
        }
        else
            return false;
    }

    return IODeviceImpl::checkPollEvent(pfd);
}

} // namespace net

} // namespace cxxtools
