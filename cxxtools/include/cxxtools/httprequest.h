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
  /**
   Simple http-client class.

   This class implements a simple http-client.
   It supports http GET- and POST-methods. It lacks keep-alive.
   User can either read the reply from the client directy or let
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
      HttpRequest()
        : method(GET),
          port(80),
          reading(false)
        { }

      explicit HttpRequest(const std::string& url, request_type method_ = GET);

      HttpRequest(const std::string& host_, const std::string& url_, request_type method_ = GET)
        : method(method_),
          host(host_),
          url(url_),
          reading(false)
      { }

      HttpRequest(const std::string& host_, unsigned short port_, const std::string& url_,
          request_type method_ = GET)
        : method(method_),
          host(host_),
          url(url_),
          port(port_),
          reading(false)
      { }

      request_type getMethod() const      { return method; }
      void setMethod(request_type t)      { method = t; }

      const std::string& getHost() const  { return host; }
      void setHost(const std::string& h)  { host = h; }

      const std::string& getUrl() const   { return url; }
      void setUrl(const std::string& u)   { url = u; }

      const std::string& getBody() const  { return body; }
      void setBody(const std::string& b)  { body = b; }

      unsigned short getPort() const      { return port; }
      void setPort(unsigned short p)      { port = p; }

      QueryParams& getQueryParams()       { return params; }
      const QueryParams& getQueryParams() const                 { return params; }

      void set(const std::string& name, const std::string& value) { params.add(name, value); }
      void set(const std::string& value)                         { params.add(value); }

      void addHeader(const std::string& key, const std::string& value)
        { headers[key] = value; }
      void setAuth(const std::string& username, const std::string& password);

      void execute();

      std::streambuf* rdbuf()  { return connection.rdbuf(); }
  };
}

#endif // CXXTOOLS_HTTPREQUEST_H

