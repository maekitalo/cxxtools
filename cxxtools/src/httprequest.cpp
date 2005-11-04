/* cxxtools/httprequest.cpp
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

#include <cxxtools/httprequest.h>

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
    if (e != std::string::npos)
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

    url = url_.substr(e);
  }

  void HttpRequest::execute()
  {
    if (reading)
    {
      if (connection.peek() != std::ios::traits_type::eof())
        return;

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

        connection << " HTTP/1.0\r\nHost: " << host << "\r\n\r\n" << std::flush;
        break;

      case POST:
        {
          std::string p = params.getUrl();

          connection << "POST ";
          if (url.size() == 0 || url.at(0) != '/')
            connection << '/';

          connection << url << " HTTP/1.0\r\n"
                        "Host: " << host << "\r\n"
                        "Content-Length: " << p.size() << "\r\n"
                        "\r\n"
                     << p << std::flush;
        }
        break;
    }
    reading = true;
  }

  class HttpReply::Parser
  {
      HttpReply& reply;

      typedef bool (Parser::*state_type)(char ch);

      bool state_httpversion0(char ch);
      bool state_httpversion(char ch);
      bool state_code0(char ch);
      bool state_code(char ch);
      bool state_request(char ch);
      bool state_name0(char ch);
      bool state_name(char ch);
      bool state_value0(char ch);
      bool state_value(char ch);
      bool state_valuee(char ch);

      state_type state;

      std::string name;
      std::string value;

    public:
      Parser(HttpReply& reply_)
        : reply(reply_),
          state(&Parser::state_httpversion0)
        { }

      bool parse(char ch)
      { return (this->*state)(ch); }
  };

  bool HttpReply::Parser::state_httpversion0(char ch)
  {
    if (!std::isspace(ch))
      state = &Parser::state_httpversion;
    return false;
  }

  bool HttpReply::Parser::state_httpversion(char ch)
  {
    if (std::isspace(ch))
      state = &Parser::state_code0;
    return false;
  }

  bool HttpReply::Parser::state_code0(char ch)
  {
    if (std::isdigit(ch))
    {
      reply.returncode = (ch - '0');
      state = &Parser::state_code;
    }
    return false;
  }

  bool HttpReply::Parser::state_code(char ch)
  {
    if (std::isdigit(ch))
      reply.returncode = reply.returncode * 10 + (ch - '0');
    else
      state = &Parser::state_request;
    return false;
  }

  bool HttpReply::Parser::state_request(char ch)
  {
    if (ch == '\n')
      state = &Parser::state_name0;
    return false;
  }

  bool HttpReply::Parser::state_name0(char ch)
  {
    if (ch == '\n')
      return true;

    if (!std::isspace(ch))
    {
      name = ch;
      state = &Parser::state_name;
    }

    return false;
  }

  bool HttpReply::Parser::state_name(char ch)
  {
    if (ch == ':')
      state = &Parser::state_value0;
    else
      name += ch;
    return false;
  }

  bool HttpReply::Parser::state_value0(char ch)
  {
    if (ch == '\n')
    {
      reply.header.insert(header_type::value_type(name, value));
      state = &Parser::state_name0;
    }

    if (!std::isspace(ch))
    {
      value = ch;
      state = &Parser::state_value;
    }

    return false;
  }

  bool HttpReply::Parser::state_value(char ch)
  {
    if (ch == '\n')
    {
      reply.header.insert(header_type::value_type(name, value));
      state = &Parser::state_name0;
    }
    else if (ch == '\r')
    {
      reply.header.insert(header_type::value_type(name, value));
      state = &Parser::state_valuee;
    }
    else
      value += ch;

    return false;
  }

  bool HttpReply::Parser::state_valuee(char ch)
  {
    if (ch == '\n')
      state = &Parser::state_name0;
    return false;
  }

  void HttpReply::parse_header()
  {
    std::streambuf* buf = rdbuf();
    Parser p(*this);

    while (buf->sgetc() != std::ios::traits_type::eof())
    {
      char ch = buf->sbumpc();
      if (p.parse(ch))
        return;
    }
  }
}
