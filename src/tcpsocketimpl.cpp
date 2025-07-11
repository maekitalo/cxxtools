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
#include <netinet/tcp.h>

#if !defined(MSG_MSG_NOSIGNAL)
#include <signal.h>
#endif

#include "tcpsocketimpl.h"
#include "tcpserverimpl.h"
#include "sslcertificateimpl.h"
#include <cxxtools/net/tcpserver.h>
#include <cxxtools/net/tcpsocket.h>
#include <cxxtools/systemerror.h>
#include <cxxtools/ioerror.h>
#include <cxxtools/log.h>
#include <cxxtools/join.h>
#include <cxxtools/hexdump.h>
#include <cxxtools/resetter.h>

#include "error.h"
#include <cerrno>
#include <cassert>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sstream>

#include <openssl/err.h>
#include <openssl/ssl.h>
#include <cxxtools/sslctx.h>
#include "sslctximpl.h"

#include <unistd.h>
#include <cstring>

log_define("cxxtools.net.tcpsocket.impl")
log_define_instance(ssl, "cxxtools.net.tcpsocket.impl.ssl")

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
                p = inet_ntop(AF_INET6, &sa.sa_in6.sin6_addr, strbuf, sizeof(strbuf));
                break;
#  endif
            case AF_UNIX:
                str = sa.sa_un.sun_path;
                return;
      }

      str = (p == 0 ? "-" : strbuf);

#else // HAVE_INET_NTOP

      static Mutex monitor;
      MutexLock lock(monitor);

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

void TcpSocketImpl::checkSslOperation(int ret, const char* fn, pollfd* pfd)
{
    int err = SSL_get_error(_ssl, ret);

    log_debug_to(ssl, "check ssl operation ret=" << ret << " err=" << err);

    switch (err)
    {
        case SSL_ERROR_NONE:
            break;

        case SSL_ERROR_ZERO_RETURN:
            log_debug_to(ssl, "SSL_ERROR_ZERO_RETURN");
            _state = CONNECTED;
            _socket.sslClosed(_socket);
            break;

        case SSL_ERROR_WANT_READ:
            log_debug_to(ssl, "SSL_ERROR_WANT_READ");
            if (pfd)
            {
                pfd->events |= POLLIN;
                pfd->events &= ~POLLOUT;
            }
            break;

        case SSL_ERROR_WANT_WRITE:
        case SSL_ERROR_WANT_CONNECT:
        case SSL_ERROR_WANT_ACCEPT:
            log_debug_to(ssl, "SSL_ERROR_WANT_WRITE");
            if (pfd)
            {
                pfd->events |= POLLOUT;
                pfd->events &= ~POLLIN;
            }
            break;

        case SSL_ERROR_WANT_X509_LOOKUP:
            log_debug_to(ssl, "SSL_ERROR_WANT_X509_LOOKUP");
            break;

        case SSL_ERROR_SYSCALL:
            log_debug_to(ssl, "SSL_ERROR_SYSCALL; errno=" << errno);
            SslError::checkSslError();
            if (ret == 0)
            {
                close();
                _socket.closed(_socket);
                throw IOError("lost connection to peer in ssl operation");
            }
            else
            {
                int e = errno;
                close();
                throw SystemError(e, fn);
            }

        case SSL_ERROR_SSL:
            log_debug_to(ssl, "SSL_ERROR_SSL");
            SslError::checkSslError();
            break;
    }
}

void TcpSocketImpl::waitSslOperation(int ret, cxxtools::Timespan timeout)
{
    pollfd pfd;
    pfd.fd = _fd;
    pfd.revents = 0;
    pfd.events = 0;

    checkSslOperation(ret, 0, &pfd);

    log_debug_to(ssl, "wait " << timeout << " fd " << _fd);
    bool avail = wait(timeout, pfd);
    if (!avail)
    {
        log_debug_to(ssl, "IOTimeout " << _fd);
        throw IOTimeout();
    }
}

void TcpSocketImpl::initSsl(const SslCtx& sslCtx)
{
    if (_ssl)
        return;

    log_debug_to(ssl, "SSL_new(" << static_cast<void*>(sslCtx.impl()->ctx()) << ')');
    _ssl = SSL_new(sslCtx.impl()->ctx());
    SslError::checkSslError();

    log_debug_to(ssl, "SSL_set_fd(" << _ssl << ", " << _fd << ')');
    SSL_set_fd(_ssl, _fd);
}

