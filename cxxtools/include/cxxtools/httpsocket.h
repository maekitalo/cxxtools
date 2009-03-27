/*
 * Copyright (C) 2009 by Marc Boris Duerner, Tommi Maekitalo
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

#ifndef cxxtools_Net_HttpSocket_h
#define cxxtools_Net_HttpSocket_h

#include <cxxtools/api.h>
#include <cxxtools/tcpsocket.h>
#include <cxxtools/httpparser.h>
#include <cxxtools/httprequest.h>
#include <cxxtools/httpreply.h>
#include <cxxtools/iostream.h>
#include <cxxtools/timer.h>
#include <cxxtools/connectable.h>

namespace cxxtools {

namespace net {

class HttpServer;
class HttpResponder;

class CXXTOOLS_API HttpSocket : public TcpSocket, public Connectable
{
        class ParseEvent : public HttpHeaderParser::HttpMessageHeaderEvent
        {
                HttpRequest& _request;

            public:
                explicit ParseEvent(HttpRequest& request)
                    : HttpHeaderParser::HttpMessageHeaderEvent(request.header()),
                      _request(request)
                    { }

                virtual void onMethod(const std::string& method);
                virtual void onUrl(const std::string& url);
                virtual void onUrlParam(const std::string& q);
        };

    public:
        HttpSocket(SelectorBase& s, HttpServer& server);

        void onInput(StreamBuffer& stream);
        bool onOutput(StreamBuffer& stream);
        void onTimeout();

        bool doReply();
        void sendReply();
        bool isReady() const
        { return _parser.end() && _contentLength == 0; }

        void addSelector(SelectorBase& s);
        void removeSelector();

        const HttpRequest& request() const { return _request; }
        const HttpReply& reply() const     { return _reply; }

    private:
        HttpServer& _server;

        ParseEvent _parseEvent;
        HttpHeaderParser _parser;
        HttpRequest _request;
        HttpReply _reply;

        Timer _timer;
        int _contentLength;
        HttpResponder* _responder;
        IOStream _stream;

};

} // namespace net

} // namespace cxxtools

#endif
