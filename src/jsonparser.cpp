/*
 * Copyright (C) 2011 Tommi Maekitalo
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

#include <cxxtools/jsonparser.h>
#include <cxxtools/jsondeserializer.h>
#include <cxxtools/serializationerror.h>
#include <cxxtools/utf8codec.h>
#include <cxxtools/log.h>

#include <cctype>
#include <sstream>

log_define("cxxtools.json.parser")

namespace cxxtools
{
inline Char fromUtf16(uint32_t lo, uint32_t hi)
{
    return Char((((hi & 0x3ff) << 10) | (lo & 0x3ff)) + 0x10000);
}

const char* JsonParserError::what() const throw()
{
  if (_msg.empty())
  {
      try
      {
          std::ostringstream s;
          s << "parsing json failed in line " << _lineNo << ": " << SerializationError::what();
          _msg = s.str();
      }
      catch (...)
      {
          return SerializationError::what();
      }
  }

  return _msg.c_str();
}

void JsonParser::doThrow(const std::string& msg)
{
    throw JsonParserError(msg, _lineNo);
}

void JsonParser::throwInvalidCharacter(Char ch)
{
  log_debug("invalid character '" << ch << "' in state " << _state);
  doThrow((std::string("invalid character '") + ch.narrow() + '\''));
}

bool JsonParser::JsonStringParser::advance(Char ch)
{
    switch (_state)
    {
        case state_0:
            if (ch == '\\')
                _state = state_esc;
            else if (ch == '"')
                return true;
            else
                _str += ch;
            break;

        case state_esc:
            _state = state_0;
            if (ch == '"' || ch == '\\' || ch == '/')
                _str += ch;
            else if (ch == 'b')
                _str += '\b';
            else if (ch == 'f')
                _str += '\f';
            else if (ch == 'n')
                _str += '\n';
            else if (ch == 'r')
                _str += '\r';
            else if (ch == 't')
                _str += '\t';
            else if (ch == 'u')
            {
                _value = 0;
                _count = 4;
                _state = state_hex;
            }
            else
                _jsonParser->doThrow(std::string("invalid character '") + ch.narrow() + "' in string");
            break;

        case state_hex:
        case state_highsurrogate_hex:
        case state_lowsurrogate_hex:
            if (ch >= '0' && ch <= '9')
                _value = (_value << 4) | (ch.value() - '0');
            else if (ch >= 'a' && ch <= 'f')
                _value = (_value << 4) | (ch.value() - 'a' + 10);
            else if (ch >= 'A' && ch <= 'F')
                _value = (_value << 4) | (ch.value() - 'A' + 10);
            else
                _jsonParser->doThrow(std::string("invalid character '") + ch.narrow() + "' in hex sequence");

            if (--_count == 0)
            {
                switch (_state)
                {
                    case state_hex:
                        // check for surrogate
                        if ((_value & 0xfc00) == 0xd800)
                        {
                            _state = state_lowsurrogate0;
                            _surrogateValue = _value;
                        }
                        else if ((_value & 0xfc00) == 0xdc00)
                        {
                            _state = state_highsurrogate0;
                            _surrogateValue = _value;
                        }
                        else
                        {
                            _str += Char(static_cast<int32_t>(_value));
                            _state = state_0;
                        }
                        break;

                    case state_highsurrogate_hex:
                        if ((_value & 0xfc00) != 0xd800)
                            _jsonParser->doThrow("expecting surrogate value \\ud8xx " + std::to_string(_value));

                        _str += fromUtf16(_surrogateValue, _value);
                        _state = state_0;
                        break;

                    case state_lowsurrogate_hex:
                        if ((_value & 0xfc00) != 0xdc00)
                            _jsonParser->doThrow("expecting surrogate value \\uddxx " + std::to_string(_value));

                        _str += fromUtf16(_value, _surrogateValue);
                        _state = state_0;
                        break;

                    default: // cannot happen - make compiler happy
                        break;
                }
            }

            break;

        case state_highsurrogate0:
        case state_lowsurrogate0:
            if (ch != '\\')
                _jsonParser->doThrow("expecting start of surrogate pair");
            _state = (_state == state_highsurrogate0 ? state_highsurrogate_esc : state_lowsurrogate_esc);
            break;

        case state_highsurrogate_esc:
        case state_lowsurrogate_esc:
            if (ch != 'u')
                _jsonParser->doThrow("expecting unicode mark \\u");

            _value = 0;
            _count = 4;
            _state = (_state == state_highsurrogate_esc ? state_highsurrogate_hex : state_lowsurrogate_hex);
            break;
    }

    return false;
}

JsonParser::JsonParser()
    : _deserializer(0),
      _stringParser(this),
      _next(0),
      _lineNo(1)
{ }

JsonParser::~JsonParser()
{
    delete _next;
}

int JsonParser::advance(Char ch)
{
    int ret;

    if (ch == '\n')
      ++_lineNo;

    try
    {
        switch (_state)
        {
            case state_beforestart:
                if (std::isspace(ch.value()))
                    break;

                _state = state_0;
                // fallthrough

            case state_0:
                if (ch == '{')
                {
                    _state = state_object;
                    _deserializer->setCategory(SerializationInfo::Object);
                }
                else if (ch == '[')
                {
                    _state = state_array;
                    _deserializer->setCategory(SerializationInfo::Array);
                }
                else if (ch == '"')
                {
                    _state = state_string;
                    _deserializer->setCategory(SerializationInfo::Value);
                }
                else if ((ch >= '0' && ch <= '9') || ch == '+' || ch == '-')
                {
                    _token = ch;
                    _state = state_number;
                    _deserializer->setCategory(SerializationInfo::Value);
                }
                else if (ch == '/')
                {
                    _nextState = _state;
                    _state = state_comment0;
                }
                else if (!std::isspace(ch.value()))
                {
                    _token = ch;
                    _state = state_token;
                }
                break;

            case state_object:
                if (ch == '"')
                {
                    _state = state_object_name;
                    _stringParser.clear();
                }
                else if (ch == '}')
                {
                    _state = state_end;
                    return 1;
                }
                else if (ch == '/')
                {
                    _nextState = _state;
                    _state = state_comment0;
                }
                else if (std::isalpha(ch.value()))
                {
                    _token = ch;
                    _state = state_object_plainname;
                }
                else if (!std::isspace(ch.value()))
                    throwInvalidCharacter(ch);
                break;

            case state_object_plainname:
                if (std::isalnum(ch.value()) || ch == 'l')
                    _token += ch;
                else if (std::isspace(ch.value()))
                {
                    _stringParser.str(std::move(_token));
                    _state = state_object_after_name;
                }
                else if (ch == ':')
                {
                    _stringParser.str(std::move(_token));
                    if (_next == 0)
                        _next = new JsonParser();
                    log_debug("begin object member " << _stringParser.str());
                    _deserializer->beginMember(Utf8Codec::encode(_stringParser.str()),
                            std::string(), SerializationInfo::Void);
                    _next->begin(*_deserializer);
                    _stringParser.clear();
                    _state = state_object_value;
                }
                else
                    throwInvalidCharacter(ch);

                break;

            case state_object_name:
                if (_stringParser.advance(ch))
                    _state = state_object_after_name;
                break;

            case state_object_after_name:
                if (ch == ':')
                {
                    if (_next == 0)
                        _next = new JsonParser();
                    log_debug("begin object member " << _stringParser.str());
                    _deserializer->beginMember(Utf8Codec::encode(_stringParser.str()),
                            std::string(), SerializationInfo::Void);
                    _next->begin(*_deserializer);
                    _stringParser.clear();
                    _state = state_object_value;
                }
                else if (ch == '/')
                {
                    _nextState = _state;
                    _state = state_comment0;
                }
                else if (!std::isspace(ch.value()))
                    throwInvalidCharacter(ch);
                break;

            case state_object_value:
                ret = _next->advance(ch);

                if (ret != 0)
                {
                    log_debug("leave object member");
                    _deserializer->leaveMember();
                    _state = state_object_e;
                }

                if (ret != -1)
                    break;

            case state_object_e:
                if (ch == ',')
                    _state = state_object_next_member0;
                else if (ch == '}')
                {
                    _state = state_end;
                    return 1;
                }
                else if (ch == '/')
                {
                    _nextState = _state;
                    _state = state_comment0;
                }
                else if (!std::isspace(ch.value()))
                    throwInvalidCharacter(ch);
                break;

            case state_object_next_member0:
                if (ch == '}')
                {
                    _state = state_end;
                    return 1;
                }
                else if (std::isspace(ch.value()))
                {
                    return 0;
                }

                _state = state_object_next_member;

                // no break

            case state_object_next_member:
                if (ch == '"')
                {
                    _state = state_object_name;
                    _stringParser.clear();
                }
                else if (ch == '/')
                {
                    _nextState = _state;
                    _state = state_comment0;
                }
                else if (std::isalpha(ch.value()))
                {
                    _token = ch;
                    _state = state_object_plainname;
                }
                else if (!std::isspace(ch.value()))
                    throwInvalidCharacter(ch);
                break;

            case state_array:
                if (ch == ']')
                {
                    _state = state_end;
                    return 1;
                }
                else if (ch == '/')
                {
                    _nextState = _state;
                    _state = state_comment0;
                }
                else if (!std::isspace(ch.value()))
                {
                    if (_next == 0)
                        _next = new JsonParser();

                    log_debug("begin array member");
                    _deserializer->beginMember(std::string(),
                            std::string(), SerializationInfo::Void);
                    _next->begin(*_deserializer);
                    _next->advance(ch);
                    _state = state_array_value;
                }
                break;

            case state_array_value0:
                if (ch == ']')
                {
                    _state = state_end;
                    return 1;
                }
                else if (std::isspace(ch.value()))
                {
                    return 0;
                }

                log_debug("begin array member");
                _deserializer->beginMember(std::string(),
                        std::string(), SerializationInfo::Void);
                _next->begin(*_deserializer);
                _state = state_array_value;

                // no break

            case state_array_value:
                ret = _next->advance(ch);
                if (ret != 0)
                    _state = state_array_e;
                if (ret != -1)
                    break;

            case state_array_e:
                if (ch == ']')
                {
                    log_debug("leave array member");
                    _deserializer->leaveMember();
                    _state = state_end;
                    return 1;
                }
                else if (ch == ',')
                {
                    log_debug("leave array member");
                    _deserializer->leaveMember();

                    _state = state_array_value0;
                }
                else if (ch == '/')
                {
                    _nextState = _state;
                    _state = state_comment0;
                }
                else if (!std::isspace(ch.value()))
                    throwInvalidCharacter(ch);
                break;

            case state_string:
                if (_stringParser.advance(ch))
                {
                    log_debug("set string value \"" << _stringParser.str() << '"');
                    _deserializer->setValue(_stringParser.str());
                    _deserializer->setTypeName("string");
                    _stringParser.clear();
                    _state = state_end;
                    return 1;
                }
                break;

            case state_number:
                if (std::isspace(ch.value()))
                {
                    log_debug("set int value \"" << _token << '"');
                    _deserializer->setValue(std::move(_token));
                    _deserializer->setTypeName("int");
                    _token.clear();
                    return 1;
                }
                else if (ch == '.' || ch == 'e' || ch == 'E')
                {
                    _token += ch;
                    _state = state_float;
                }
                else if (ch >= '0' && ch <= '9')
                {
                    _token += ch;
                }
                else
                {
                    log_debug("set int value \"" << _token << '"');
                    _deserializer->setValue(std::move(_token));
                    _deserializer->setTypeName("int");
                    _token.clear();
                    return -1;
                }
                break;

            case state_float:
                if (std::isspace(ch.value()))
                {
                    log_debug("set double value \"" << _token << '"');
                    _deserializer->setValue(std::move(_token));
                    _deserializer->setTypeName("double");
                    _token.clear();
                    return 1;
                }
                else if ((ch >= '0' && ch <= '9') || ch == '+' || ch == '-'
                        || ch == '.' || ch == 'e' || ch == 'E')
                    _token += ch;
                else
                {
                    log_debug("set double value \"" << _token << '"');
                    _deserializer->setValue(std::move(_token));
                    _deserializer->setTypeName("double");
                    _token.clear();
                    return -1;
                }
                break;

            case state_token:
                if (std::isalpha(ch.value()))
                    _token += Char(std::tolower(ch));
                else
                {
                    if (_token == "true" || _token == "false")
                    {
                        log_debug("set bool value \"" << _token << '"');
                        _deserializer->setValue(std::move(_token));
                        _deserializer->setTypeName("bool");
                        _token.clear();
                    }
                    else if (_token == "null")
                    {
                        log_debug("set null value \"" << _token << '"');
                        _deserializer->setTypeName("null");
                        _deserializer->setNull();
                        _token.clear();
                    }
                    else
                        doThrow("unknown token \"" + _token.narrow() + '"');

                    return -1;
                }

                break;

            case state_comment0:
                if (ch == '/')
                    _state = state_commentline;
                else if (ch == '*')
                    _state = state_comment;
                else
                    throwInvalidCharacter(ch);
                break;

            case state_commentline:
                if (ch == '\n')
                    _state = _nextState;
                break;

            case state_comment:
                if (ch == '*')
                    _state = state_comment_e;
                break;

            case state_comment_e:
                if (ch == '/')
                    _state = _nextState;
                else if (ch != '*')
                    _state = state_comment;
                break;

            case state_end:
                if (ch == '/')
                {
                    _nextState = _state;
                    _state = state_comment0;
                }
                else if (!std::isspace(ch.value()))
                    doThrow(std::string("unexpected character '") + ch.narrow() + "\' after end in json parser");
                break;
        }
    }
    catch (JsonParserError& e)
    {
        e._lineNo = _lineNo;
        throw;
    }

    return 0;
}

void JsonParser::finish()
{
    if (_state == state_commentline)
        _state = _nextState;

    switch (_state)
    {
        case state_beforestart:
            log_warn("no json data found");
            throw JsonNoData(_lineNo);

        case state_0:
        case state_object:
        case state_object_plainname:
        case state_object_name:
        case state_object_after_name:
        case state_object_value:
        case state_object_e:
        case state_object_next_member0:
        case state_object_next_member:
        case state_array:
        case state_array_value0:
        case state_array_value:
        case state_array_e:
        case state_string:
        case state_comment0:
        case state_commentline:
        case state_comment:
        case state_comment_e:
            log_warn("unexpected end of json; state=" << _state);
            doThrow("unexpected end of json");

        case state_number:
            _deserializer->setValue(std::move(_token));
            _deserializer->setTypeName("int");
            _token.clear();
            break;

        case state_float:
            _deserializer->setValue(std::move(_token));
            _deserializer->setTypeName("double");
            _token.clear();
            break;

        case state_token:
            if (_token == "true" || _token == "false")
            {
                _deserializer->setValue(std::move(_token));
                _deserializer->setTypeName("bool");
                _token.clear();
            }
            else if (_token == "null")
            {
                _deserializer->setTypeName("null");
                _deserializer->setNull();
                _token.clear();
            }

            break;

        case state_end:
            break;
    }
}

}
