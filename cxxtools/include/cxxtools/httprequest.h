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

namespace cxxtools
{
  class httprequest : public std::iostream
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

      query_params params;

      tcp::iostream connection;

    public:
      httprequest(const std::string& host_, const std::string& url_, request_type method_ = GET)
        : std::iostream(0),
          method(method_),
          host(host_),
          url(url_),
          reading(false)
      {
        init(connection.rdbuf());
      }

      httprequest(const std::string& host_, unsigned short port_, const std::string& url_,
          request_type method_ = GET)
        : std::iostream(0),
          method(method_),
          host(host_),
          url(url_),
          port(port_),
          reading(false)
      {
        init(connection.rdbuf());
      }

      query_params& getQueryParams()                            { return params; }
      void set(const std::string name, const std::string value) { params.add(name, value); }
      void set(const std::string value)                         { params.add(value); }

      void execute();
  };

}

#endif // CXXTOOLS_HTTPREQUEST_H

