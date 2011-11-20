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

#ifndef CXXTOOLS_BIN_SOCKET_H
#define CXXTOOLS_BIN_SOCKET_H

#include <cxxtools/net/tcpsocket.h>
#include <cxxtools/iostream.h>
#include <cxxtools/timer.h>
#include <cxxtools/connectable.h>
#include <cxxtools/signal.h>
#include <cxxtools/method.h>
#include "responder.h"

namespace cxxtools
{
namespace bin
{

class RpcServerImpl;

class Socket : public net::TcpSocket, public Connectable
{
    public:
        Socket(RpcServerImpl& server, net::TcpServer& tcpServer);
        explicit Socket(Socket& socket);

        void accept();
        bool hasAccepted() const  { return _accepted; }

        void setSelector(SelectorBase* s);
        void removeSelector();

        void onIODeviceInput(IODevice& iodevice);
        void onInput(StreamBuffer& sb);
        bool onOutput(StreamBuffer& sb);

        Signal<Socket&> inputReady;

        StreamBuffer& buffer()         { return _stream.buffer(); }

        MethodSlot<void, Socket, StreamBuffer&> inputSlot;

        Connection inputConnection;
        Connection timeoutConnection;

    private:
        net::TcpServer& _tcpServer;
        RpcServerImpl& _server;

        Responder _responder;
        IOStream _stream;

        bool _accepted;
};

}
}

#endif // CXXTOOLS_BIN_SOCKET_H
