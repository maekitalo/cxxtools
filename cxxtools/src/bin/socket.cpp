/*
 * Copyright (C) 2011 Tommi Maekitalo
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * is provided AS IS, WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, and
 * NON-INFRINGEMENT.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 *
 */

#include "socket.h"
#include "rpcserverimpl.h"
#include <cxxtools/log.h>

log_define("cxxtools.bin.socket")

namespace cxxtools
{
namespace bin
{

Socket::Socket(RpcServerImpl& server, net::TcpServer& tcpServer)
    : inputSlot(slot(*this, &Socket::onInput)),
      _tcpServer(tcpServer),
      _server(server),
      _responder(server),
      _accepted(false)
{
    _stream.attachDevice(*this);
    cxxtools::connect(IODevice::inputReady, *this, &Socket::onIODeviceInput);
    cxxtools::connect(_stream.buffer().outputReady, *this, &Socket::onOutput);
}

Socket::Socket(Socket& socket)
    : inputSlot(slot(*this, &Socket::onInput)),
      _tcpServer(socket._tcpServer),
      _server(socket._server),
      _responder(socket._server),
      _accepted(false)
{
    _stream.attachDevice(*this);
    cxxtools::connect(IODevice::inputReady, *this, &Socket::onIODeviceInput);
    cxxtools::connect(_stream.buffer().outputReady, *this, &Socket::onOutput);
}

void Socket::accept()
{
    net::TcpSocket::accept(_tcpServer, net::TcpSocket::DEFER_ACCEPT);

    _accepted = true;

    _stream.buffer().beginRead();
}

void Socket::setSelector(SelectorBase* s)
{
    s->add(*this);
}

void Socket::removeSelector()
{
    TcpSocket::setSelector(0);
}

void Socket::onIODeviceInput(IODevice& iodevice)
{
    log_debug("onIODeviceInput");
    inputReady(*this);
}

void Socket::onInput(StreamBuffer& sb)
{
    log_debug("onInput");

    sb.endRead();

    if (sb.in_avail() == 0 || sb.device()->eof())
    {
        close();
        return;
    }

    _responder.onInput(_stream);
}

bool Socket::onOutput(StreamBuffer& sb)
{
    log_trace("onOutput");

    log_debug("send data to " << getPeerAddr());

    try
    {
        sb.endWrite();

        if ( sb.out_avail() )
        {
            sb.beginWrite();
        }
        else
        {
            if (sb.in_avail())
                onInput(sb);
            else
                _stream.buffer().beginRead();
        }
    }
    catch (const std::exception& e)
    {
        log_warn("exception occured when processing request: " << e.what());
        close();
        return false;
    }

    return true;
}

}
}
