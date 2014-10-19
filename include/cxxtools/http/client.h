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
#include <cxxtools/http/reply.h>
#include <cxxtools/query_params.h>
#include <cxxtools/selectable.h>
#include <cxxtools/signal.h>
#include <cxxtools/delegate.h>
#include <string>

namespace cxxtools
{

class SelectorBase;

namespace net
{

class AddrInfo;
class Uri;

}

namespace http
{

class ClientImpl;
class ReplyHeader;
class Request;

/**
 This class implements a http client.

 The class supports syncronous and asyncronous requests.

 For connection handling see \ref connection.

 For asyncronous I/O see \ref asyncronousIO.

 Example for a syncronous call:

 \code
   cxxtools::http::Client client("www.tntnet.org", 80);
   std::string indexPage = client.get("/");
 \endcode

 */
class CXXTOOLS_HTTP_API Client
{
        ClientImpl* _impl;
        ClientImpl* getImpl();
        const ClientImpl* getImpl() const
        { return const_cast<Client*>(this)->getImpl(); }

    public:
        /// default constructor.
        Client()
            : _impl(0)
        { }

        ///@{
        /// constructors which set the connection parameters and optionally connect to the server.
        explicit Client(const net::AddrInfo& addr);
        Client(const std::string& host, unsigned short int port);
        ///@}

        explicit Client(SelectorBase& selector)
            : _impl(0)
        { setSelector(selector); }

        /** constructor with cxxtools::net::Uri.
            Note that the Uri class has a non explicit constructor from std::string.
            The protocol of the uri must be http. The url part of the uri is ignored.

            This makes using uris easy.

            example:
            \code
              cxxtools::http::Client client("http://localhost:8000/");
            \endcode
         */
        explicit Client(const net::Uri& uri);

        ///@{
        /// constructors which set the selector for asyncronous request processing and the connection parameters.

        Client(SelectorBase& selector, const std::string& host, unsigned short int port);
        Client(SelectorBase& selector, const net::AddrInfo& addrinfo);
        Client(SelectorBase& selector, const net::Uri& uri);

        ///@}

        /** copy and assignment.
            When the class is copied, the copy points to the same implementation.
            Hence it is not a real copy.
         */

        Client(const Client& other);
        Client& operator= (const Client& other);

        ~Client();

        /** The prepareConnect methods set the host and port of the server for this http client.

           No actual network connect is done.

           \see
             \ref connection
         */
        void prepareConnect(const net::AddrInfo& addrinfo);
        void prepareConnect(const std::string& host, unsigned short int port);
        void prepareConnect(const net::Uri& uri);

        /** Connects to the client specified by prepareConnect or passed by teh constructor.

            This happens actually automatically when needed. It just helps checking whether
            the server is reachable, since setting the connection paramters do not check it.
         */
        void connect();

        /** Closes the network connection if connected.

            The network connection is automaticalle initiated on the next request.
            The method do not discard the current connection parameters.
         */
        void close();

        /** Sets the network parameters and connects the socket.
         */
        void connect(const net::AddrInfo& addrinfo)
        { prepareConnect(addrinfo); connect(); }

        void connect(const std::string& host, unsigned short int port)
        { prepareConnect(host, port); connect(); }

        void connect(const net::Uri& uri)
        { prepareConnect(uri); connect(); }

        /** Sends the passed request to the server and parses the headers.

            The body must be read with readBody.
            This method blocks or times out until the body is parsed.
            When connectTimeout is set to WaitInfinite but timeout is set to
            something else, the connectTimeout is set to timeout as well.
        */
        const ReplyHeader& execute(const Request& request,
            Milliseconds timeout = Selectable::WaitInfinite,
            Milliseconds connectTimeout = Selectable::WaitInfinite);

        /** Reads the http body after header read with execute.

            This method blocks until the body is received.
         */
        const Reply& readBody();

        /** Returns the reply of the last executed request.
         */
        const Reply& reply() const;
        Reply& reply();

        /** Returns the headers of the last executed reply.

            The headers can be read after calling execute().
         */
        const ReplyHeader& header() const
        { return reply().header(); }

        /** Returns the body of the last executed request.

            The readBody() method must be called before the body is available.
         */
        std::string body() const
        { return reply().body(); }

        /** Executes a GET request with query parameters.

            This method blocks until the reply is recieved.
            When connectTimeout is set to WaitInfinite but timeout is set to
            something else, the connectTimeout is set to timeout as well.
          */
        const Reply& get(const std::string& url,
            const QueryParams& qparams,
            Milliseconds timeout = Selectable::WaitInfinite,
            Milliseconds connectTimeout = Selectable::WaitInfinite);

        /** Executes a GET request.

            This method blocks until the reply is recieved.
            When connectTimeout is set to WaitInfinite but timeout is set to
            something else, the connectTimeout is set to timeout as well.
          */
        const Reply& get(const std::string& url,
            Milliseconds timeout = Selectable::WaitInfinite,
            Milliseconds connectTimeout = Selectable::WaitInfinite)
        { return get(url, QueryParams(), timeout, connectTimeout); }

        /** Executes a POST request with query parameters.

            This method blocks until the reply is recieved.
            When connectTimeout is set to WaitInfinite but timeout is set to
            something else, the connectTimeout is set to timeout as well.
          */
        const Reply& post(const std::string& url,
            const QueryParams& qparams,
            Milliseconds timeout = Selectable::WaitInfinite,
            Milliseconds connectTimeout = Selectable::WaitInfinite);

        /** Executes a POST request.

            This method blocks until the reply is recieved.
            When connectTimeout is set to WaitInfinite but timeout is set to
            something else, the connectTimeout is set to timeout as well.
          */
        const Reply& post(const std::string& url,
            Milliseconds timeout = Selectable::WaitInfinite,
            Milliseconds connectTimeout = Selectable::WaitInfinite)
        { return post(url, QueryParams(), timeout, connectTimeout); }

        /** Starts a new request.

            This method does not block. To actually process the request, the
            event loop must be executed. The state of the request is signaled
            with the corresponding signals and delegates.
            The delegate "bodyAvailable" must be connected, if a body is
            received.
         */
        void beginExecute(const Request& request);

        /** Finishes the request.

            When the signal replyFinished is issued, the user has to call
            endExecute. This may throw an exception when something went wrong.
         */
        void endExecute();

        /// Sets the selector for asyncronous event processing.
        void setSelector(SelectorBase& selector);

        /// Returns the selector for asyncronous event processing.
        SelectorBase* selector();

        /** Executes the underlying selector until a event occurs or the
            specified timeout is reached.
         */
        bool wait(Milliseconds msecs);

        /** Returns the underlying stream, where the reply is to be read from.
         */
        std::istream& in();

        const std::string& host() const;

        unsigned short int port() const;

        /** Sets the username and password for all subsequent requests.

            Basic authorization is used always.
         */
        void auth(const std::string& username, const std::string& password);

        /** Clears the username and password for all subsequent requests.
         */
        void clearAuth();

        void cancel();

        /// Signals that the request is sent to the server.
        Signal<Client&> requestSent;

        /// Signals that the header is received.
        Signal<Client&> headerReceived;

        /// This delegate is called, when data is arrived while reading the
        /// body. The connected functor must return the number of bytes read.
        cxxtools::Delegate<std::size_t, Client&> bodyAvailable;

        /// Signals that the reply is completely processed.
        Signal<Client&> replyFinished;
};

} // namespace http

} // namespace cxxtools

#endif
