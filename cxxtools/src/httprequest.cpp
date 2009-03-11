/*
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
 */

#include <cxxtools/httprequest.h>
#include <cxxtools/base64stream.h>
#include <cxxtools/log.h>
#include <sstream>
#include <cctype>

log_define("cxxtools.httprequest")

namespace cxxtools
{
  HttpRequest::HttpRequest(const std::string& url_, request_type method_)
    : method(method_),
      port(80),
      reading(false)
  {
    std::string::size_type pos = 0;
    if (url_.compare(0, 7, "http://") == 0)
      pos = 7;

    std::string::size_type e = url_.find(':', pos);
    std::string::size_type e2 = url_.find('/', pos);
    if (e != std::string::npos && (e2 == std::string::npos || e2 > e))
    {
      host = url_.substr(pos, e - pos);
      port = 0;
      for (++e; e < url_.size() && url_.at(e) != '/'; ++e)
      {
        if (!std::isdigit(url_.at(e)))
          throw std::runtime_error("invalid url \"" + url_ + '"');
        port = port * 10 + (url_.at(e) - '0');
      }
      if (e >= url_.size())
        throw std::runtime_error("invalid url \"" + url_ + '"');
    }
    else
    {
      e = url_.find('/', pos);
      if (e == std::string::npos)
        throw std::runtime_error("invalid url \"" + url_ + '"');
      host = url_.substr(pos, e - pos);
    }

    std::string::size_type q = url_.find('?', e);
    if (q != std::string::npos)
    {
      url = url_.substr(e, q - e);
      params.parse_url(url_.substr(q + 1));
    }
    else
      url = url_.substr(e);

    log_debug("host=" << host << " port=" << port << " url=" << url << " qparams=" << params.getUrl());
  }

  void HttpRequest::setAuth(const std::string& username, const std::string& password)
  {
    std::ostringstream value;
    value << "Basic ";
    Base64ostream encoder(value);
    encoder << username << ':' << password;
    encoder.end();
    addHeader("Authorization:", value.str());
  }

  net::iostream& HttpRequest::execute()
  {
    if (reading)
    {
      if (connection.peek() != std::ios::traits_type::eof())
        return connection;

      connection.close();
      connection.clear();
    }

    connection.connect(host, port);
    switch (method)
    {
      case GET:
        connection << "GET ";
        if (url.size() == 0 || url.at(0) != '/')
          connection << '/';
        connection << url;

        if (!params.empty())
          connection << '?' << params.getUrl();

        connection << " HTTP/1.1\r\n"
                      "Host: " << host << "\r\n"
                      "Connection: close\r\n";
        for (Headers::const_iterator it = headers.begin(); it != headers.end(); ++it)
          connection << it->first << ' ' << it->second << "\r\n";
        connection << "\r\n" << std::flush;
        break;

      case POST:
        {
          std::string b = (body.empty() ? params.getUrl() : body);

          connection << "POST ";
          if (url.size() == 0 || url.at(0) != '/')
            connection << '/';

          connection << url << " HTTP/1.1\r\n"
                        "Host: " << host << "\r\n"
                        "Content-Length: " << b.size() << "\r\n"
                        "Content-Type: application/x-www-form-urlencoded\r\n"
                        "Connection: close\r\n";
          for (Headers::const_iterator it = headers.begin(); it != headers.end(); ++it)
            connection << it->first << ' ' << it->second << "\r\n";
          connection << "\r\n"
                     << b << std::flush;
        }
        break;
    }
    reading = true;

    return connection;
  }

}
