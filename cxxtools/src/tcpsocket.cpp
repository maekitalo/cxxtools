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
#include "cxxtools/tcpsocket.h"
#include <stdexcept>
#include <memory>

namespace cxxtools {

namespace net {

TcpSocket::TcpSocket()
: _impl(0)
{
    _impl = new TcpSocketImpl(*this);
}


TcpSocket::TcpSocket(TcpServer& server)
: _impl(0)
{
    _impl = new TcpSocketImpl(*this);
    std::auto_ptr<TcpSocketImpl> impl(_impl);

    this->accept(server);

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


TcpSocket::~TcpSocket()
{
    try
    {
        this->close();
    }
    catch(...)
    {}

    delete _impl;
}


std::string TcpSocket::getSockAddr() const
{
    return _impl->getSockAddr();
}


void TcpSocket::setTimeout(std::size_t msecs)
{
    _impl->setTimeout(msecs);
}


std::size_t TcpSocket::timeout() const
{
    return _impl->timeout();
}


void TcpSocket::connect(const std::string& ipaddr, unsigned short int port)
{
    this->close();
    _impl->connect(ipaddr, port);
    this->setEnabled(true);
}


bool TcpSocket::beginConnect(const std::string& ipaddr, unsigned short int port)
{
    this->close();
    bool ret = _impl->beginConnect(ipaddr, port);
    this->setEnabled(true);
    return ret;
}


void TcpSocket::endConnect()
{
    _impl->endConnect();
}


void TcpSocket::accept(TcpServer& server)
{
    this->close();
    _impl->accept(server);
    this->setEnabled(true);
}


SelectableImpl& TcpSocket::simpl()
{
    return *_impl;
}


void TcpSocket::onClose()
{
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

} // namespace net

} // namespace cxxtools
