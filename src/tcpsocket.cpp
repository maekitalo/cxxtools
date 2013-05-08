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
#include <stdexcept>
#include <memory>
#include "cxxtools/log.h"
#include <errno.h>
#include <cxxtools/systemerror.h>
#include <cxxtools/ioerror.h>

log_define("cxxtools.net.tcpsocket")

namespace cxxtools {

namespace net {

TcpSocket::TcpSocket()
: _impl(0)
{
    _impl = new TcpSocketImpl(*this);
}


TcpSocket::TcpSocket(const TcpServer& server, unsigned flags)
: _impl(0)
{
    _impl = new TcpSocketImpl(*this);
    std::auto_ptr<TcpSocketImpl> impl(_impl);

    this->accept(server, flags);

    impl.release();
}


TcpSocket::TcpSocket(const std::string& ipaddr, unsigned short int port)
: _impl(0)
{
    _impl = new TcpSocketImpl(*this);
    std::auto_ptr<TcpSocketImpl> impl(_impl);

    this->connect(ipaddr, port);

    impl.release();
}


TcpSocket::TcpSocket(const AddrInfo& addrinfo)
: _impl(0)
{
    _impl = new TcpSocketImpl(*this);
    std::auto_ptr<TcpSocketImpl> impl(_impl);

    this->connect(addrinfo);

    impl.release();
}


TcpSocket::~TcpSocket()
{
    try
    {
        this->close();
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


void TcpSocket::setTimeout(std::size_t msecs)
{
    _impl->setTimeout(msecs);
}


std::size_t TcpSocket::timeout() const
{
    return _impl->timeout();
}


void TcpSocket::connect(const AddrInfo& addrinfo)
{
    this->close();
    _impl->connect(addrinfo);
    this->setEnabled(true);
    this->setAsync(true);
    this->setEof(false);
}


bool TcpSocket::beginConnect(const AddrInfo& addrinfo)
{
    this->close();
    bool ret = _impl->beginConnect(addrinfo);
    this->setEnabled(true);
    this->setAsync(true);
    this->setEof(false);

    if(ret)
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


int TcpSocket::getFd() const
{
    return _impl->fd();
}


void TcpSocket::accept(const TcpServer& server, unsigned flags)
{
    this->close();
    _impl->accept(server, flags);
    this->setEnabled(true);
    this->setAsync(true);
    this->setEof(false);
}


SelectableImpl& TcpSocket::simpl()
{
    return *_impl;
}


void TcpSocket::onClose()
{
    cancel();
    _impl->close();
}


bool TcpSocket::onWait(std::size_t msecs)
{
    return _impl->wait(msecs);
}


void TcpSocket::onAttach(SelectorBase& sb)
{
    _impl->attach(sb);
}


void TcpSocket::onDetach(SelectorBase& sb)
{
    _impl->detach(sb);
}


size_t TcpSocket::onBeginRead(char* buffer, size_t n, bool& eof)
{
    if (!_impl->isConnected())
        throw IOPending("connect operation pending");

    return _impl->beginRead(buffer, n, eof);
}


size_t TcpSocket::onEndRead(bool& eof)
{
    return _impl->endRead(eof);
}


size_t TcpSocket::onRead(char* buffer, size_t count, bool& eof)
{
    return _impl->read(buffer, count, eof);
}


size_t TcpSocket::onBeginWrite(const char* buffer, size_t n)
{
    if (!_impl->isConnected())
        throw IOPending("connect operation pending");

    return _impl->beginWrite(buffer, n);
}


size_t TcpSocket::onEndWrite()
{
    return _impl->endWrite();
}


size_t TcpSocket::onWrite(const char* buffer, size_t count)
{
    return _impl->write(buffer, count);
}


void TcpSocket::onCancel()
{
    if (_impl->isConnected())
    {
        _impl->cancel();
    }
    else if (enabled())
    {
        // we are in connecting state
        _impl->close();
        setEnabled(false);
    }
}


IODeviceImpl& TcpSocket::ioimpl()
{
    throw std::runtime_error("TcpSocket::ioimpl() not implemented");
    IODeviceImpl* impl = 0;
    return *impl;
}

short TcpSocket::poll(short events) const
{
    struct pollfd fds;
    fds.fd = _impl->fd();
    fds.events = events;

    log_debug("poll timeout " << getTimeout());

    int p = ::poll(&fds, 1, getTimeout());

    log_debug("poll returns " << p << " revents " << fds.revents);

    if (p < 0)
    {
      log_error("error in poll; errno=" << errno);
      throw SystemError("poll");
    }
    else if (p == 0)
    {
      log_debug("poll timeout (" << getTimeout() << ')');
      throw IOTimeout();
    }

    return fds.revents;
}


} // namespace net

} // namespace cxxtools
