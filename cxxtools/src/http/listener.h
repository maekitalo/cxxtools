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

#ifndef cxxtools_http_Listener_H
#define cxxtools_http_Listener_H

#include <cxxtools/connectable.h>
#include <cxxtools/net/tcpserver.h>

namespace cxxtools
{

class SelectorBase;

namespace http
{

class ServerImpl;

class Listener : public Connectable
{
        std::string _ip;
        unsigned short int _port;

        net::TcpServer _tcpServer;
        SelectorBase& _selector;
        ServerImpl& _httpServer;

        void onConnect(net::TcpServer& tcpServer);

    public:
        Listener(const std::string& ip, unsigned short int port,
            SelectorBase& selector, ServerImpl& httpServer);

        void listen()
        { _tcpServer.listen(_ip, _port); }

        void close()
        { _tcpServer.close(); }
};

}
}

#endif // cxxtools_http_Listener_H

