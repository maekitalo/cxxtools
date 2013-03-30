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

#ifndef cxxtools_Http_ClientImpl_h
#define cxxtools_Http_ClientImpl_h

#include <cxxtools/net/tcpserver.h>
#include <cxxtools/net/tcpsocket.h>
#include "parser.h"
#include <cxxtools/http/request.h>
#include <cxxtools/http/reply.h>
#include <cxxtools/selectable.h>
#include <cxxtools/iostream.h>
#include <cxxtools/timer.h>
#include <cxxtools/connectable.h>
#include <cxxtools/delegate.h>
#include <string>
#include <sstream>
#include <cstddef>
#include "chunkedreader.h"

namespace cxxtools
{

namespace net
{
    class Uri;
}

namespace http
{

class Client;

class ClientImpl : public Connectable
{
        friend class ParseEvent;

        Client* _client;

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

        net::AddrInfo _addrInfo;
        net::TcpSocket _socket;
        IOStream _stream;
        ChunkedIStream _chunkedIStream;
        std::string _username;
        std::string _password;

        long _contentLength;
        bool _readHeader;
        bool _chunkedEncoding;
        bool _reconnectOnError;
        bool _errorPending;

        void sendRequest(const Request& request);
        void processHeaderAvailable(StreamBuffer& sb);
        void processBodyAvailable(StreamBuffer& sb);

        void reexecute(const Request& request);
        void reexecuteBegin(const Request& request);
        void doparse();

    protected:
        void onConnect(net::TcpSocket& socket);
        void onOutput(StreamBuffer& sb);
        void onInput(StreamBuffer& sb);

    public:
        ClientImpl(Client* client);
        ClientImpl(Client* client, const net::AddrInfo& addrinfo);
        ClientImpl(Client* client, const net::Uri& uri);
        ClientImpl(Client* client, SelectorBase& selector, const net::AddrInfo& addrinfo);
        ClientImpl(Client* client, SelectorBase& selector, const net::Uri& uri);

        // Sets the server and port. No actual network connect is done.
        void connect(const net::AddrInfo& addrinfo)
        {
            _addrInfo = addrinfo;
            _socket.close();
        }

        // Sends the passed request to the server and parses the headers.
        // The body must be read with readBody.
        // This method blocks or times out until the body is parsed.
        const ReplyHeader& execute(const Request& request,
            std::size_t timeout = Selectable::WaitInfinite);

        const ReplyHeader& header()
        { return _replyHeader; }

        // Reads the http body after header read with execute.
        // This method blocks until the body is received.
        void readBody(std::string& s);

        // Reads the http body after header read with execute.
        // This method blocks until the body is received.
        std::string readBody()
        {
            std::string ret;
            readBody(ret);
            return ret;
        }

        // Combines the execute and readBody methods in one call.
        // This method blocks until the reply is recieved.
        std::string get(const std::string& url,
            std::size_t timeout = Selectable::WaitInfinite);

        // Starts a new request.
        // This method does not block. To actually process the request, the
        // event loop must be executed. The state of the request is signaled
        // with the corresponding signals and delegates.
        // The delegate "bodyAvailable" must be connected, if a body is
        // received.
        void beginExecute(const Request& request);

        void endExecute();

        void setSelector(SelectorBase& selector);

        SelectorBase* selector();

        // Executes the underlying selector until a event occures or the
        // specified timeout is reached.
        bool wait(std::size_t msecs);

        // Returns the underlying stream, where the reply may be read from.
        std::istream& in()
        {
            return _chunkedEncoding ? static_cast<std::istream&>(_chunkedIStream)
                                    : static_cast<std::istream&>(_stream);
        }

        const std::string& host() const
        {
            return _addrInfo.host();
        }

        unsigned short port() const
        {
            return _addrInfo.port();
        }

        // Sets the username and password for all subsequent requests.
        void auth(const std::string& username, const std::string& password)
        { _username = username; _password = password; }

        void clearAuth()
        { _username.clear(); _password.clear(); }

        void cancel();
};

} // namespace http

} // namespace cxxtools

#endif
