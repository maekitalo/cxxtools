/* cxxtools/httpreply.cpp
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

#include <cxxtools/httpreply.h>
#include <cxxtools/httprequest.h>

#include <cctype>

namespace cxxtools
{
  HttpReply::HttpReply(HttpRequest& request)
    : std::istream(request.rdbuf()),
      returncode(0)
  {
    request.execute();
    parse_header();
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
