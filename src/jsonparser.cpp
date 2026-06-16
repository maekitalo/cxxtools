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
#include <type_traits>

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
  log_debug("invalid character '" << ch << "' in state " << static_cast<std::underlying_type_t<State>>(_state));
  doThrow((std::string("invalid character '") + ch.narrow() + '\''));
}

bool JsonParser::JsonStringParser::advance(Char ch)
{
    switch (_state)
    {
        case State::null:
            if (ch == '\\')
                _state = State::esc;
            else if (ch == '"')
                return true;
            else
                _str += ch;
            break;

        case State::esc:
            _state = State::null;
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
                _state = State::hex;
            }
            else
                _jsonParser->doThrow(std::string("invalid character '") + ch.narrow() + "' in string");
            break;

        case State::hex:
        case State::lowsurrogate_hex:
        case State::highsurrogate_hex:
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
                    case State::hex:
                        // check for surrogate
                        if ((_value & 0xfc00) == 0xd800)
                        {
                            _state = State::highsurrogate0;
                            _surrogateValue = _value;
                        }
                        else if ((_value & 0xfc00) == 0xdc00)
                        {
                            _state = State::lowsurrogate0;
                            _surrogateValue = _value;
                        }
                        else
                        {
                            _str += Char(static_cast<int32_t>(_value));
                            _state = State::null;
                        }
                        break;

                    case State::lowsurrogate_hex:
                        if ((_value & 0xfc00) != 0xd800)
                            _jsonParser->doThrow("expecting surrogate value \\ud8xx " + std::to_string(_value));

                        _str += fromUtf16(_surrogateValue, _value);
                        _state = State::null;
                        break;

                    case State::highsurrogate_hex:
                        if ((_value & 0xfc00) != 0xdc00)
                            _jsonParser->doThrow("expecting surrogate value \\uddxx " + std::to_string(_value));

                        _str += fromUtf16(_value, _surrogateValue);
                        _state = State::null;
                        break;

                    default: // cannot happen - make compiler happy
                        break;
                }
            }

            break;

        case State::lowsurrogate0:
        case State::highsurrogate0:
            if (ch != '\\')
                _jsonParser->doThrow("expecting start of surrogate pair");
            _state = (_state == State::lowsurrogate0 ? State::lowsurrogate_esc : State::highsurrogate_esc);
            break;

        case State::lowsurrogate_esc:
        case State::highsurrogate_esc:
            if (ch != 'u')
                _jsonParser->doThrow("expecting unicode mark \\u");

            _value = 0;
            _count = 4;
            _state = (_state == State::lowsurrogate_esc ? State::lowsurrogate_hex : State::highsurrogate_hex);
            break;
    }

    return false;
}

JsonParser::JsonParser()
    : _deserializer(0),
      _stringParser(this),
      _lineNo(1)
{ }

