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

#ifndef CXXTOOLS_BIN_CLIENT_H
#define CXXTOOLS_BIN_CLIENT_H

#include <cxxtools/remoteclient.h>
#include <cxxtools/delegate.h>
#include <string>

namespace cxxtools
{

class SslCertificate;
class SelectorBase;

namespace net
{

class AddrInfo;
class Uri;

}

namespace bin
{

class RpcClientImpl;

class RpcClient : public RemoteClient
{
        RpcClientImpl* _impl;
        RpcClientImpl* getImpl();
        const RpcClientImpl* getImpl() const
        { return const_cast<RpcClient*>(this)->getImpl(); }

    public:
        RpcClient()
            : _impl(0)
            { }

        explicit RpcClient(SelectorBase& selector)
            : _impl(0)
            { setSelector(selector); }

        explicit RpcClient(const net::AddrInfo& addr, bool ssl = false)
            : _impl(0)
            { prepareConnect(addr, ssl); }
        RpcClient(const net::AddrInfo& addr, const std::string& sslCertificate)
            : _impl(0)
            { prepareConnect(addr, sslCertificate); }
        RpcClient(const std::string& addr, unsigned short port, bool ssl = false)
            : _impl(0)
            { prepareConnect(addr, port, ssl); }
        RpcClient(const std::string& addr, unsigned short port, const std::string& sslCertificate)
            : _impl(0)
            { prepareConnect(addr, port, sslCertificate); }
        explicit RpcClient(const net::Uri& uri)
            : _impl(0)
            { prepareConnect(uri); }
        RpcClient(const net::Uri& uri, const std::string& sslCertificate)
            : _impl(0)
            { prepareConnect(uri, sslCertificate); }

        RpcClient(SelectorBase& selector, const net::AddrInfo& addr, bool ssl = false)
            : _impl(0)
            { setSelector(selector); prepareConnect(addr, ssl); }
        RpcClient(SelectorBase& selector, const net::AddrInfo& addr, const std::string& sslCertificate)
            : _impl(0)
            { setSelector(selector); prepareConnect(addr, sslCertificate); }
        RpcClient(SelectorBase& selector, const std::string& addr, unsigned short port, bool ssl = false)
            : _impl(0)
            { setSelector(selector); prepareConnect(addr, port, ssl); }
        RpcClient(SelectorBase& selector, const std::string& addr, unsigned short port, const std::string& sslCertificate)
            : _impl(0)
            { setSelector(selector); prepareConnect(addr, port, sslCertificate); }
        RpcClient(SelectorBase& selector, const net::Uri& uri)
            : _impl(0)
            { setSelector(selector); prepareConnect(uri); }
        RpcClient(SelectorBase& selector, const net::Uri& uri, const std::string& sslCertificate)
            : _impl(0)
            { setSelector(selector); prepareConnect(uri, sslCertificate); }

        RpcClient(const RpcClient&);
        RpcClient& operator= (const RpcClient&);

        virtual ~RpcClient();

        void prepareConnect(const net::AddrInfo& addr, bool ssl = false);
        void prepareConnect(const net::AddrInfo& addr, const std::string& sslCertificate);
        void prepareConnect(const std::string& host, unsigned short port, bool ssl = false);
        void prepareConnect(const std::string& host, unsigned short port, const std::string& sslCertificate);
        void prepareConnect(const net::Uri& uri);
        void prepareConnect(const net::Uri& uri, const std::string& sslCertificate);

        void connect(const net::AddrInfo& addrinfo, bool ssl_ = false)
            { prepareConnect(addrinfo, ssl_); connect(); }

        void connect(const net::AddrInfo& addrinfo, const std::string& sslCertificate)
            { prepareConnect(addrinfo, sslCertificate); connect(); }

        void connect(const std::string& host, unsigned short int port, bool ssl_ = false)
            { prepareConnect(host, port, ssl_); connect(); }

        void connect(const std::string& host, unsigned short int port, const std::string& sslCertificate)
            { prepareConnect(host, port, sslCertificate); connect(); }

        void connect(const net::Uri& uri)
            { prepareConnect(uri); connect(); }

        void connect(const net::Uri& uri, const std::string& sslCertificate)
            { prepareConnect(uri, sslCertificate); connect(); }

        void connect();

        void close();

        void setSelector(SelectorBase* selector);

        void setSelector(SelectorBase& selector);

        void beginCall(IComposer& r, IRemoteProcedure& method, IDecomposer** argv, unsigned argc);

        void endCall();

        void call(IComposer& r, IRemoteProcedure& method, IDecomposer** argv, unsigned argc);

        Milliseconds timeout() const;
        void timeout(Milliseconds t);

        Milliseconds connectTimeout() const;
        void connectTimeout(Milliseconds t);

        const IRemoteProcedure* activeProcedure() const;

        void cancel();

        void wait(Milliseconds msecs = WaitInfinite);

        const std::string& domain() const;

        void domain(const std::string& p);

        Delegate<bool, const SslCertificate&>& acceptSslCertificate();
};

}

}

#endif // CXXTOOLS_BIN_CLIENT_H
