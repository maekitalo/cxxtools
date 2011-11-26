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

#ifndef CXXTOOLS_BIN_CLIENTIMPL_H
#define CXXTOOLS_BIN_CLIENTIMPL_H

#include <cxxtools/remoteclient.h>
#include <cxxtools/bin/formatter.h>
#include <cxxtools/net/tcpsocket.h>
#include <cxxtools/iostream.h>
#include <cxxtools/string.h>
#include <string>
#include "scanner.h"

namespace cxxtools
{

class SelectorBase;

namespace bin
{

class RpcClient;

class RpcClientImpl
{
        RpcClientImpl(RpcClientImpl&) { }
        void operator= (const RpcClientImpl&) { }

    public:
        RpcClientImpl(RpcClient* client, SelectorBase* selector, const std::string& addr, unsigned short port);

        ~RpcClientImpl();

        void setSelector(SelectorBase* selector)
        { _socket.setSelector(selector); }

        void connect(const std::string& addr, unsigned short port);

        void close();

        void beginCall(IDeserializer& r, IRemoteProcedure& method, ISerializer** argv, unsigned argc);

        void endCall();

        void call(IDeserializer& r, IRemoteProcedure& method, ISerializer** argv, unsigned argc);

        const IRemoteProcedure* activeProcedure() const
        { return _proc; }

        void cancel();

    private:
        void prepareRequest(const String& name, ISerializer** argv, unsigned argc);
        void onConnect(net::TcpSocket& socket);
        void onOutput(StreamBuffer& sb);
        void onInput(StreamBuffer& sb);

        RpcClient* _client;
        IRemoteProcedure* _proc;
        net::TcpSocket _socket;
        IOStream _stream;
        Scanner _scanner;
        Formatter _formatter;

        bool _exceptionPending;
        bool _reconnectOnError;

        std::string _addr;
        unsigned short _port;
};

}

}

#endif // CXXTOOLS_BIN_CLIENTIMPL_H
