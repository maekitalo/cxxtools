/*
 * Copyright (C) 2011 by Tommi Maekitalo
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
#ifndef CXXTOOLS_JSON_HTTPCLIENTIMPL_H
#define CXXTOOLS_JSON_HTTPCLIENTIMPL_H

#include <cxxtools/http/client.h>
#include <cxxtools/http/request.h>
#include <cxxtools/connectable.h>
#include <cxxtools/deserializer.h>
#include <cxxtools/formatter.h>
#include <cxxtools/refcounted.h>
#include <cxxtools/timespan.h>
#include <string>
#include "scanner.h"

namespace cxxtools
{

    class IRemoteProcedure;
    class IComposer;
    class IDecomposer;

namespace net
{
    class AddrInfo;
}

namespace json
{
    class HttpClientImpl : public RefCounted, public Connectable
    {
        public:
            HttpClientImpl();

            void prepareConnect(const net::AddrInfo& addrinfo, const std::string& url)
            {
                _client.prepareConnect(addrinfo);
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

            const std::string& url() const
            {
                return _request.url();
            }

            void beginCall(IComposer& r, IRemoteProcedure& method, IDecomposer** argv, unsigned argc);

            void endCall();

            void call(IComposer& r, IRemoteProcedure& method, IDecomposer** argv, unsigned argc);

            Timespan timeout() const  { return _timeout; }
            void timeout(Timespan t)  { _timeout = t; if (!_connectTimeoutSet) _connectTimeout = t; }

            Timespan connectTimeout() const  { return _connectTimeout; }
            void connectTimeout(Timespan t)  { _connectTimeout = t; _connectTimeoutSet = true; }

            const IRemoteProcedure* activeProcedure() const;

            void cancel();

            void wait(Timespan msecs);

        private:
            void prepareRequest(const String& name, IDecomposer** argv, unsigned argc);

            void onReplyHeader(http::Client& client);

            std::size_t onReplyBody(http::Client& client);

            void onReplyFinished(http::Client& client);

            static void verifyHeader(const http::ReplyHeader& header);

            // connection
            Timespan _timeout;
            bool _connectTimeoutSet;
            Timespan _connectTimeout;
            http::Client _client;

            http::Request _request;

            // serialization
            Scanner _scanner;
            DeserializerBase _deserializer;

            // current processing
            IRemoteProcedure* _proc;
            bool _exceptionPending;
            Formatter::int_type _count;
    };

}

}

#endif
