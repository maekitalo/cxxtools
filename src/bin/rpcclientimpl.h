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

#ifndef CXXTOOLS_BIN_RPCCLIENTIMPL_H
#define CXXTOOLS_BIN_RPCCLIENTIMPL_H

#include <cxxtools/remoteclient.h>
#include <cxxtools/bin/formatter.h>
#include <cxxtools/net/tcpsocket.h>
#include <cxxtools/iostream.h>
#include <cxxtools/string.h>
#include <cxxtools/connectable.h>
#include <cxxtools/bin/deserializer.h>
#include <cxxtools/refcounted.h>
#include <cxxtools/timespan.h>
#include <cxxtools/sslctx.h>
#include <string>
#include "scanner.h"

namespace cxxtools
{

class SelectorBase;

namespace bin
{

class RpcClientImpl : public RefCounted, public Connectable
{
        RpcClientImpl(RpcClientImpl&);
        void operator= (const RpcClientImpl&);

    public:
        RpcClientImpl();

        net::TcpSocket& socket()    { return _socket; }

        void setSelector(SelectorBase* selector)
        {
            _socket.setSelector(selector);
        }

        void prepareConnect(const net::AddrInfo& addrinfo, const SslCtx& sslCtx)
        {
            _addrInfo = addrinfo;
            _socket.close();
            _sslCtx = sslCtx;
        }

        void connect();

        void close();

        void beginCall(IComposer& r, IRemoteProcedure& method, IDecomposer** argv, unsigned argc);

        void endCall();

        void call(IComposer& r, IRemoteProcedure& method, IDecomposer** argv, unsigned argc);

        Timespan timeout() const  { return _timeout; }
        void timeout(Timespan t)  { _timeout = t; if (!_connectTimeoutSet) _connectTimeout = t; }

        Timespan connectTimeout() const  { return _connectTimeout; }
        void connectTimeout(Timespan t)  { _connectTimeout = t; _connectTimeoutSet = true; }

        const IRemoteProcedure* activeProcedure() const
        { return _proc; }

        void cancel();

        void wait(Timespan msecs);

        const std::string& domain() const
        { return _domain; }

        void domain(const std::string& p)
        { _domain = p; }

    private:
        void prepareRequest(const String& name, IDecomposer** argv, unsigned argc);
        void onConnect(net::TcpSocket& socket);
        void onSslConnect(net::TcpSocket& socket);
        void onOutput(StreamBuffer& sb);
        void onInput(StreamBuffer& sb);

        // connection state
        net::TcpSocket _socket;
        IOStream _stream;

        net::AddrInfo _addrInfo;
        SslCtx _sslCtx;
        std::string _domain;

        // serialization
        Scanner _scanner;
        Deserializer _deserializer;
        Formatter _formatter;

        bool _exceptionPending;
        IRemoteProcedure* _proc;

        Timespan _timeout;
        bool _connectTimeoutSet;  // indicates if connectTimeout is explicitely set
                                  // when not, it follows the setting of _timeout
        Timespan _connectTimeout;
};

}

}

#endif // CXXTOOLS_BIN_RPCCLIENTIMPL_H