TcpSocketImpl::TcpSocketImpl(TcpSocket& socket)
: IODeviceImpl(socket),
  _socket(socket),
  _state(IDLE),
  _sentry(0),
  _ssl(0),
  _peerCertificateLoaded(false)
{
}


TcpSocketImpl::~TcpSocketImpl()
{
    assert(_pfd == 0);

    if (_ssl)
        SSL_free(_ssl);

    if (_sentry)
        _sentry->detach();
}


void TcpSocketImpl::close()
{
    log_debug("close socket " << _fd);
    IODeviceImpl::close();
    _state = IDLE;
    _peerCertificate.clear();
    _peerCertificateLoaded = false;
    if (_ssl)
    {
        log_debug("SSL_free");
        SSL_free(_ssl);
        _ssl = 0;
    }
}


std::string TcpSocketImpl::getSockAddr() const
{ return net::getSockAddr(_fd); }

std::string TcpSocketImpl::getPeerAddr() const
{
    Sockaddr addr;

    addr.storage = _peeraddr;

    std::string ret;
    formatIp(addr, ret);
    return ret;
}


int TcpSocketImpl::checkConnect()
{
    log_trace("checkConnect");

    int sockerr;
    socklen_t optlen = sizeof(sockerr);

    // check for socket error
    if (::getsockopt(_fd, SOL_SOCKET, SO_ERROR, &sockerr, &optlen) != 0)
    {
        // getsockopt failed
        int e = errno;
        close();
        throw SystemError(e, "getsockopt");
    }

    if (sockerr == 0)
    {
        log_debug("connected successfully to " << getPeerAddr());
        _connectFailedMessages.clear();
        _state = CONNECTED;
    }

    return sockerr;
}

void TcpSocketImpl::checkPendingError()
{
    if (!_connectResult.empty())
    {
        _connectFailedMessages.push_back(_connectResult);
        _connectResult.clear();
    }

    if (!_connectFailedMessages.empty())
        throw IOError(connectFailedMessages());
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

        static const int on = 1;
#ifdef HAVE_SO_NOSIGPIPE
        if (::setsockopt(fd, SOL_SOCKET, SO_NOSIGPIPE, &on, sizeof(on)) < 0)
            throw SystemError("setsockopt(SO_NOSIGPIPE)");
#endif
        if (::setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, &on, sizeof(on)) < 0
            && errno != EOPNOTSUPP)
            throw SystemError("setsockopt(TCP_NODELAY)");

        IODeviceImpl::open(fd, true, false);

        std::memmove(&_peeraddr, _addrInfoPtr->ai_addr, _addrInfoPtr->ai_addrlen);

        log_debug("created socket " << _fd << " max: " << FD_SETSIZE);
        log_debug("connect to port " << _addrInfo.port());

        if (::connect(_fd, _addrInfoPtr->ai_addr, _addrInfoPtr->ai_addrlen) == 0)
        {
            _state = CONNECTED;
            log_debug("connected successfully to " << getPeerAddr());
            break;
        }

        if (errno == EINPROGRESS)
        {
            log_debug("connect in progress");
            break;
        }

        _connectFailedMessages.push_back(connectFailedMessage(_addrInfoPtr, errno));

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

    assert(_state == IDLE);

    _connectFailedMessages.clear();
    _addrInfo = addrInfo;
    _addrInfoPtr = _addrInfo.impl()->begin();
    _state = CONNECTING;
    _connectResult = tryConnect();
    return _state == CONNECTED || !_connectResult.empty();
}


