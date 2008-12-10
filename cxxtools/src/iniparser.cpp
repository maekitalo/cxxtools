/*
 * Copyright (C) 2007 Tommi Maekitalo
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

#include <cxxtools/iniparser.h>
#include <cxxtools/log.h>
#include <cctype>
#include <iostream>
#include <stdexcept>

log_define("cxxtools.iniparser")

namespace cxxtools
{
  bool IniParser::Event::onSection(const std::string& section)
  {
    return false;
  }

  bool IniParser::Event::onKey(const std::string& key)
  {
    return false;
  }

  bool IniParser::Event::onValue(const std::string& key)
  {
    return false;
  }

  bool IniParser::Event::onComment(const std::string& comment)
  {
    return false;
  }

  bool IniParser::Event::onError()
  {
    throw std::runtime_error("parse error in ini-file");
    return true;
  }

  void IniParser::parse(std::istream& in)
  {
    char ch;
    while (in.get(ch) && !parse(ch))
      ;
    end();
  }

  bool IniParser::parse(char ch)
  {
    bool ret = false;
    switch (state)
    {
      case state_0:
        if (ch == '[')
          state = state_section;
        else if (std::isalnum(ch))
        {
          data = ch;
          state = state_key;
        }
        else if (ch == '#')
        {
          state = state_comment;
        }
        else if (std::isspace(ch))
          ;
        else
        {
          log_debug("onError");
          ret = event.onError();
        }
        break;

      case state_section:
        if (ch == ']')
        {
          log_debug("onSection(" << data << ')');
          ret = event.onSection(data);
          data.clear();
          state = state_0;
        }
        else
          data += ch;
        break;

      case state_key:
        if (ch == '=')
        {
          log_debug("onKey(" << data << ')');
          ret = event.onKey(data);
          state = state_value0;
        }
        else if (std::isspace(ch))
        {
          log_debug("onKey(" << data << ')');
          ret = event.onKey(data);
          state = state_key_sp;
        }
        else
          data += ch;
        break;

      case state_key_sp:
        if (ch == '=')
          state = state_value0;
        else if (!std::isspace(ch))
        {
          log_debug("onError");
          ret = event.onError();
        }
        break;

      case state_value0:
        if (ch == '\n')
        {
          log_debug("onValue(\"\")");
          ret = event.onValue(std::string());
          state = state_0;
        }
        else if (!std::isspace(ch))
        {
          data = ch;
          state = state_value;
        }
        break;

      case state_value:
        if (ch == '\n')
        {
          log_debug("onValue(" << data << ')');
          ret = event.onValue(data);
          data.clear();
          state = state_0;
        }
        else
          data += ch;
        break;

      case state_comment:
        if (ch == '\n')
          state = state_0;
        break;
    }

    return ret;
  }

  void IniParser::end()
  {
    switch (state)
    {
      case state_0:
      case state_comment:
        break;

      case state_section:
      case state_key:
      case state_key_sp:
        log_debug("onError");
        event.onError();
        break;

      case state_value0:
        log_debug("onValue(\"\")");
        event.onValue(std::string());
        break;

      case state_value:
        log_debug("onValue" << data << ')');
        event.onValue(data);
        break;
    }
  }

}
