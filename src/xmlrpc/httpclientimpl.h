/*
 * Copyright (C) 2009 by Tommi Maekitalo
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
#ifndef cxxtools_xmlrpc_HttpClientImpl_h
#define cxxtools_xmlrpc_HttpClientImpl_h

#include <cxxtools/http/client.h>
#include <cxxtools/http/request.h>
#include <cxxtools/refcounted.h>
#include "clientimpl.h"

namespace cxxtools
{

namespace net
{
    class AddrInfo;
}

namespace xmlrpc
{

class HttpClientImpl : public RefCounted, public ClientImpl
{
    public:
        HttpClientImpl();

        void prepareConnect(const net::AddrInfo& addrinfo, const std::string& url, bool ssl)
        {
            _client.prepareConnect(addrinfo, ssl);
            _request.url(url);
        }

        void connect()
        {
            _client.connect();
        }

        void url(const std::string& url)
        {
            _request.url(url);
        }

        void auth(const std::string& username, const std::string& password)
        {
            _client.auth(username, password);
        }

        void clearAuth()
        {
            _client.clearAuth();
        }

        void setSelector(SelectorBase& selector)
        {
            _client.setSelector(selector);
        }

        std::string url() const;

        virtual void wait(std::size_t msecs);

    protected:
        void onReplyHeader(http::Client& client);

        std::size_t onReplyBody(http::Client& client);

        void onReplyFinished(http::Client& client);

        virtual void beginExecute();

        virtual void endExecute();

        virtual std::istream& execute();

        virtual std::ostream& prepareRequest();

        virtual void cancel();

    private:
        static void verifyHeader(const http::ReplyHeader& header);

        http::Client _client;
        http::Request _request;
};

}

}

#endif
