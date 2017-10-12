/*
 * Copyright (C) 2011 Tommi Maekitalo
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

#include <cxxtools/net/tcpstream.h>

namespace cxxtools
{
namespace net
{
void TcpStream::init(cxxtools::Timespan timeout)
{
    _socket.setTimeout(timeout);
    attachDevice(_socket);
    cxxtools::connect(_socket.inputReady, *this, &TcpStream::onInput);
    cxxtools::connect(_socket.outputReady, *this, &TcpStream::onOutput);
    cxxtools::connect(_socket.connected, *this, &TcpStream::onConnected);
    cxxtools::connect(_socket.closed, *this, &TcpStream::onClosed);
    cxxtools::connect(_socket.sslAccepted, *this, &TcpStream::onSslAccepted);
    cxxtools::connect(_socket.sslConnected, *this, &TcpStream::onSslConnected);
    cxxtools::connect(_socket.sslClosed, *this, &TcpStream::onSslClosed);
}

void TcpStream::onInput(IODevice&)
{
    inputReady(*this);
}

void TcpStream::onOutput(IODevice&)
{
    outputReady(*this);
}

void TcpStream::onConnected(TcpSocket&)
{
    connected(*this);
}

void TcpStream::onClosed(TcpSocket&)
{
    closed(*this);
}

void TcpStream::onSslAccepted(TcpSocket&)
{
    sslAccepted(*this);
}

void TcpStream::onSslConnected(TcpSocket&)
{
    sslConnected(*this);
}

void TcpStream::onSslClosed(TcpSocket&)
{
    sslClosed(*this);
}


}
}
