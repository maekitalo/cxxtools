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
#include <cxxtools/deserializerbase.h>
#include <cxxtools/serializationerror.h>
#include <cxxtools/utf8codec.h>
#include <cxxtools/log.h>
#include <cctype>

log_define("cxxtools.json.parser")

namespace cxxtools
{
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
                SerializationError::doThrow(std::string("invalid character '") + ch.narrow() + "' in string");
            break;

        case state_hex:
            if (ch >= '0' && ch <= '9')
                _value = (_value << 4) | (ch.value() - '0');
            else if (ch >= 'a' && ch <= 'f')
                _value = (_value << 4) | (ch.value() - 'a' + 10);
            else if (ch >= 'A' && ch <= 'F')
                _value = (_value << 4) | (ch.value() - 'A' + 10);
            else
                SerializationError::doThrow(std::string("invalid character '") + ch.narrow() + "' in hex sequence");

            if (--_count == 0)
            {
                _str += Char(_value);
                _state = state_0;
            }

            break;

    }

    return false;
}

JsonParser::JsonParser()
    : _deserializer(0),
      _next(0)
{ }

int JsonParser::advance(Char ch)
{
    int ret;

    switch (_state)
    {
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
                return 1;
            else if (ch == '/')
            {
                _nextState = _state;
                _state = state_comment0;
            }
            else if (!std::isspace(ch.value()))
                SerializationError::doThrow(std::string("invalid character '") + ch.narrow() + '\'');
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
                SerializationError::doThrow(std::string("invalid character '") + ch.narrow() + '\'');
            break;

        case state_object_value:
            ret = _next->advance(ch);

            if (ret != 0)
            {
                log_debug("leave member");
                _deserializer->leaveMember();
                _state = state_object_e;
            }

            if (ret != -1)
                break;

        case state_object_e:
            if (ch == ',')
                _state = state_object_next_member;
            else if (ch == '}')
                return 1;
            else if (ch == '/')
            {
                _nextState = _state;
                _state = state_comment0;
            }
            else if (!std::isspace(ch.value()))
                SerializationError::doThrow(std::string("invalid character '") + ch.narrow() + '\'');
            break;

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
            else if (!std::isspace(ch.value()))
                SerializationError::doThrow(std::string("invalid character '") + ch.narrow() + '\'');
            break;

        case state_array:
            if (ch == ']')
            {
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

        case state_array_value:
            ret = _next->advance(ch);
            if (ret != 0)
                _state = state_array_e;
            if (ret != -1)
                break;

        case state_array_e:
            if (ch == ']')
            {
                log_debug("leave member");
                _deserializer->leaveMember();
                return 1;
            }
            else if (ch == ',')
            {
                log_debug("leave member");
                _deserializer->leaveMember();

                log_debug("begin array member");
                _deserializer->beginMember(std::string(),
                        std::string(), SerializationInfo::Void);
                _next->begin(*_deserializer);
                _state = state_array_value;
            }
            else if (ch == '/')
            {
                _nextState = _state;
                _state = state_comment0;
            }
            else if (!std::isspace(ch.value()))
                SerializationError::doThrow(std::string("invalid character '") + ch.narrow() + '\'');
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
                _deserializer->setValue(_token);
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
                _deserializer->setValue(_token);
                _deserializer->setTypeName("int");
                _token.clear();
                return -1;
            }
            break;

        case state_float:
            if (std::isspace(ch.value()))
            {
                log_debug("set double value \"" << _token << '"');
                _deserializer->setValue(_token);
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
                _deserializer->setValue(_token);
                _deserializer->setTypeName("double");
                _token.clear();
                return -1;
            }
            break;

        case state_token:
            if (std::isalpha(ch.value()))
                _token += Char(std::tolower(ch.value()));
            else
            {
                if (_token == "true" || _token == "false")
                {
                    log_debug("set bool value \"" << _token << '"');
                    _deserializer->setValue(_token);
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

                return -1;
            }

            break;

        case state_comment0:
            if (ch == '/')
                _state = state_commentline;
            else if (ch == '*')
                _state = state_comment;
            else
                SerializationError::doThrow(std::string("invalid character '") + ch.narrow() + '\'');
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
                SerializationError::doThrow(std::string("unexpected character '") + ch.narrow() + "\' after end");
            break;
    }

    return 0;
}

void JsonParser::finish()
{
    if (_state == state_commentline)
        _state = _nextState;

    switch (_state)
    {
        case state_0:
        case state_object:
        case state_object_name:
        case state_object_after_name:
        case state_object_value:
        case state_object_e:
        case state_object_next_member:
        case state_array:
        case state_array_value:
        case state_array_e:
        case state_string:
        case state_comment0:
        case state_commentline:
        case state_comment:
        case state_comment_e:
            SerializationError::doThrow("unexpected end");

        case state_number:
            _deserializer->setValue(_token);
            _deserializer->setTypeName("int");
            _token.clear();
            break;

        case state_float:
            _deserializer->setValue(_token);
            _deserializer->setTypeName("double");
            _token.clear();
            break;

        case state_token:
            if (_token == "true" || _token == "false")
            {
                _deserializer->setValue(_token);
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
