/*
 * Copyright (C) 2009 by Tommi Meakitalo
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
#ifndef cxxtools_xmlrpc_HttpClient_h
#define cxxtools_xmlrpc_HttpClient_h

#include <cxxtools/xmlrpc/client.h>

namespace cxxtools
{

class SelectorBase;
class SslCtx;

namespace net
{
    class AddrInfo;
    class Uri;
}

namespace xmlrpc
{

class HttpClientImpl;

/** A cxxtools::xmlrpc::HttpClient executes xmlrpc requests on a xmlrpc server.

 */
class HttpClient : public Client
{
    public:
        HttpClient()
            : _impl(0)
            { }

        explicit HttpClient(SelectorBase& selector)
            : _impl(0)
            { setSelector(selector); }

        HttpClient(const net::AddrInfo& addr, const std::string& url)
            : _impl(0)
            { prepareConnect(addr, url); }

        HttpClient(const net::AddrInfo& addr, const std::string& url, const SslCtx& sslCtx)
            : _impl(0)
            { prepareConnect(addr, url, sslCtx); }
        HttpClient(const std::string& addr, unsigned short port, const std::string& url)
            : _impl(0)
            { prepareConnect(addr, port, url); }
        HttpClient(const std::string& addr, unsigned short port, const std::string& url, const SslCtx& sslCtx)
            : _impl(0)
            { prepareConnect(addr, port, url, sslCtx); }
        explicit HttpClient(const net::Uri& uri)
            : _impl(0)
            { prepareConnect(uri); }
        HttpClient(const net::Uri& uri, const SslCtx& sslCtx)
            : _impl(0)
            { prepareConnect(uri, sslCtx); }

        HttpClient(SelectorBase& selector, const net::AddrInfo& addr, const std::string& url)
            : _impl(0)
            { setSelector(selector); prepareConnect(addr, url); }
        HttpClient(SelectorBase& selector, const net::AddrInfo& addr, const std::string& url, const SslCtx& sslCtx)
            : _impl(0)
            { setSelector(selector); prepareConnect(addr, url, sslCtx); }
        HttpClient(SelectorBase& selector, const std::string& addr, unsigned short port, const std::string& url)
            : _impl(0)
            { setSelector(selector); prepareConnect(addr, port, url); }
        HttpClient(SelectorBase& selector, const std::string& addr, unsigned short port, const std::string& url, const SslCtx& sslCtx)
            : _impl(0)
            { setSelector(selector); prepareConnect(addr, port, url, sslCtx); }
        HttpClient(SelectorBase& selector, const net::Uri& uri)
            : _impl(0)
            { setSelector(selector); prepareConnect(uri); }
        HttpClient(SelectorBase& selector, const net::Uri& uri, const SslCtx& sslCtx)
            : _impl(0)
            { setSelector(selector); prepareConnect(uri, sslCtx); }

        HttpClient(const HttpClient&);
        HttpClient& operator= (const HttpClient&);

        virtual ~HttpClient();

        void prepareConnect(const net::AddrInfo& addr, const std::string& url);
        void prepareConnect(const net::AddrInfo& addr, const std::string& url, const SslCtx& sslCtx);
        void prepareConnect(const std::string& host, unsigned short port, const std::string& url);
        void prepareConnect(const std::string& host, unsigned short port, const std::string& url, const SslCtx& sslCtx);
        void prepareConnect(const net::Uri& uri);
        void prepareConnect(const net::Uri& uri, const SslCtx& sslCtx);

        void connect(const net::AddrInfo& addrinfo, const std::string& url)
            { prepareConnect(addrinfo, url); connect(); }
        void connect(const net::AddrInfo& addrinfo, const std::string& url, const SslCtx& sslCtx)
            { prepareConnect(addrinfo, url, sslCtx); connect(); }
        void connect(const std::string& host, unsigned short int port, const std::string& url)
            { prepareConnect(host, port, url); connect(); }
        void connect(const std::string& host, unsigned short int port, const std::string& url, const SslCtx& sslCtx)
            { prepareConnect(host, port, url, sslCtx); connect(); }
        void connect(const net::Uri& uri)
            { prepareConnect(uri); connect(); }
        void connect(const net::Uri& uri, const SslCtx& sslCtx)
            { prepareConnect(uri, sslCtx); connect(); }

        void connect();

        /// sets the url part
        void url(const std::string& url);

        /// sets username and password for authentification
        void auth(const std::string& username, const std::string& password);

        /// removes username and password for authentification
        void clearAuth();

        void setSelector(SelectorBase* selector);
        void setSelector(SelectorBase& selector) { setSelector(&selector); }

        void wait(Milliseconds msecs = WaitInfinite);

    private:
        HttpClientImpl* _impl;
        HttpClientImpl* getImpl();
};

}

}

#endif
