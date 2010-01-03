/*
 * Copyright (C) 2009 Tommi Maekitalo
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

