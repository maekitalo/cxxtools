/*
 * Copyright (C) 2009 Tommi Maekitalo
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

#include "listener.h"
#include "serverimpl.h"
#include <cxxtools/selector.h>
#include "socket.h"

namespace cxxtools
{

namespace http
{

Listener::Listener(const std::string& ip, unsigned short int port,
    SelectorBase& selector, ServerImpl& httpServer)
    : _ip(ip),
      _port(port),
      _tcpServer(),
      _selector(selector),
      _httpServer(httpServer)
{
    selector.add(_tcpServer);
    connect(_tcpServer.connectionPending, *this, &Listener::onConnect);
}

void Listener::onConnect(net::TcpServer& tcpServer)
{
    new Socket(_selector, _httpServer, tcpServer);
}

}
}
