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
#if defined(HAVE_ACCEPT4) || defined(HAVE_SO_NOSIGPIPE) || defined(MSG_NOSIGNAL)
#include <sys/types.h>
#include <sys/socket.h>
#endif

#if !defined(MSG_MSG_NOSIGNAL)
#include <signal.h>
#endif

#include "tcpsocketimpl.h"
#include "tcpserverimpl.h"
#include <cxxtools/net/tcpserver.h>
#include <cxxtools/net/tcpsocket.h>
#include <cxxtools/systemerror.h>
#include <cxxtools/ioerror.h>
#include <cxxtools/log.h>
#include <cxxtools/join.h>
#include <cxxtools/hdstream.h>
#include "config.h"
#include "error.h"
#include <cerrno>
#include <cstring>
#include <cassert>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sstream>
#ifndef HAVE_INET_NTOP
#include "cxxtools/mutex.h"
#endif

log_define("cxxtools.net.tcpsocket.impl")

namespace cxxtools
{

namespace net
{

namespace
{
    std::string connectFailedMessage(AddrInfoImpl::const_iterator aip, int err)
    {
        std::ostringstream msg;
        msg << "failed to connect to <" << formatIp(*reinterpret_cast<const Sockaddr*>(aip->ai_addr))
            << ">: " << getErrnoString(err);
        return msg.str();
    }

}

void formatIp(const Sockaddr& sa, std::string& str)
{
#ifdef HAVE_INET_NTOP

#  ifdef HAVE_IPV6
      char strbuf[INET6_ADDRSTRLEN + 1];
#  else
      char strbuf[INET_ADDRSTRLEN + 1];
#  endif

      const char* p = 0;
 
      switch(sa.sa_in.sin_family)
      {
            case AF_INET:
                  p = inet_ntop(AF_INET, &sa.sa_in.sin_addr, strbuf, sizeof(strbuf));
                  break;
#  ifdef HAVE_IPV6
            case AF_INET6:
                  p = inet_ntop(AF_INET6, &sa.sa_in6.sin6_addr,
                        strbuf, sizeof(strbuf));
                  break;
#  endif
      }

      str = (p == 0 ? "-" : strbuf);

#else // HAVE_INET_NTOP

      static cxxtools::Mutex monitor;
      cxxtools::MutexLock lock(monitor);

      const char* p = inet_ntoa(sa.sa_in.sin_addr);
      if (p)
        str = p;
      else
        str.clear();

#endif
}

std::string getSockAddr(int fd)
{
    Sockaddr addr;

    socklen_t slen = sizeof(addr);
    if (::getsockname(fd, &addr.sa, &slen) < 0)
        throw SystemError("getsockname");

    std::string ret;
    formatIp(addr, ret);
    return ret;
}

std::string TcpSocketImpl::connectFailedMessages()
{
    std::ostringstream msg;
    msg << "while trying to connect to ";
    if (_addrInfo.host().empty())
        msg << "local host";
    else
        msg << "host \"" << _addrInfo.host() << '"';
    msg << " port " << _addrInfo.port() << ": ";
    join(_connectFailedMessages.begin(), _connectFailedMessages.end(), " / ", msg);
    _connectFailedMessages.clear();
    return msg.str();
}

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
{ return net::getSockAddr(fd()); }

std::string TcpSocketImpl::getPeerAddr() const
{
    Sockaddr addr;

    addr.storage = _peeraddr;

    std::string ret;
    formatIp(addr, ret);
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
        log_debug("connected successfully to " << getPeerAddr());
        _isConnected = true;
    }