JsonParser::~JsonParser()
{
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
            case State::beforestart:
                if (std::isspace(ch.value()))
                    break;

                _state = State::null;
                // fallthrough

            case State::null:
                if (ch == '{')
                {
                    _state = State::object;
                    _deserializer->setCategory(SerializationInfo::Object);
                }
                else if (ch == '[')
                {
                    _state = State::array;
                    _deserializer->setCategory(SerializationInfo::Array);
                }
                else if (ch == '"')
                {
                    _state = State::string;
                    _deserializer->setCategory(SerializationInfo::Value);
                }
                else if ((ch >= '0' && ch <= '9') || ch == '+' || ch == '-')
                {
                    _token = ch;
                    _state = State::number;
                    _deserializer->setCategory(SerializationInfo::Value);
                }
                else if (ch == '/')
                {
                    _nextState = _state;
                    _state = State::comment0;
                }
                else if (!std::isspace(ch.value()))
                {
                    _token = ch;
                    _state = State::token;
                }
                break;

            case State::object:
                if (ch == '"')
                {
                    _state = State::object_name;
                    _stringParser.clear();
                }
                else if (ch == '}')
                {
                    _state = State::end;
                    return 1;
                }
                else if (ch == '/')
                {
                    _nextState = _state;
                    _state = State::comment0;
                }
                else if (std::isalpha(ch.value()))
                {
                    _token = ch;
                    _state = State::object_plainname;
                }
                else if (!std::isspace(ch.value()))
                    throwInvalidCharacter(ch);
                break;

            case State::object_plainname:
                if (std::isalnum(ch.value()) || ch == 'l')
                    _token += ch;
                else if (std::isspace(ch.value()))
                {
                    _stringParser.str(std::move(_token));
                    _state = State::object_after_name;
                }
                else if (ch == ':')
                {
                    _stringParser.str(std::move(_token));
                    if (!_next)
                        _next = std::make_unique<JsonParser>();
                    log_debug("begin object member " << _stringParser.str());
                    _deserializer->beginMember(Utf8Codec::encode(_stringParser.str()),
                            std::string(), SerializationInfo::Void);
                    _next->begin(*_deserializer);
                    _stringParser.clear();
                    _state = State::object_value;
                }
                else
                    throwInvalidCharacter(ch);

                break;

            case State::object_name:
                if (_stringParser.advance(ch))
                    _state = State::object_after_name;
                break;

            case State::object_after_name:
                if (ch == ':')
                {
                    if (!_next)
                        _next = std::make_unique<JsonParser>();
                    log_debug("begin object member " << _stringParser.str());
                    _deserializer->beginMember(Utf8Codec::encode(_stringParser.str()),
                            std::string(), SerializationInfo::Void);
                    _next->begin(*_deserializer);
                    _stringParser.clear();
                    _state = State::object_value;
                }
                else if (ch == '/')
                {
                    _nextState = _state;
                    _state = State::comment0;
                }
                else if (!std::isspace(ch.value()))
                    throwInvalidCharacter(ch);
                break;

            case State::object_value:
                ret = _next->advance(ch);

                if (ret != 0)
                {
                    log_debug("leave object member");
                    _deserializer->leaveMember();
                    _state = State::object_e;
                }

                if (ret != -1)
                    break;

            case State::object_e:
                if (ch == ',')
                    _state = State::object_next_member0;
                else if (ch == '}')
                {
                    _state = State::end;
                    return 1;
                }
                else if (ch == '/')
                {
                    _nextState = _state;
                    _state = State::comment0;
                }
                else if (!std::isspace(ch.value()))
                    throwInvalidCharacter(ch);
                break;

            case State::object_next_member0:
                if (ch == '}')
                {
                    _state = State::end;
                    return 1;
                }
                else if (std::isspace(ch.value()))
                {
                    return 0;
                }

                _state = State::object_next_member;

                // no break

            case State::object_next_member:
                if (ch == '"')
                {
                    _state = State::object_name;
                    _stringParser.clear();
                }
                else if (ch == '/')
                {
                    _nextState = _state;
                    _state = State::comment0;
                }
                else if (std::isalpha(ch.value()))
                {
                    _token = ch;
                    _state = State::object_plainname;
                }
                else if (!std::isspace(ch.value()))
                    throwInvalidCharacter(ch);
                break;

            case State::array:
                if (ch == ']')
                {
                    _state = State::end;
                    return 1;
                }
                else if (ch == '/')
                {
                    _nextState = _state;
                    _state = State::comment0;
                }
                else if (!std::isspace(ch.value()))
                {
                    if (!_next)
                        _next = std::make_unique<JsonParser>();

                    log_debug("begin array member");
                    _deserializer->beginMember(std::string(),
                            std::string(), SerializationInfo::Void);
                    _next->begin(*_deserializer);
                    _next->advance(ch);
                    _state = State::array_value;
                }
                break;

            case State::array_value0:
                if (ch == ']')
                {
                    _state = State::end;
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
                _state = State::array_value;

                // no break

            case State::array_value:
                ret = _next->advance(ch);
                if (ret != 0)
                    _state = State::array_e;
                if (ret != -1)
                    break;

            case State::array_e:
                if (ch == ']')
                {
                    log_debug("leave array member");
                    _deserializer->leaveMember();
                    _state = State::end;
                    return 1;
                }
                else if (ch == ',')
                {
                    log_debug("leave array member");
                    _deserializer->leaveMember();

                    _state = State::array_value0;
                }
                else if (ch == '/')
                {
                    _nextState = _state;
                    _state = State::comment0;
                }
                else if (!std::isspace(ch.value()))
                    throwInvalidCharacter(ch);
                break;

            case State::string:
                if (_stringParser.advance(ch))
                {
                    log_debug("set string value \"" << _stringParser.str() << '"');
                    _deserializer->setValue(_stringParser.str());
                    _deserializer->setTypeName("string");
                    _stringParser.clear();
                    _state = State::end;
                    return 1;
                }
                break;

            case State::number:
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
                    _state = State::real;
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

            case State::real:
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

            case State::token:
                if (std::isalpha(ch.value()))
                    _token += Char(ch);
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

            case State::comment0:
                if (ch == '/')
                    _state = State::commentline;
                else if (ch == '*')
                    _state = State::comment;
                else
                    throwInvalidCharacter(ch);
                break;

            case State::commentline:
                if (ch == '\n')
                    _state = _nextState;
                break;

            case State::comment:
                if (ch == '*')
                    _state = State::comment_e;
                break;

            case State::comment_e:
                if (ch == '/')
                    _state = _nextState;
                else if (ch != '*')
                    _state = State::comment;
                break;

            case State::end:
                if (ch == '/')
                {
                    _nextState = _state;
                    _state = State::comment0;
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
    if (_state == State::commentline)
        _state = _nextState;

    switch (_state)
    {
        case State::beforestart:
            log_warn("no json data found");
            throw JsonNoData(_lineNo);

        case State::null:
        case State::object:
        case State::object_plainname:
        case State::object_name:
        case State::object_after_name:
        case State::object_value:
        case State::object_e:
        case State::object_next_member0:
        case State::object_next_member:
        case State::array:
        case State::array_value0:
        case State::array_value:
        case State::array_e:
        case State::string:
        case State::comment0:
        case State::commentline:
        case State::comment:
        case State::comment_e:
            log_warn("unexpected end of json; state=" << static_cast<std::underlying_type_t<State>>(_state));
            doThrow("unexpected end of json");

        case State::number:
            _deserializer->setValue(std::move(_token));
            _deserializer->setTypeName("int");
            _token.clear();
            break;

        case State::real:
            _deserializer->setValue(std::move(_token));
            _deserializer->setTypeName("double");
            _token.clear();
            break;

        case State::token:
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

        case State::end:
            break;
    }
}

}
