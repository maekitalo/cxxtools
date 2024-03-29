/*
 * Copyright (C) 2006-2009 by Marc Boris Duerner, Tommi Maekitalo
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

#include "tcpsocketimpl.h"
#include "cxxtools/net/tcpsocket.h"
#include "cxxtools/log.h"
#include "cxxtools/systemerror.h"
#include "cxxtools/ioerror.h"
#include "cxxtools/sslcertificate.h"

#include <stdexcept>
#include <errno.h>

log_define("cxxtools.net.tcpsocket")

namespace cxxtools {

namespace net {

TcpSocket::TcpSocket()
: _impl(new TcpSocketImpl(*this))
{
}


TcpSocket::TcpSocket(const TcpServer& server, unsigned flags)
: _impl(new TcpSocketImpl(*this))
{
    try
    {
        accept(server, flags);
    }
    catch (...)
    {
        delete _impl;
        throw;
    }
}


TcpSocket::TcpSocket(const std::string& ipaddr, unsigned short int port)
: _impl(new TcpSocketImpl(*this))
{
    try
    {
        connect(ipaddr, port);
    }
    catch (...)
    {
        delete _impl;
        throw;
    }
}


TcpSocket::TcpSocket(const AddrInfo& addrinfo)
: _impl(new TcpSocketImpl(*this))
{
    try
    {
        connect(addrinfo);
    }
    catch (...)
    {
        delete _impl;
        throw;
    }
}


TcpSocket::~TcpSocket()
{
    try
    {
        close();
    }
    catch(const std::exception& e)
    {
        log_error("TcpSocket::close failed: " << e.what());
    }

    delete _impl;
}


std::string TcpSocket::getSockAddr() const
{
    return _impl->getSockAddr();
}


std::string TcpSocket::getPeerAddr() const
{
    return _impl->getPeerAddr();
}


void TcpSocket::setTimeout(Milliseconds timeout)
{
    _impl->setTimeout(timeout);
}


Milliseconds TcpSocket::timeout() const
{
    return _impl->timeout();
}


void TcpSocket::connect(const AddrInfo& addrinfo)
{
    close();
    _impl->beginConnect(addrinfo);
    _impl->endConnect();
    setEnabled(true);
    setAsync(true);
    setEof(false);
}


bool TcpSocket::beginConnect(const AddrInfo& addrinfo)
{
    close();
    bool ret = _impl->beginConnect(addrinfo);
    setEnabled(true);
    setAsync(true);
    setEof(false);

    if (ret)
        connected(*this);
    return ret;
}


void TcpSocket::endConnect()
{
    try
    {
        _impl->endConnect();
    }
    catch (...)
    {
        close();
        throw;
    }
}


bool TcpSocket::isConnected() const
{
    return _impl->isConnected();
}


bool TcpSocket::isSslConnected() const
{
    return _impl->isSslConnected();
}


int TcpSocket::getFd() const
{
    return _impl->fd();
}


bool TcpSocket::hasSslPeerCertificate() const
{
    return getSslPeerCertificate();
}

const SslCertificate& TcpSocket::getSslPeerCertificate() const
{
    return _impl->getSslPeerCertificate();
}

void TcpSocket::beginSslConnect(const SslCtx& ctx)
{
    if (_impl->beginSslConnect(ctx))
        sslConnected(*this);
}

void TcpSocket::endSslConnect()
{
    _impl->endSslConnect();
}

void TcpSocket::sslConnect(const SslCtx& ctx)
{
    _impl->beginSslConnect(ctx);
    _impl->endSslConnect();
}

void TcpSocket::beginSslAccept(const SslCtx& ctx)
{
    if (_impl->beginSslAccept(ctx))
        sslAccepted(*this);
}

void TcpSocket::endSslAccept()
{
    _impl->endSslAccept();
}

void TcpSocket::sslAccept(const SslCtx& ctx)
{
    _impl->beginSslAccept(ctx);
    _impl->endSslAccept();
}

void TcpSocket::beginSslShutdown()
{
    if (_impl->beginSslShutdown())
        sslClosed(*this);
}

void TcpSocket::endSslShutdown()
{
    _impl->endSslShutdown();
}

void TcpSocket::sslShutdown()
{
    _impl->beginSslShutdown();
    _impl->endSslShutdown();
}

void TcpSocket::accept(const TcpServer& server, unsigned flags)
{
    close();
    _impl->accept(server, flags);
    setEnabled(true);
    setAsync(true);
    setEof(false);
}


SelectableImpl& TcpSocket::simpl()
{
    return *_impl;
}


size_t TcpSocket::onBeginRead(char* buffer, size_t n, bool& eof)
{
    if (!_impl->isConnected())
        throw IOError("socket not connected when trying to read");

    return _impl->beginRead(buffer, n, eof);
}

size_t TcpSocket::onBeginWrite(const char* buffer, size_t n)
{
    if (!_impl->isConnected())
        throw IOError("socket not connected when trying to write");

    return _impl->beginWrite(buffer, n);
}

IODeviceImpl& TcpSocket::ioimpl()
{
    return *_impl;
}

short TcpSocket::poll(short events) const
{
    struct pollfd fds;
    fds.fd = _impl->fd();
    fds.events = events;

    int timeout = getTimeout().ceil();

    log_debug("poll timeout " << timeout);

    int p = ::poll(&fds, 1, timeout);

    log_debug("poll returns " << p << " revents " << fds.revents);

    if (p < 0)
    {
      log_error("error in poll; errno=" << errno);
      throw SystemError("poll");
    }
    else if (p == 0)
    {
      log_debug("poll timeout (" << timeout << ')');
      throw IOTimeout();
    }

    return fds.revents;
}


} // namespace net

} // namespace cxxtools
