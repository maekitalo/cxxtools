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
  void httprequest::execute()
  {
    if (reading)
    {
      if (connection.peek() != std::ios::traits_type::eof())
        return;

      connection.close();
      connection.clear();
    }

    connection.Connect(host, port);
    switch (method)
    {
      case GET:
        connection << "GET ";
        if (url.size() == 0 || url.at(0) != '/')
          connection << '/';
        connection << url;

        if (!params.empty())
          connection << '?' << params.getUrl();

        connection << " HTTP/1.0\r\n\r\n" << std::flush;
        break;

      case POST:
        {
          std::string p = params.getUrl();

          connection << "POST ";
          if (url.size() == 0 || url.at(0) != '/')
            connection << '/';

          connection << url << " HTTP/1.0\r\n"
                        "Content-Length: " << p.size() << "\r\n"
                        "\r\n"
                     << p << std::flush;
        }
        break;
    }
    reading = true;
  }

  class httpreply::parser
  {
      httpreply& reply;

      typedef bool (parser::*state_type)(char ch);

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
      parser(httpreply& reply_)
        : reply(reply_),
          state(&parser::state_httpversion0)
        { }

      bool parse(char ch)
      { return (this->*state)(ch); }
  };

  bool httpreply::parser::state_httpversion0(char ch)
  {
    if (!std::isspace(ch))
      state = &parser::state_httpversion;
    return false;
  }

  bool httpreply::parser::state_httpversion(char ch)
  {
    if (std::isspace(ch))
      state = &parser::state_code0;
    return false;
  }

  bool httpreply::parser::state_code0(char ch)
  {
    if (std::isdigit(ch))
    {
      reply.returncode = (ch - '0');
      state = &parser::state_code;
    }
    return false;
  }

  bool httpreply::parser::state_code(char ch)
  {
    if (std::isdigit(ch))
      reply.returncode = reply.returncode * 10 + (ch - '0');
    else
      state = &parser::state_request;
    return false;
  }

  bool httpreply::parser::state_request(char ch)
  {
    if (ch == '\n')
      state = &parser::state_name0;
    return false;
  }

  bool httpreply::parser::state_name0(char ch)
  {
    if (ch == '\n')
      return true;

    if (!std::isspace(ch))
    {
      name = ch;
      state = &parser::state_name;
    }

    return false;
  }

  bool httpreply::parser::state_name(char ch)
  {
    if (ch == ':')
      state = &parser::state_value0;
    else
      name += ch;
    return false;
  }

  bool httpreply::parser::state_value0(char ch)
  {
    if (ch == '\n')
    {
      reply.header.insert(header_type::value_type(name, value));
      state = &parser::state_name0;
    }

    if (!std::isspace(ch))
    {
      value = ch;
      state = &parser::state_value;
    }

    return false;
  }

  bool httpreply::parser::state_value(char ch)
  {
    if (ch == '\n')
    {
      reply.header.insert(header_type::value_type(name, value));
      state = &parser::state_name0;
    }
    else if (ch == '\r')
    {
      reply.header.insert(header_type::value_type(name, value));
      state = &parser::state_valuee;
    }
    else
      value += ch;

    return false;
  }

  bool httpreply::parser::state_valuee(char ch)
  {
    if (ch == '\n')
      state = &parser::state_name0;
    return false;
  }

  void httpreply::parse_header()
  {
    std::streambuf* buf = rdbuf();
    parser p(*this);

    while (buf->sgetc() != std::ios::traits_type::eof())
    {
      char ch = buf->sbumpc();
      if (p.parse(ch))
        return;
    }
  }
}
