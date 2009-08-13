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
#include <cxxtools/selectable.h>
#include <cxxtools/signal.h>
#include <cxxtools/delegate.h>
#include <cxxtools/noncopyable.h>
#include <string>

namespace cxxtools {

class SelectorBase;

namespace http {

class ClientImpl;
class ReplyHeader;
class Request;

class CXXTOOLS_HTTP_API Client : private NonCopyable
{
        ClientImpl* _impl;

    public:
        Client();
        Client(const std::string& server, unsigned short int port);

        Client(SelectorBase& selector, const std::string& server, unsigned short int port);

        ~Client();

        // Sets the server and port. No actual network connect is done.
        void connect(const std::string& server, unsigned short int port);

        // Sends the passed request to the server and parses the headers.
        // The body must be read with readBody.
        // This method blocks or times out until the body is parsed.
        const ReplyHeader& execute(const Request& request,
            std::size_t timeout = Selectable::WaitInfinite);

        const ReplyHeader& header();

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

        void setSelector(SelectorBase& selector);

        // Executes the underlying selector until a event occures or the
        // specified timeout is reached.
        void wait(std::size_t msecs);

        // Returns the underlying stream, where the reply may be read from.
        std::istream& in();

        const std::string& server() const;

        unsigned short int port() const;

        // Sets the username and password for all subsequent requests.
        void auth(const std::string& username, const std::string& password);

        void clearAuth();

        // Signals that the request is sent to the server.
        Signal<Client&> requestSent;

        // Signals that the header is received.
        Signal<Client&> headerReceived;

        // This delegate is called, when data is arrived while reading the
        // body. The connected fuctor must return the number of bytes read.
        cxxtools::Delegate<std::size_t, Client&> bodyAvailable;

        // Signals that the reply is completely processed.
        Signal<Client&> replyFinished;

        // Signals that a exception is catched while processing the request.
        Signal<Client&, const std::exception&> errorOccured;
};

} // namespace http

} // namespace cxxtools

#endif
