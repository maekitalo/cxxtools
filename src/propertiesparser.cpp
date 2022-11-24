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

#include <cxxtools/propertiesparser.h>
#include <cxxtools/utf8codec.h>
#include <iostream>
#include <sstream>
#include <cctype>

namespace cxxtools
{
  namespace
  {
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

  void PropertiesParser::parse(std::istream& in, TextCodec<Char, char>* codec)
  {
    TextIStream ts(in, codec ? codec : new Utf8Codec());
    parse(ts);
  }

  void PropertiesParser::parse(std::basic_istream<Char>& in)
  {
    Char ch;
    while (in.get(ch) && !advance(ch))
      ;
    end();
  }

  bool PropertiesParser::advance(Char ch)
  {
    bool ret = false;

    if (ch == '\n')
      ++_lineNo;

    switch (_state)
    {
      case state_0:
        if (ch == '#' || ch == '!')
          _state = state_comment;
        else if (ch == ' ' || ch == '\t' || ch == '\r' || ch == '\n')
          ;
        else if (ch == '\\')
        {
          _key.clear();
          _keypart.clear();
          _state = state_key_esc;
        }
        else
        {
          _key = ch;
          _keypart = ch;
          _state = state_key;
        }
        break;

      case state_key:
        if (ch == '.')
        {
          _event.onKeyPart(_keypart);
          _keypart.clear();
          _key += ch;
        }
        else if (ch == '\r' || ch == '\n')
        {
          ret = _event.onKeyPart(_keypart)
             || _event.onKey(_key)
             || _event.onValue(String());
          _keypart.clear();
          _key.clear();
        }
        else if (std::isspace(ch.value()))
        {
          ret = _event.onKeyPart(_keypart)
             || _event.onKey(_key);
          _state = state_key_sp;
        }
        else if (ch == '=' || ch == ':')
        {
          ret = _event.onKeyPart(_keypart)
             || _event.onKey(_key);
          _state = state_value0;
        }
        else if (ch == '\\')
        {
          _state = state_key_esc;
        }
        else
        {
          _keypart += ch;
          _key += ch;
        }
        break;

      case state_key_esc:
        if (ch == 'u')
        {
          _unicode = 0;
          _unicodeCount = 0;
          _state = state_key_unicode;
        }
        else if (ch == 'n')
        {
          _keypart += '\n';
          _key += '\n';
          _state = state_key;
        }
        else if (ch == 'r')
        {
          _keypart += '\r';
          _key += '\r';
          _state = state_key;
        }
        else if (ch == 't')
        {
          _keypart += '\t';
          _key += '\t';
          _state = state_key;
        }
        else
        {
          _keypart += ch;
          _key += ch;
          _state = state_key;
        }
        break;

      case state_key_sp:
        if (ch == '=' || ch == ':')
        {
          _state = state_value0;
        }
        else if (ch == '\r' || ch == '\n')
        {
          ret = _event.onValue(String());
          _keypart.clear();
          _key.clear();
          _state = state_key;
        }
        else if (ch == '\\')
        {
          _value.clear();
          _state = state_value_esc;
        }
        else if (!std::isspace(ch.value()))
        {
          _value = ch;
          _state = state_value;
        }
        break;

      case state_value0:
        if (ch == '\n')
        {
          ret = _event.onValue(std::move(_value));
          _value.clear();
          _state = state_0;
        }
        else if (ch == '\\')
          _state = state_value_esc;
        else if (!std::isspace(ch.value()))
        {
          _value = ch;
          _state = state_value;
        }
        break;

      case state_value:
        if (ch == '\n')
        {
          ret = _event.onValue(std::move(_value));
          _value.clear();
          _state = state_0;
        }
        else if (ch == '\\')
          _state = state_value_esc;
        else if (_trim && std::isspace(ch.value()))
        {
          _space = ch;
          _state = state_value_space;
        }
        else
          _value += ch;
        break;

      case state_value_esc:
        if (ch == 'u')
        {
          _unicode = 0;
          _unicodeCount = 0;
          _state = state_unicode;
        }
        else if (ch == 'n')
        {
          _value += '\n';
          _state = state_value;
        }
        else if (ch == 'r')
        {
          _value += '\r';
          _state = state_value;
        }
        else if (ch == 't')
        {
          _value += '\t';
          _state = state_value;
        }
        else if (ch == '\r' || ch == '\n')
        {
          _state = state_value_cont;
        }
        else
        {
          _value += ch;
          _state = state_value;
        }
        break;

      case state_value_cont:
        if (ch == '\n')
        {
          ret = _event.onValue(std::move(_value));
          _value.clear();
          _state = state_0;
        }
        else if (ch == '\\')
          _state = state_value_esc;
        else if (!std::isspace(ch.value()))
        {
          _value += ch;
          _state = state_value;
        }
        break;

      case state_value_space:
        if (ch == '\n')
        {
          ret = _event.onValue(std::move(_value));
          _value.clear();
          _state = state_0;
        }
        else if (ch == '\\')
        {
          _value += _space;
          _state = state_value_esc;
        }
        else if (std::isspace(ch.value()))
        {
          _space += ch;
        }
        else
        {
          _value += _space;
          _value += ch;
          _state = state_value;
        }
        break;

      case state_unicode:
      case state_key_unicode:
        if (ch >= '0' && ch <= '9')
        {
          _unicode = (_unicode << 4) | (ch - '0');
          ++_unicodeCount;
        }
        else if (ch >= 'a' && ch <= 'f')
        {
          _unicode = (_unicode << 4) | (ch - 'a' + 10);
          ++_unicodeCount;
        }
        else if (ch >= 'A' && ch <= 'F')
        {
          _unicode = (_unicode << 4) | (ch - 'A' + 10);
          ++_unicodeCount;
        }
        else if (_unicodeCount == 0)
          throw PropertiesParserError(std::string("invalid unicode sequence \\u") + ch.narrow() + " in properties", _lineNo);
        else
        {
          if (_state == state_unicode)
          {
            _state = state_value;
            _value += Char(_unicode);
          }
          else
          {
            _state = state_key;
            _key += Char(_unicode);
            _keypart += Char(_unicode);
          }

          return advance(ch);
        }

        if (_unicodeCount >= 8)
        {
          if (_state == state_unicode)
          {
            _state = state_value;
            _value += Char(_unicode);
          }
          else
          {
            _state = state_key;
            _key += Char(_unicode);
          }

          return false;
        }

        break;

      case state_comment:
        if (ch == '\n')
          _state = state_0;
        break;
    }

    return ret;
  }

  void PropertiesParser::end()
  {
    switch (_state)
    {
      case state_value0:
      case state_value:
      case state_value_cont:
      case state_value_esc:
      case state_key_sp:
        _event.onValue(std::move(_value));
        _value.clear();
        break;

      case state_value_space:
        _event.onValue(_value + _space);
        _value.clear();
        break;

      case state_key:
          _event.onKeyPart(_keypart)
             || _event.onKey(_key)
             || _event.onValue(String());
          break;

      case state_unicode:
        if (_unicodeCount == 0)
          throw PropertiesParserError("invalid unicode sequence at end of properties", _lineNo);

        _value += Char(_unicode);
        _event.onValue(std::move(_value));
        _value.clear();
        break;

      case state_0:
      case state_comment:
        break;

      case state_key_esc:
      case state_key_unicode:
        throw PropertiesParserError("parse error while reading key " + Utf8Codec::encode(_key) + " in properties", _lineNo);
    }
  }

}
