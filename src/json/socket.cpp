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

#include "socket.h"
#include "rpcserverimpl.h"
#include <cxxtools/log.h>

log_define("cxxtools.json.socket")

namespace cxxtools
{
namespace json
{

Socket::Socket(RpcServerImpl& server, ServiceRegistry& serviceRegistry, net::TcpServer& tcpServer)
    : inputSlot(slot(*this, &Socket::onInput)),
      _tcpServer(tcpServer),
      _server(server),
      _responder(serviceRegistry),
      _accepted(false)
{
    _stream.attachDevice(*this);
    cxxtools::connect(IODevice::inputReady, *this, &Socket::onIODeviceInput);
    cxxtools::connect(_stream.buffer().outputReady, *this, &Socket::onOutput);
    _responder.begin();
}

Socket::Socket(Socket& socket)
    : net::TcpSocket(),
      Connectable(*this),
      inputSlot(slot(*this, &Socket::onInput)),
      _tcpServer(socket._tcpServer),
      _server(socket._server),
      _responder(socket._responder._serviceRegistry),
      _accepted(false)
{
    _stream.attachDevice(*this);
    cxxtools::connect(IODevice::inputReady, *this, &Socket::onIODeviceInput);
    cxxtools::connect(_stream.buffer().outputReady, *this, &Socket::onOutput);
    _responder.begin();
}

void Socket::accept()
{
    net::TcpSocket::accept(_tcpServer);

    if (!_server.certificateFile().empty())
    {
        loadSslCertificateFile(_server.certificateFile(), _server.privateKeyFile());
        sslAccept();
    }

    _accepted = true;

    buffer().beginRead();
}

void Socket::setSelector(SelectorBase* s)
{
    s->add(*this);
}

void Socket::removeSelector()
{
    TcpSocket::setSelector(0);
}

void Socket::onIODeviceInput(IODevice& /*iodevice*/)
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

    while (sb.in_avail() > 0)
    {
        if (_responder.advance(sb.sbumpc()))
        {
            _responder.finalize(_stream);
            buffer().beginWrite();
            onOutput(sb);
            return;
        }
    }

    sb.beginRead();

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
        else if (_responder.failed())
        {
            close();
            return false;
        }
        else
        {
            _responder.begin();
            if (sb.in_avail())
                onInput(sb);
            else
                buffer().beginRead();
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
