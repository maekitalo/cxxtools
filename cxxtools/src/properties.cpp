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
#include <sstream>
#include <cctype>

namespace cxxtools
{
  namespace
  {
    class PropertiesEvent : public PropertiesParser::Event
    {
        Properties& properties;
        String key;

      public:
        PropertiesEvent(Properties& properties_)
          : properties(properties_)
          { }
        bool onKeyPart(const String& key);
        bool onKey(const String& key);
        bool onValue(const String& value);
    };

    bool PropertiesEvent::onKeyPart(const String&)
    {
      return false;
    }

    bool PropertiesEvent::onKey(const String& key_)
    {
      key = key_;
      return false;
    }

    bool PropertiesEvent::onValue(const String& value)
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

    std::string mkErrorMessage(const std::string& msg, unsigned lineNo)
    {
      std::ostringstream s;
      s << "parsing properties failed in line " << lineNo << ": " << msg;
      return s.str();
    }
  }

  PropertiesParserError::PropertiesParserError(const std::string& msg, unsigned lineNo)
    : SerializationError(mkErrorMessage(msg, lineNo))
  { }

  Properties::Properties(const std::string& filename)
  {
    PropertiesEvent ev(*this);
    std::ifstream in(filename.c_str());
    if (!in)
      throw PropertiesParserError("could not open properties file \"" + filename + '"');
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

    if (ch == '\n')
      ++lineNo;

    switch (state)
    {
      case state_0:
        if (ch == '#' || ch == '!')
          state = state_comment;
        else if (isKeyChar(ch))
        {
          key = ch;
          keypart = ch;
          state = state_key;
        }
        else if (ch == ' ' || ch == '\t' || ch == '\r' || ch == '\n')
          ;
        else if (ch == '\\')
        {
          key.clear();
          keypart.clear();
          state = state_key_esc;
        }
        else if (!std::isspace(ch.value()) && ch != '\n' && ch != '\r')
          throw PropertiesParserError("format error", lineNo);
        break;

      case state_key:
        if (ch == '.')
        {
          event.onKeyPart(keypart);
          keypart.clear();
          key += ch;
        }
        else if (isKeyChar(ch))
        {
          keypart += ch;
          key += ch;
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
        else if (ch == '\\')
        {
          state = state_key_esc;
        }
        else
          throw PropertiesParserError("parse error in properties while reading key " + Utf8Codec::encode(key), lineNo);
        break;

      case state_key_esc:
        if (ch == 'u')
        {
          unicode = 0;
          unicodeCount = 0;
          state = state_key_unicode;
        }
        else if (ch == 'n')
        {
          keypart += '\n';
          key += '\n';
          state = state_key;
        }
        else if (ch == 'r')
        {
          keypart += '\r';
          key += '\r';
          state = state_key;
        }
        else if (ch == 't')
        {
          keypart += '\t';
          key += '\t';
          state = state_key;
        }
        else
        {
          keypart += ch;
          key += ch;
          state = state_key;
        }
        break;

      case state_key_sp:
        if (ch == '=')
        {
          state = state_value;
        }
        else if (!std::isspace(ch.value()))
          throw PropertiesParserError("parse error while reading key " + Utf8Codec::encode(key), lineNo);
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
          value += ch;
        break;

      case state_value_esc:
        if (ch == 'u')
        {
          unicode = 0;
          unicodeCount = 0;
          state = state_unicode;
        }
        else if (ch == 'n')
        {
          value += '\n';
          state = state_value;
        }
        else if (ch == 'r')
        {
          value += '\r';
          state = state_value;
        }
        else if (ch == 't')
        {
          value += '\t';
          state = state_value;
        }
        else
        {
          value += ch;
          state = state_value;
        }
        break;

      case state_unicode:
      case state_key_unicode:
        if (ch >= '0' && ch <= '9')
        {
          unicode = (unicode << 4) | (ch - '0');
          ++unicodeCount;
        }
        else if (ch >= 'a' && ch <= 'f')
        {
          unicode = (unicode << 4) | (ch - 'a' + 10);
          ++unicodeCount;
        }
        else if (ch >= 'A' && ch <= 'F')
        {
          unicode = (unicode << 4) | (ch - 'A' + 10);
          ++unicodeCount;
        }
        else if (unicodeCount == 0)
          throw PropertiesParserError(std::string("invalid unicode sequence \\u") + ch.narrow(), lineNo);
        else
        {
          if (state == state_unicode)
          {
            state = state_value;
            value += Char(unicode);
          }
          else
          {
            state = state_key;
            key += Char(unicode);
          }

          return parse(ch);
        }

        if (unicodeCount >= 8)
        {
          if (state == state_unicode)
          {
            state = state_value;
            value += Char(unicode);
          }
          else
          {
            state = state_key;
            key += Char(unicode);
          }

          return false;
        }

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

      case state_unicode:
        if (unicodeCount == 0)
          throw PropertiesParserError("invalid unicode sequence at end", lineNo);

        value += Char(unicode);
        event.onValue(value);
        value.clear();
        break;

      case state_0:
      case state_comment:
        break;

      case state_key:
      case state_key_esc:
      case state_key_unicode:
      case state_key_sp:
        throw PropertiesParserError("parse error while reading key " + Utf8Codec::encode(key), lineNo);
    }
  }

}
