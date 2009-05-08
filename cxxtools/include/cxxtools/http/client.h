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

#ifndef cxxtools_Http_Client_h
#define cxxtools_Http_Client_h

#include <cxxtools/http/api.h>
#include <cxxtools/net/tcpserver.h>
#include <cxxtools/net/tcpsocket.h>
#include <cxxtools/http/parser.h>
#include <cxxtools/http/request.h>
#include <cxxtools/http/reply.h>
#include <cxxtools/selectable.h>
#include <cxxtools/iostream.h>
#include <cxxtools/timer.h>
#include <cxxtools/connectable.h>
#include <cxxtools/delegate.h>
#include <string>
#include <sstream>
#include <map>
#include <cstddef>

namespace cxxtools {

namespace http {

class CXXTOOLS_HTTP_API Client : public cxxtools::Connectable
{
        friend class ParseEvent;

        class CXXTOOLS_HTTP_API ParseEvent : public HeaderParser::MessageHeaderEvent
        {
                ReplyHeader& _replyHeader;

            public:
                explicit ParseEvent(ReplyHeader& replyHeader)
                    : HeaderParser::MessageHeaderEvent(replyHeader),
                      _replyHeader(replyHeader)
                    { }

                void onHttpReturn(unsigned ret, const std::string& text);
        };

        ParseEvent _parseEvent;
        HeaderParser _parser;

        const Request* _request;
        ReplyHeader _replyHeader;

        std::string _server;
        unsigned short int _port;
        net::TcpSocket _socket;
        IOStream _stream;
        bool _readHeader;
        long _contentLength;

        void sendRequest(const Request& request);
        void processHeaderAvailable(StreamBuffer& sb);
        void processBodyAvailable(StreamBuffer& sb);

        void reexecute(const Request& request);
        void doparse();

    protected:
        void onConnect(net::TcpSocket& socket);
        void onOutput(StreamBuffer& sb);
        void onInput(StreamBuffer& sb);

    public:
        Client(const std::string& server, unsigned short int port);

        Client(SelectorBase& selector, const std::string& server, unsigned short int port);

        const ReplyHeader& execute(const Request& request,
            std::size_t timeout = Selectable::WaitInfinite);

        const ReplyHeader& header()
        { return _replyHeader; }

        void readBody(std::string& s);

        std::string readBody()
        {
            std::string ret;
            readBody(ret);
            return ret;
        }

        std::string get(const std::string& url,
            std::size_t timeout = Selectable::WaitInfinite);

        void beginExecute(const Request& request);

        void setSelector(SelectorBase& selector);

        void wait(std::size_t msecs);

        std::istream& in()   // reply body is received here
        {
            return _stream;
        }

        Signal<Client&> requestSent;
        Signal<Client&> headerReceived;
        cxxtools::Delegate<std::size_t, Client&> bodyAvailable;
        Signal<Client&> replyFinished;
        Signal<Client&, const std::exception&> errorOccured;
};

} // namespace http

} // namespace cxxtools

#endif