void TcpSocketImpl::endConnect()
{
    log_trace("ending connect");

    if (_pfd && ! _socket.wbuf())
    {
        _pfd->events &= ~POLLOUT;
    }

    checkPendingError();

    if ( _state == CONNECTED )
        return;

    try
    {
        while (true)
        {
            pollfd pfd;
            pfd.fd = _fd;
            pfd.revents = 0;
            pfd.events = POLLOUT;

            log_debug("wait " << timeout());
            bool avail = wait(timeout(), pfd);

            if (avail)
            {
                // something has happened
                int sockerr = checkConnect();
                if (_state == CONNECTED)
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
            if (_state == CONNECTED)
            {
                _connectResult.clear();
                _connectFailedMessages.clear();
                return;
            }

            if (!_connectResult.empty())
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

    if (_fd < 0)
        throw SystemError("accept");

#ifdef HAVE_ACCEPT4
    // Pass inherit flag as "true" since this is the default.
    // Otherwise `open` would set it although we have already set it with accept4
    IODeviceImpl::open(_fd, false, true);
#else
    bool inherit = (flags & TcpSocket::INHERIT) != 0;
    IODeviceImpl::open(_fd, true, inherit);
#endif
    //TODO ECONNABORTED EINTR EPERM

    static const int on = 1;
    if (::setsockopt(_fd, IPPROTO_TCP, TCP_NODELAY, &on, sizeof(on)) < 0
        && errno != EOPNOTSUPP)
        throw SystemError("setsockopt(TCP_NODELAY)");

    _state = CONNECTED;
    log_debug( "accepted from " << getPeerAddr() << " fd=" << _fd);
}


void TcpSocketImpl::initWait(pollfd& pfd)
{
    IODeviceImpl::initWait(pfd);

    if (!isConnected())
    {
        log_debug("not connected, setting POLLOUT ");
        pfd.events = POLLOUT;
    }
}


bool TcpSocketImpl::checkPollEvent(pollfd& pfd)
{
    log_finer("checkPollEvent " << pfd.revents);

    DestructionSentry sentry(_sentry);

    if (isConnected())
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

        if (_state == SSLCONNECTING)
        {
            log_debug_to(ssl, "SSL_connect");
            int ret = SSL_connect(_ssl);
            if (ret == 1)
            {
                _state = SSLCONNECTED;
                _socket.sslConnected(_socket);
            }
            else
            {
                try
                {
                    checkSslOperation(ret, "SSL_connect", _pfd);
                }
                catch (const std::exception&)
                {
                    Resetter<State> resetter(_state, CONNECTED);
                    _state = THROWING;
                    _socket.sslConnected(_socket);
                }
            }

            return true;
        }
        else if (_state == SSLACCEPTING)
        {
            log_debug_to(ssl, "SSL_accept " << _fd);
            int ret = SSL_accept(_ssl);
            if (ret == 1)
            {
                _state = SSLCONNECTED;
                _socket.sslAccepted(_socket);
            }
            else
            {
                try
                {
                    checkSslOperation(ret, "SSL_accept", _pfd);
                }
                catch (const std::exception&)
                {
                    Resetter<State> resetter(_state, CONNECTED);
                    _state = THROWING;
                    _socket.sslAccepted(_socket);
                }
            }

            return true;
        }

        bool avail = IODeviceImpl::checkPollEvent(pfd);

        if (!sentry)
            return avail;

        if ( !_device.reading() && !_device.writing() && !_socket.enabled())
        {
            _socket.closed(_socket);
            return true;
        }

        return avail;
    }

    if (pfd.revents & POLLERR)
    {
        int sockerr;
        socklen_t optlen = sizeof(sockerr);

        // check for socket error
        if (::getsockopt(_fd, SOL_SOCKET, SO_ERROR, &sockerr, &optlen) != 0)
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

            if (_state == CONNECTED || !_connectResult.empty())
            {
                // immediate success or error
                log_debug_if(_state == CONNECTED, "connected successfully");
                log_debug_if(!_connectResult.empty(), "connection failed: " << _connectResult);
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
    else if (pfd.revents & POLLOUT)
    {
        int sockerr = checkConnect();
        if (_state == CONNECTED)
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
        if (_state == CONNECTED)
        {
            _socket.connected(_socket);
            return true;
        }
    }

    return false;
}

size_t TcpSocketImpl::callSend(const char* buffer, size_t n)
{
    log_debug("::send(" << _fd << ", buffer, " << n << ')');
    log_finer(hexDump(buffer, n));

#if defined(HAVE_MSG_NOSIGNAL)

    ssize_t ret;
    do {
        ret = ::send(_fd, (const void*)buffer, n, MSG_NOSIGNAL);
    } while (ret == -1 && errno == EINTR);

#elif defined(HAVE_SO_NOSIGPIPE)

    ssize_t ret;
    do {
        ret = ::send(_fd, (const void*)buffer, n, 0);
    } while (ret == -1 && errno == EINTR);

#else

    // block SIGPIPE
    sigset_t sigpipeMask, oldSigmask;
    sigemptyset(&sigpipeMask);
    sigaddset(&sigpipeMask, SIGPIPE);
    pthread_sigmask(SIG_BLOCK, &sigpipeMask, &oldSigmask);

    // execute send
    ssize_t ret;
    do {
        ret = ::send(_fd, (const void*)buffer, n, 0);
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

    int e = errno;

    log_debug("send returned " << ret);
    if (ret > 0)
        return static_cast<size_t>(ret);

    errno = e;

    if (e == ECONNRESET || e == EPIPE)
        throw IOError("lost connection to peer");

    return 0;
}


size_t TcpSocketImpl::beginWrite(const char* buffer, size_t n)
{
    if (_state == CONNECTED)
    {
        try
        {
            size_t ret = callSend(buffer, n);

            if (ret > 0)
                return ret;

            if (_pfd)
                _pfd->events |= POLLOUT;
        }
        catch (const std::exception&)
        {
            _exception = std::current_exception();
        }
    }
    else if (_state == SSLCONNECTED)
    {
        log_debug("SSL_write(" << _fd << ", buffer, " << n << ')');
        log_finer(hexDump(buffer, n));

        int ret = SSL_write(_ssl, buffer, n);
        log_debug("SSL_write returned " << ret);
        if (ret > 0)
            return ret;

        try
        {
            checkSslOperation(ret, "SSL_write", _pfd);
        }
        catch (const std::exception&)
        {
            _exception = std::current_exception();
        }
    }
    else
    {
        log_error("Device not connected when trying to write; state=" << _state);
        throw std::logic_error("Device not connected when trying to write");
    }

    return 0;
}


size_t TcpSocketImpl::write(const char* buffer, size_t n)
{
    ssize_t ret = 0;

    while (true)
    {
        if (_state == CONNECTED)
        {
            ret = callSend(buffer, n);
            if (ret > 0)
                break;

            if (errno != EAGAIN)
                throw IOError(getErrnoString("send"));

            pollfd pfd;
            pfd.fd = _fd;
            pfd.revents = 0;
            pfd.events = POLLOUT;

            if (!wait(_timeout, pfd))
                throw IOTimeout();
        }
        else if (_state == SSLCONNECTED)
        {
            log_debug("SSL_write");
            ret = SSL_write(_ssl, buffer, n);
            if (ret > 0)
                break;
            waitSslOperation(ret, timeout());
        }
        else
        {
            log_error("Device not connected when trying to write; state=" << _state);
            throw std::logic_error("Device not connected when trying to write");
        }
    }

    return static_cast<size_t>(ret);
}

void TcpSocketImpl::inputReady()
{
    log_trace("inputReady; state=" << static_cast<int>(_state));

    switch (_state)
    {
        case IDLE:
        case CONNECTING:
            break;

        case CONNECTED:
        case SSLCONNECTED:
            IODeviceImpl::inputReady();
            break;

        case SSLACCEPTING:
            if (continueSslAccept())
                _socket.sslAccepted(_socket);
            break;

        case SSLCONNECTING:
            if (continueSslConnect())
                _socket.sslConnected(_socket);
            break;

        case SSLSHUTTINGDOWN:
            if (beginSslShutdown())
                _socket.sslClosed(_socket);
            break;

        case THROWING:
            // won't happen
            throw;
    }
}

void TcpSocketImpl::outputReady()
{
    switch (_state)
    {
        case IDLE:
        case CONNECTING:
            break;

        case CONNECTED:
        case SSLCONNECTED:
            IODeviceImpl::outputReady();
            break;

        case SSLACCEPTING:
            if (continueSslAccept())
                _socket.sslAccepted(_socket);
            break;

        case SSLCONNECTING:
            if (continueSslConnect())
                _socket.sslConnected(_socket);
            break;

        case SSLSHUTTINGDOWN:
            if (beginSslShutdown())
                _socket.sslClosed(_socket);
            break;

        case THROWING:
            // won't happen
            throw;
    }
}

size_t TcpSocketImpl::read(char* buffer, size_t count, bool& eof)
{
    if (_state == CONNECTED)
    {
        return IODeviceImpl::read(buffer, count, eof);
    }
    else if (_state == SSLCONNECTED)
    {
        while (true)
        {
            log_debug("SSL_read");
            int ret = SSL_read(_ssl, buffer, count);
            log_debug("SSL_read(" << _fd << ", " << count << ") returned " << ret);
            if (ret > 0)
            {
                log_finer(hexDump(buffer, ret));
                return ret;
            }

            auto sslError = ERR_peek_error();
            if (ERR_GET_LIB(sslError) == ERR_LIB_SSL
                    && ERR_GET_REASON(sslError) == SSL_R_UNEXPECTED_EOF_WHILE_READING)
            {
                log_debug("SSL_R_UNEXPECTED_EOF_WHILE_READING detected");
                ERR_get_error();    // remove error from list when processed
                eof = true;
                return 0;
            }

            pollfd pfd;
            pfd.fd = _fd;
            pfd.revents = 0;
            pfd.events = 0;

            checkSslOperation(ret, "SSL_read", &pfd);

            if (_state != SSLCONNECTED)
            {
                eof = true;
                return 0;
            }

            log_debug("wait " << timeout());
            bool avail = wait(timeout(), pfd);
            if (!avail)
                throw IOTimeout();
        }
    }
    else
    {
        log_warn("socket not connected when trying to read; state=" << _state);
        throw IOError("socket not connected when trying to read");
    }
}

const SslCertificate& TcpSocketImpl::getSslPeerCertificate() const
{
    if (_ssl && !_peerCertificateLoaded)
    {
        X509* cert = SSL_get_peer_certificate(_ssl);
        if (cert)
            _peerCertificate = SslCertificate(new SslCertificateImpl(cert));
        _peerCertificateLoaded = true;
    }
    return _peerCertificate;
}

bool TcpSocketImpl::beginSslConnect(const SslCtx& ctx)
{
    log_trace("beginSslConnect; state=" << static_cast<int>(_state));

    if (!(_state == CONNECTED || _state == SSLCONNECTING))
    {
        log_error("Device not connected when trying to enable ssl; state=" << _state);
        throw std::logic_error("Device not connected when trying to enable ssl");
    }

    _state = SSLCONNECTING;
    initSsl(ctx);

    return continueSslConnect();
}

bool TcpSocketImpl::continueSslConnect()
{
    log_debug("SSL_connect");
    int ret = SSL_connect(_ssl);
    if (ret == 1)
    {
        log_debug("SSL connection successful");
        _state = SSLCONNECTED;
        return true;
    }

    try
    {
        checkSslOperation(ret, "SSL_connect", _pfd);
    }
    catch (const std::exception&)
    {
        _exception = std::current_exception();
    }

    return false;
}

void TcpSocketImpl::endSslConnect()
{
    log_trace("ending ssl connect");

    if (_pfd && !_socket.wbuf())
        _pfd->events &= ~POLLOUT;

    if (_state == THROWING)
        throw;

    checkPendingException();

    if (_state == SSLCONNECTED)
    {
        verifySslCertificate();
        return;
    }

    if (_state == IDLE)
    {
        log_debug("no connection when ending connect; assume lost connection");
        throw IOError("lost connection to peer");
    }

    if (_state != SSLCONNECTING)
    {
        log_error_to(ssl, "Device not in connecting mode when trying to finish ssl connect; state=" << _state);
        throw std::logic_error("Device not in connecting mode when trying to finish ssl connect");
    }

    while (true)
    {
        log_debug("SSL_connect");
        int ret = SSL_connect(_ssl);
        if (ret == 1)
        {
            log_debug("SSL connection successful");
            log_debug_if(_socket.hasSslPeerCertificate(),
                    "peer subject: \"" << _socket.getSslPeerCertificate().getSubject() << "\" "
                    "validity: " << _socket.getSslPeerCertificate().getNotBefore().toString() <<
                    " - " << _socket.getSslPeerCertificate().getNotAfter().toString());
            verifySslCertificate();
            _state = SSLCONNECTED;
            return;
        }

        waitSslOperation(ret, timeout());
    }
}

bool TcpSocketImpl::beginSslAccept(const SslCtx& ctx)
{
    log_trace("begin ssl accept");

    if (!(_state == CONNECTED || _state == SSLACCEPTING))
    {
        log_error_to(ssl, "Device not connected when trying to enable ssl; state=" << _state);
        throw std::logic_error("Device not connected when trying to enable ssl");
    }

    _state = SSLACCEPTING;
    initSsl(ctx);

    return continueSslAccept();
}

bool TcpSocketImpl::continueSslAccept()
{
    log_debug_to(ssl, "SSL_accept " << _fd);
    int ret = SSL_accept(_ssl);
    if (ret == 1)
    {
        log_debug_to(ssl, "SSL accepted");
        log_debug_to_if(ssl, _socket.hasSslPeerCertificate(), "peer subject: \"" << _socket.getSslPeerCertificate().getSubject() << "\" validity: " << _socket.getSslPeerCertificate().getNotBefore().toString() << " - " << _socket.getSslPeerCertificate().getNotAfter().toString());
        _state = SSLCONNECTED;
        return true;
    }

    try
    {
        checkSslOperation(ret, "SSL_accept", _pfd);
    }
    catch (const std::exception&)
    {
        _exception = std::current_exception();
    }

    return false;
}

void TcpSocketImpl::endSslAccept()
{
    log_trace_to(ssl, "ending ssl accept");

    if (_pfd && !_socket.wbuf())
        _pfd->events &= ~POLLOUT;

    if (_state == THROWING)
        throw;

    checkPendingException();

    if (_state == SSLCONNECTED)
    {
        verifySslCertificate();
        return;
    }

    if (_state == IDLE)
    {
        log_debug("no connection when ending accept; assume lost connection");
        throw IOError("lost connection to peer");
    }

    if (_state != SSLACCEPTING)
    {
        log_error_to(ssl, "Device not in accepting mode when trying to finish ssl accept; state=" << _state);
        throw std::logic_error("Device not in accepting mode when trying to finish ssl accept");
    }

    while (true)
    {
        log_debug_to(ssl, "SSL_accept " << _fd);
        int ret = SSL_accept(_ssl);
        if (ret == 1)
        {
            log_debug_to(ssl, "SSL accepted");
            verifySslCertificate();
            _state = SSLCONNECTED;
            return;
        }

        waitSslOperation(ret, timeout());
    }
}

bool TcpSocketImpl::beginSslShutdown()
{
    if (!(_state == SSLCONNECTED || _state == SSLSHUTTINGDOWN))
    {
        log_error_to(ssl, "Device not connected when trying to shutdown ssl; state=" << _state);
        throw std::logic_error("Device not connected when trying to shutdown ssl");
    }

    _state = SSLSHUTTINGDOWN;

    log_debug_to(ssl, "SSL_shutdown");
    int ret = SSL_shutdown(_ssl);
    if (ret == 1)
    {
        _state = CONNECTED;
        return true;
    }

    try
    {
        checkSslOperation(ret, "SSL_shutdown", _pfd);
    }
    catch (const std::exception&)
    {
        _exception = std::current_exception();
    }

    return false;
}

void TcpSocketImpl::endSslShutdown()
{
    log_trace_to(ssl, "ending ssl shutdown");

    if (_pfd && !_socket.wbuf())
        _pfd->events &= ~POLLOUT;

    if (_state == CONNECTED)
        return;

    if (_state == IDLE)
    {
        log_debug("no connection when ending ssl shutdown; assume lost connection");
        throw IOError("lost connection to peer");
    }

    if (_state != SSLSHUTTINGDOWN)
    {
        log_error_to(ssl, "Device not in ssl shutdown mode when trying to finish ssl shutdown; state=" << _state);
        throw std::logic_error("Device not in ssl shutdown mode when trying to finish ssl shutdown");
    }

    checkPendingException();

    while (true)
    {
        log_debug_to(ssl, "SSL_shutdown");
        int ret = SSL_shutdown(_ssl);
        if (ret == 1)
        {
            _state = CONNECTED;
            SSL* ssl = _ssl;
            _ssl = 0;
            SSL_clear(ssl);
            SslError::checkSslError();
            return;
        }

        waitSslOperation(ret, timeout());
    }
}

void TcpSocketImpl::verifySslCertificate()
{
    if (_socket.acceptSslCertificate.isConnected()
        && !_socket.acceptSslCertificate(_socket.getSslPeerCertificate()))
    {
        _state = CONNECTED;
        throw SslCertificateNotAccepted();
    }
}

} // namespace net

} // namespace cxxtools
