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

        HttpClient(SelectorBase& selector, const std::string& addr,
               unsigned short port, const std::string& url, bool ssl = false);

        HttpClient(SelectorBase& selector, const net::Uri& uri);

        HttpClient(const std::string& addr, unsigned short port, const std::string& url, bool ssl = false);

        explicit HttpClient(const net::Uri& uri);

        HttpClient(const HttpClient&);
        HttpClient& operator= (const HttpClient&);

        virtual ~HttpClient();

        void prepareConnect(const net::AddrInfo& addrinfo, const std::string& url, bool ssl = false);

        void prepareConnect(const net::Uri& uri);

        void prepareConnect(const std::string& addr, unsigned short port,
                     const std::string& url, bool ssl = false);

        void connect();

        void connect(const net::AddrInfo& addrinfo, const std::string& url, bool ssl = false)
        { prepareConnect(addrinfo, url, ssl); connect(); }

        void connect(const net::Uri& uri)
        { prepareConnect(uri); connect(); }

        void connect(const std::string& host, unsigned short port,
                     const std::string& url, bool ssl = false)
        { prepareConnect(host, port, url, false); connect(); }

        /// sets the url part
        void url(const std::string& url);

        /// sets username and password for authentification
        void auth(const std::string& username, const std::string& password);

        /// removes username and password for authentification
        void clearAuth();

        void setSelector(SelectorBase* selector);
        void setSelector(SelectorBase& selector);

        void wait(Milliseconds msecs = WaitInfinite);

    private:
        HttpClientImpl* _impl;
        HttpClientImpl* getImpl();
};

}

}

#endif
