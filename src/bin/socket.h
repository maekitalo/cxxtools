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

#ifndef CXXTOOLS_BIN_SOCKET_H
#define CXXTOOLS_BIN_SOCKET_H

#include <cxxtools/net/tcpsocket.h>
#include <cxxtools/iostream.h>
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
        Socket(RpcServerImpl& rpcServerImpl, net::TcpServer& tcpServer, const std::string& certificateFile, const std::string& privateKeyFile, int sslVerifyLevel, const std::string& sslCa);
        explicit Socket(Socket& socket);

        void accept();
        void postAccept();
        bool hasAccepted() const  { return _accepted; }

        void setSelector(SelectorBase* s);
        void removeSelector();

        void onIODeviceInput(IODevice& iodevice);
        void onInput(StreamBuffer& sb);
        bool onOutput(StreamBuffer& sb);
        bool onAcceptSslCertificate(const SslCertificate& cert);

        Signal<Socket&> inputReady;

        StreamBuffer& buffer()         { return _stream.buffer(); }

        MethodSlot<void, Socket, StreamBuffer&> inputSlot;

        Connection inputConnection;
        Connection timeoutConnection;

    private:
        RpcServerImpl& _rpcServerImpl;
        net::TcpServer& _tcpServer;
        std::string _certificateFile;
        std::string _privateKeyFile;

        Responder _responder;
        IOStream _stream;

        int _sslVerifyLevel;
        std::string _sslCa;
        bool _accepted;
};

}
}

#endif // CXXTOOLS_BIN_SOCKET_H
