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

#ifndef CXXTOOLS_BIN_CLIENTIMPL_H
#define CXXTOOLS_BIN_CLIENTIMPL_H

#include <cxxtools/remoteclient.h>
#include <cxxtools/bin/formatter.h>
#include <cxxtools/net/tcpsocket.h>
#include <cxxtools/iostream.h>
#include <cxxtools/string.h>
#include <cxxtools/connectable.h>
#include <string>
#include "scanner.h"

namespace cxxtools
{

class SelectorBase;

namespace bin
{

class RpcClient;

class RpcClientImpl : public Connectable
{
        RpcClientImpl(RpcClientImpl&) { }
        void operator= (const RpcClientImpl&) { }

    public:
        RpcClientImpl(RpcClient* client, const std::string& addr, unsigned short port);

        RpcClientImpl(RpcClient* client, SelectorBase& selector, const std::string& addr, unsigned short port);

        ~RpcClientImpl();

        void setSelector(SelectorBase& selector)
        { selector.add(_socket); }

        void connect(const std::string& addr, unsigned short port);

        void close();

        void beginCall(IComposer& r, IRemoteProcedure& method, IDecomposer** argv, unsigned argc);

        void endCall();

        void call(IComposer& r, IRemoteProcedure& method, IDecomposer** argv, unsigned argc);

        const IRemoteProcedure* activeProcedure() const
        { return _proc; }

        void cancel();

        const std::string& praefix() const
        { return _praefix; }

        void praefix(const std::string& p)
        { _praefix = p; }

    private:
        void prepareRequest(const String& name, IDecomposer** argv, unsigned argc);
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

        std::string _addr;
        unsigned short _port;
        std::string _praefix;
};

}

}

#endif // CXXTOOLS_BIN_CLIENTIMPL_H
