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

#include <cxxtools/properties.h>
#include <cxxtools/utf8codec.h>
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <cctype>

namespace cxxtools
{
  namespace
  {
    class PropertiesEvent : public PropertiesParser::Event
    {
        Properties& properties;
        std::string key;

      public:
        PropertiesEvent(Properties& properties_)
          : properties(properties_)
          { }
        bool onKeyPart(const std::string& key);
        bool onKey(const std::string& key);
        bool onValue(const std::string& value);
    };

    bool PropertiesEvent::onKeyPart(const std::string& key)
    {
      return false;
    }

    bool PropertiesEvent::onKey(const std::string& key_)
    {
      key = key_;
      return false;
    }

    bool PropertiesEvent::onValue(const std::string& value)
    {
      properties.setValue(key, value);
      return false;
    }

    inline bool isKeyChar(Char ch)
    {
      return (ch >= 'a' && ch <= 'z')
          || (ch >= 'A' && ch <= 'Z')
          || (ch >= '0' && ch <= '9')
          || ch == '_';
    }

  }

  Properties::Properties(const std::string& filename)
  {
    PropertiesEvent ev(*this);
    std::ifstream in(filename.c_str());
    if (!in)
      throw std::runtime_error("could not open file \"" + filename + '"');
    PropertiesParser(ev).parse(in);
  }

  Properties::Properties(std::istream& in)
  {
    PropertiesEvent ev(*this);
    PropertiesParser(ev).parse(in);
  }

  void PropertiesParser::parse(std::istream& in, TextCodec<Char, char>* codec)
  {
    TextIStream ts(in, codec ? codec : new Utf8Codec());
    parse(ts);
  }

  void PropertiesParser::parse(TextIStream& in)
  {
    Char ch;
    while (in.get(ch) && !parse(ch))
      ;
    end();
  }

  bool PropertiesParser::parse(Char ch)
  {
    bool ret = false;
    switch (state)
    {
      case state_0:
        if (ch == '#')
          state = state_comment;
        else if (isKeyChar(ch))
        {
          key = ch.narrow();
          keypart = ch.narrow();
          state = state_key;
        }
        else if (!std::isspace(ch.value()) && ch != '\n' && ch != '\r')
          throw std::runtime_error("format error in properties");
        break;

      case state_key:
        if (ch == '.')
        {
          event.onKeyPart(keypart);
          keypart.clear();
          key += ch.narrow();
        }
        else if (isKeyChar(ch))
        {
          keypart += ch.narrow();
          key += ch.narrow();
        }
        else if (std::isspace(ch.value()))
        {
          ret = event.onKeyPart(keypart)
             || event.onKey(key);
          state = state_key_sp;
        }
        else if (ch == '=')
        {
          ret = event.onKeyPart(keypart)
             || event.onKey(key);
          state = state_value;
        }
        else
          throw std::runtime_error("parse error in properties while reading key " + key);
        break;

      case state_key_sp:
        if (ch == '=')
        {
          state = state_value;
        }
        else if (!std::isspace(ch.value()))
          throw std::runtime_error("parse error while reading key " + key);
        break;

      case state_value:
        if (ch == '\n')
        {
          ret = event.onValue(value);
          value.clear();
          state = state_0;
        }
        else if (ch == '\\')
          state = state_value_esc;
        else if (!value.empty() || !std::isspace(ch.value()))
          value += ch.narrow();
        break;

      case state_value_esc:
        value += ch.narrow();
        state = state_value;
        break;

      case state_comment:
        if (ch == '\n')
          state = state_0;
        break;
    }
    return ret;
  }

  void PropertiesParser::end()
  {
    switch (state)
    {
      case state_value:
      case state_value_esc:
        event.onValue(value);
        value.clear();
        break;

      case state_0:
      case state_comment:
        break;

      case state_key:
      case state_key_sp:
        throw std::runtime_error("parse error while reading key " + key);
    }
  }

}
