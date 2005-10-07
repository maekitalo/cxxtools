/* cxxtools/httprequest.h
   Copyright (C) 2005 Tommi Maekitalo

This file is part of cxxtools.

Cxxtools is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

Cxxtools is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with cxxtools; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330,
Boston, MA  02111-1307  USA
*/

#ifndef CXXTOOLS_HTTPREQUEST_H
#define CXXTOOLS_HTTPREQUEST_H

#include <cxxtools/tcpstream.h>
#include <cxxtools/query_params.h>
#include <map>

namespace cxxtools
{
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
      unsigned short port;
      bool reading;

      QueryParams params;

      net::iostream connection;

    public:
      explicit HttpRequest()
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

      unsigned short getPort() const      { return port; }
      void setPort(unsigned short p)      { port = p; }

      QueryParams& getQueryParams()                             { return params; }
      const QueryParams& getQueryParams() const                 { return params; }

      void set(const std::string name, const std::string value) { params.add(name, value); }
      void set(const std::string value)                         { params.add(value); }

      void execute();

      std::streambuf* rdbuf()  { return connection.rdbuf(); }
  };

  class HttpReply : public std::istream
  {
      class Parser;
      friend class Parser;

      void parse_header();

      typedef std::map<std::string, std::string> header_type;
      header_type header;
      unsigned returncode;

    public:
      HttpReply(HttpRequest& request)
        : std::istream(request.rdbuf()),
          returncode(0)
      {
        request.execute();
        parse_header();
      }

      HttpReply(std::istream& request)
        : std::istream(request.rdbuf())
      {
        parse_header();
      }

      const std::string& getHeader(const std::string& name, const std::string& def) const
      {
        header_type::const_iterator it = header.find(name);
        return it == header.end() ? def : it->second;
      }

      unsigned getReturnCode()  { return returncode; }
  };
}

#endif // CXXTOOLS_HTTPREQUEST_H

