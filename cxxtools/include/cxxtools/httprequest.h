/* cxxtools/httprequest.h
 *
 * cxxtools - general purpose C++-toolbox
 * Copyright (C) 2005 Tommi Maekitalo
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
 *
 */

#ifndef CXXTOOLS_HTTPREQUEST_H
#define CXXTOOLS_HTTPREQUEST_H

#include <cxxtools/tcpstream.h>
#include <cxxtools/query_params.h>
#include <map>

namespace cxxtools
{
  class HttpReply;

  /**
   Simple http-client class.

   This class implements a simple http-client.
   It supports http GET- and POST-methods.
   User can either read the reply from the client directly using the underlying
   stream returned by methods execute() or getStream() or let
   cxxtools::HttpReply parse the reply.

   example:
   \code
    cxxtools::HttpRequest request("http://www.tntnet.org/");
    cxxtools::HttpReply reply(request); // this does the actual request
    // print the reply:
    std::cout << "the server returned the http-code " << reply.getReturnCode() << '\n'
              << "The body is: " << reply.rdbuf() << std::endl;
    // note that printing a std::streambuf returned by reply.rebuf() copies the source
    // to the ostream.
   \endcode

   If you want to have the body, sent by the server in a std::string, you may
   use std::ostringstream:
   \code
   std::ostringstream os;
   os << reply.rdbuf(); // this copies the body to the ostringstream
   std::string body = os.str();  // get the body from the stream
   \endcode
   */
  class HttpRequest
  {
      friend class HttpReply;

    public:
      enum request_type {
        GET, POST
      } type;

    private:
      request_type method;
      std::string host;
      std::string url;
      std::string body;
      unsigned short port;
      bool reading;

      QueryParams params;

      net::iostream connection;

      typedef std::map<std::string, std::string> Headers;
      Headers headers;

    public:
      /// Initializes an empty http-request object.
      /// At least the host must be set, before the request can be executed.
      /// The port is set to the default http port 80.
      HttpRequest()
        : method(GET),
          port(80),
          reading(false)
        { }

      /// Initializes a http-request object with a given url.
      /// The url is parsed. The url may have a port and query parameters.
      /// e.g. "http://localhost:8080/mypage?param1=15".
      explicit HttpRequest(const std::string& url, request_type method_ = GET);

      /// Initializes a http-request object with a host and a url.
      HttpRequest(const std::string& host_, const std::string& url_, request_type method_ = GET)
        : method(method_),
          host(host_),
          url(url_),
          reading(false)
      { }

      /// Initializes a http-request object with a host, a port and a url.
      HttpRequest(const std::string& host_, unsigned short port_, const std::string& url_,
          request_type method_ = GET)
        : method(method_),
          host(host_),
          url(url_),
          port(port_),
          reading(false)
      { }

      /// Returns the request method GET or POST set for this request.
      request_type getMethod() const      { return method; }
      /// Sets the request method GET or POST for this request.
      void setMethod(request_type t)      { method = t; }

      /// Returns the current host for this request.
      const std::string& getHost() const  { return host; }
      /// Sets the current host for this request.
      void setHost(const std::string& h)  { host = h; }

      /// Returns the current url for this request.
      const std::string& getUrl() const   { return url; }
      /// Sets the current url for this request.
      void setUrl(const std::string& u)   { url = u; }

      /// Returns the request body for this request.
      const std::string& getBody() const  { return body; }
      /// Sets the request body for this request.
      void setBody(const std::string& b)  { body = b; }

      /// Returns the port for this request.
      unsigned short getPort() const      { return port; }
      /// Sets the port for this request.
      void setPort(unsigned short p)      { port = p; }

      /// Returns the query parameters for this request.
      /// For POST requests these query parameters are sent in the body
      /// only if the body is not set otherwise.
      QueryParams& getQueryParams()       { return params; }
      /// Sets the query parameters for this request.
      const QueryParams& getQueryParams() const                 { return params; }

      /// Adds a single named query parameters for this request.
      void set(const std::string& name, const std::string& value) { params.add(name, value); }
      /// Adds a single unnamed query parameters for this request.
      void set(const std::string& value)                         { params.add(value); }

      /// Adds a http request header for this request.
      void addHeader(const std::string& key, const std::string& value)
        { headers[key] = value; }
      /// Sets the http-username and password used for http basic auth.
      void setAuth(const std::string& username, const std::string& password);

      /// Executes the request and returns a stream to the associated reply.
      net::iostream& execute();

      /// Returns the streambuffer for this request.
      std::streambuf* rdbuf()  { return connection.rdbuf(); }

      /// Returns a iostream, which is connected to the server.
      net::iostream& getStream()   { return connection; }

  };
}

#endif // CXXTOOLS_HTTPREQUEST_H