    return sockerr;
}

void TcpSocketImpl::checkPendingError()
{
    if (!_connectResult.empty())
    {
        _connectFailedMessages.push_back(_connectResult);
        _connectResult.clear();
        throw IOError(connectFailedMessages());
    }
}


std::string TcpSocketImpl::tryConnect()
{
    log_trace("tryConnect");

    assert(_fd == -1);

    if (_addrInfoPtr == _addrInfo.impl()->end())
    {
        log_debug("no more address informations");
        std::ostringstream msg;
        msg << "invalid address information; host \"" << _addrInfo.host() << "\" port " << _addrInfo.port();
        return msg.str();
    }

    while (true)
    {
        int fd;
        while (true)
        {
            log_debug("create socket for ip <" << formatIp(*reinterpret_cast<const Sockaddr*>(_addrInfoPtr->ai_addr)) << '>');
            fd = ::socket(_addrInfoPtr->ai_family, SOCK_STREAM, 0);
            if (fd >= 0)
                break;

            log_debug("failed to create socket: " << getErrnoString());

            AddrInfoImpl::const_iterator aip = _addrInfoPtr;
            if (++_addrInfoPtr == _addrInfo.impl()->end())
                return connectFailedMessage(aip, errno);
        }

#ifdef HAVE_SO_NOSIGPIPE
        static const int on = 1;
        if (::setsockopt(fd, SOL_SOCKET, SO_NOSIGPIPE, &on, sizeof(on)) < 0)
            throw cxxtools::SystemError("setsockopt(SO_NOSIGPIPE)");
#endif

        IODeviceImpl::open(fd, true, false);

        std::memmove(&_peeraddr, _addrInfoPtr->ai_addr, _addrInfoPtr->ai_addrlen);

        log_debug("created socket " << _fd << " max: " << FD_SETSIZE);
        log_debug("connect to port " << _addrInfo.port());

        if( ::connect(this->fd(), _addrInfoPtr->ai_addr, _addrInfoPtr->ai_addrlen) == 0 )
        {
            _isConnected = true;
            log_debug("connected successfully to " << getPeerAddr());
            break;
        }

        if (errno == EINPROGRESS)
        {
            log_debug("connect in progress");
            break;
        }

        close();
        AddrInfoImpl::const_iterator aip = _addrInfoPtr;
        if (++_addrInfoPtr == _addrInfo.impl()->end())
            return connectFailedMessage(aip, errno);
    }

    return std::string();
}


bool TcpSocketImpl::beginConnect(const AddrInfo& addrInfo)
{
    log_trace("begin connect");

    assert(!_isConnected);

    _connectFailedMessages.clear();
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

    checkPendingError();

    if( _isConnected )
        return;

    try
    {
        while (true)
        {
            pollfd pfd;
            pfd.fd = this->fd();
            pfd.revents = 0;
            pfd.events = POLLOUT;

            log_debug("wait " << timeout());
            bool avail = this->wait(this->timeout(), pfd);

            if (avail)
            {
                // something has happened
                int sockerr = checkConnect();
                if (_isConnected)
                {
                    _connectFailedMessages.clear();
                    return;
                }

                _connectFailedMessages.push_back(connectFailedMessage(_addrInfoPtr, sockerr));
                log_debug("connect failed: " << _connectFailedMessages.back());

                if (++_addrInfoPtr == _addrInfo.impl()->end())
                {
                    // no more addrInfo - propagate error
                    throw IOError(connectFailedMessages());
                }
            }
            else if (++_addrInfoPtr == _addrInfo.impl()->end())
            {
                log_debug("timeout");
                throw IOTimeout();
            }

            close();

            _connectResult = tryConnect();
            if (_isConnected)
            {
                _connectResult.clear();
                _connectFailedMessages.clear();
                return;
            }
            checkPendingError();
        }
    }
    catch(...)
    {
        close();
        throw;
    }
}


void TcpSocketImpl::accept(const TcpServer& server, unsigned flags)
{
    socklen_t peeraddr_len = sizeof(_peeraddr);

    _fd = server.impl().accept(flags, reinterpret_cast <struct sockaddr*>(&_peeraddr), peeraddr_len);

    if( _fd < 0 )
        throw SystemError("accept");

#ifdef HAVE_ACCEPT4
    IODeviceImpl::open(_fd, false, false);
#else
    bool inherit = (flags & TcpSocket::INHERIT) != 0;
    IODeviceImpl::open(_fd, true, inherit);
#endif
    //TODO ECONNABORTED EINTR EPERM

    _isConnected = true;
    log_debug( "accepted from " << getPeerAddr());
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


bool TcpSocketImpl::checkPollEvent(pollfd& pfd)
{
    log_debug("checkPollEvent " << pfd.revents);

    if (_isConnected)
    {
        // check for error while neither reading nor writing
        //
        // if reading or writing, IODeviceImpl::checkPollEvent will emit
        // inputReady or outputReady signal and the user gets an exception in
        // endRead or endWrite.
        if ( !_device.reading() && !_device.writing()
            && (pfd.revents & POLLERR) )
        {
            _device.close();
            _socket.closed(_socket);
            return true;
        }

        return IODeviceImpl::checkPollEvent(pfd);
    }

    if (pfd.revents & POLLERR)
    {
        int sockerr;
        socklen_t optlen = sizeof(sockerr);

        // check for socket error
        if( ::getsockopt(this->fd(), SOL_SOCKET, SO_ERROR, &sockerr, &optlen) != 0 )
            throw SystemError("getsockopt");

        _connectFailedMessages.push_back(connectFailedMessage(_addrInfoPtr, sockerr));

        AddrInfoImpl::const_iterator ptr = _addrInfoPtr;
        if (++ptr == _addrInfo.impl()->end())
        {
            // not really connected but error
            // end of addrinfo list means that no working addrinfo was found
            log_debug("no more addrinfos found");
            _socket.connected(_socket);
            return true;
        }
        else
        {
            _addrInfoPtr = ptr;

            close();
            _connectResult = tryConnect();

            if (_isConnected || !_connectFailedMessages.empty())
            {
                // immediate success or error
                log_debug("connected successfully");
                _socket.connected(_socket);
            }
            else
            {
                // by closing the previous file handle _pfd is set to 0.
                // creating a new socket in tryConnect may also change the value of fd.
                initializePoll(&pfd, 1);
            }

            return true;
        }
    }
    else if( pfd.revents & POLLOUT )
    {
        int sockerr = checkConnect();
        if (_isConnected)
        {
            _socket.connected(_socket);
            return true;
        }

        // something went wrong - look for next addrInfo
        _connectFailedMessages.push_back(connectFailedMessage(_addrInfoPtr, sockerr));
        log_debug("connect failed: " << _connectFailedMessages.back());
        if (++_addrInfoPtr == _addrInfo.impl()->end())
        {
            // no more addrInfo - propagate error
            _connectResult = connectFailedMessages();
            _socket.connected(_socket);
            return true;
        }

        close();
        _connectResult = tryConnect();
        if (_isConnected)
        {
            _socket.connected(_socket);
            return true;
        }
    }

    return false;
}

namespace
{
    size_t callSend(int fd, const char* buffer, size_t n)
    {
        log_debug("::send(" << fd << ", buffer, " << n << ')');
        log_finer(HexDump(buffer, n));

#if defined(HAVE_MSG_NOSIGNAL)

        ssize_t ret;
        do {
            ret = ::send(fd, (const void*)buffer, n, MSG_NOSIGNAL);
        } while (ret == -1 && errno == EINTR);

#elif defined(HAVE_SO_NOSIGPIPE)

        ssize_t ret;
        do {
            ret = ::send(fd, (const void*)buffer, n, 0);
        } while (ret == -1 && errno == EINTR);

#else

        // block SIGPIPE
        sigset_t sigpipeMask, oldSigmask;
        sigemptyset(&sigpipeMask);
        sigaddset(&sigpipeMask, SIGPIPE);
        pthread_sigmask(SIG_BLOCK, &sigpipeMask, &oldSigmask);

        // execute send
        ssize_t ret;
        eo {
            ret = ::send(fd, (const void*)buffer, n, 0);
        } while (ret == -1 && errno == EINTR);

        // clear possible SIGPIPE
        sigset_t pending;
        sigemptyset(&pending);
        sigpending(&pending);
        if (sigismember(&pending, SIGPIPE))
        {
          static const struct timespec nowait = { 0, 0 };
          while (sigtimedwait(&sigpipeMask, 0, &nowait) == -1 && errno == EINTR)
            ;
        }

        // unblock SIGPIPE
        pthread_sigmask(SIG_SETMASK, &oldSigmask, 0);

#endif

        log_debug("send returned " << ret);
        if (ret > 0)
            return static_cast<size_t>(ret);

        if (errno == ECONNRESET || errno == EPIPE)
            throw IOError("lost connection to peer");

        return 0;
    }
}


size_t TcpSocketImpl::beginWrite(const char* buffer, size_t n)
{
    size_t ret = callSend(_fd, buffer, n);

    if (ret > 0)
        return ret;

    if (_pfd)
        _pfd->events |= POLLOUT;

    return 0;
}


size_t TcpSocketImpl::write(const char* buffer, size_t n)
{
    ssize_t ret = 0;

    while (true)
    {
        ret = callSend(_fd, buffer, n);
        if (ret > 0)
            break;

        if (errno != EAGAIN)
            throw IOError(getErrnoString("Could not write to file handle"));

        pollfd pfd;
        pfd.fd = _fd;
        pfd.revents = 0;
        pfd.events = POLLOUT;

        if (!wait(_timeout, pfd))
            throw IOTimeout();
    }

    return static_cast<size_t>(ret);
}

void TcpSocketImpl::inputReady()
{
    // check for ssl - may need to call ssl_read or ssl_write (when it returned with SSL_ERROR_WANT_READ)
    IODeviceImpl::inputReady();
}

void TcpSocketImpl::outputReady()
{
    // check for ssl - may need to call ssl_read (on SSL_ERROR_WANT_WRITE) or ssl_write
    IODeviceImpl::outputReady();
}

size_t TcpSocketImpl::read(char* buffer, size_t count, bool& eof)
{
    // TODO check for ssl mode
    return IODeviceImpl::read(buffer, count, eof);
}

void TcpSocketImpl::sslStart()
{
    // TODO
}

void TcpSocketImpl::sslStop()
{
    // TODO
}

} // namespace net

} // namespace cxxtools
