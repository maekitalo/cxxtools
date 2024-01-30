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
#include <cxxtools/serializationerror.h>
#include <cxxtools/log.h>
#include <cxxtools/trim.h>
#include <cxxtools/utf8codec.h>
#include <cctype>
#include <iostream>
#include <stdexcept>

log_define("cxxtools.iniparser")

namespace cxxtools
{
bool IniParser::Event::onSection(const String& section)
{
    return onSection(Utf8Codec::encode(section));
}

bool IniParser::Event::onKey(const String& key)
{
    return onKey(Utf8Codec::encode(key));
}

bool IniParser::Event::onValue(const String& value)
{
    return onValue(Utf8Codec::encode(value));
}

bool IniParser::Event::onComment(const String& /*comment*/)
{
    return false;
}

bool IniParser::Event::onError()
{
    throw std::runtime_error("parse error in ini-file");
    return true;
}

bool IniParser::Event::onKey(const std::string& key)
{
    return false;
}

bool IniParser::Event::onSection(const std::string& section)
{
    return false;
}

bool IniParser::Event::onValue(const std::string& value)
{
    return false;
}

void IniParser::parse(std::istream& in, TextCodec<Char, char>* codec)
{
    _linenumber = 1;
    TextIStream tin(in, codec);
    parse(tin);
}

void IniParser::parse(std::basic_istream<Char>& in)
{
    Char ch;
    int ret;
    _linenumber = 1;
    while (in.get(ch))
    {
        ret = parse(ch);
        if (ret == -1)
            in.putback(ch);
        if (ret != 0)
            break;
    }

    if (in.rdstate() & std::ios::badbit)
        SerializationError::doThrow("parsing ini failed in line " + std::to_string(_linenumber) + " expected: " + expected());

    end();
}

bool IniParser::parse(Char ch)
{
    bool ret = false;
    log_finest("line " << _linenumber << " parse('" << ch << "') state " << static_cast<int>(_state));
    switch (_state)
    {
        case state_0:
            if (ch == L'[')
                _state = state_section;
            else if (std::isalnum(ch))
            {
                _data = ch;
                _state = state_key;
            }
            else if (ch == L'#' || ch == L';')
            {
                _state = state_comment;
            }
            else if (std::isspace(ch))
                ;
            else
            {
                log_debug("onError");
                ret = _event.onError();
            }
            break;

        case state_section:
            if (ch == L']')
            {
                log_debug("onSection(" << _data << ')');
                ret = _event.onSection(_data);
                _data.clear();
                _state = state_0;
            }
            else
                _data += ch;
            break;

        case state_key:
            if (ch == L'=')
            {
                log_debug("onKey(" << _data << ')');
                ret = _event.onKey(_data);
                _data.clear();
                _state = state_value0;
            }
            else if (std::isspace(ch))
            {
                log_debug("onKey(" << _data << ')');
                ret = _event.onKey(_data);
                _data.clear();
                _state = state_key_sp;
            }
            else
                _data += ch;
            break;

        case state_key_sp:
            if (ch == L'=')
                _state = state_value0;
            else if (!std::isspace(ch))
            {
                log_debug("onError");
                ret = _event.onError();
            }
            break;

        case state_value0:
            if (ch == '\n')
            {
                log_debug("onValue(\"\")");
                ret = _event.onValue(String());
                _state = state_0;
            }
            else if (ch == '\'')
            {
                _data.clear();
                _state = state_valuesq;
            }
            else if (ch == '"')
            {
                _data.clear();
                _state = state_valuedq;
            }
            else if (ch == '\\')
                _state = state_valueesc;
            else if (!std::isspace(ch))
            {
                _data = ch;
                _state = state_value;
            }
            break;

        case state_value:
            if (ch == '\n')
            {
                log_debug("onValue(" << _data << ')');
                rtrimi(_data);
                ret = _event.onValue(_data);
                _data.clear();
                _state = state_0;
            }
            else if (ch == '\\')
                _state = state_valueesc;
            else
                _data += ch;
            break;

        case state_valueesc:
            if (ch == 'b')
                _data += '\b';
            else if (ch == 'f')
                _data += '\f';
            else if (ch == 'n')
                _data += '\n';
            else if (ch == 'r')
                _data += '\r';
            else if (ch == 't')
                _data += '\t';
            else
                _data += ch;
            _state = state_value;
            break;

        case state_valuesq:
            if (ch == '\'')
            {
                log_debug("onValue(" << _data << ')');
                ret = _event.onValue(_data);
                _data.clear();
                _state = state_valueqend;
            }
            else if (ch == '\\')
                _state = state_valuesqesc;
            else
                _data += ch;
            break;

        case state_valuesqesc:
            if (ch == 'b')
                _data += '\b';
            else if (ch == 'f')
                _data += '\f';
            else if (ch == 'n')
                _data += '\n';
            else if (ch == 'r')
                _data += '\r';
            else if (ch == 't')
                _data += '\t';
            else
                _data += ch;
            _state = state_valuesq;
            break;

        case state_valuedq:
            if (ch == '"')
            {
                log_debug("onValue(" << _data << ')');
                ret = _event.onValue(_data);
                _data.clear();
                _state = state_valueqend;
            }
            else if (ch == '\\')
                _state = state_valuedqesc;
            else if (ch == '\n')
                ret = _event.onError();
            else
                _data += ch;
            break;

        case state_valuedqesc:
            if (ch == 'b')
                _data += '\b';
            else if (ch == 'f')
                _data += '\f';
            else if (ch == 'n')
                _data += '\n';
            else if (ch == 'r')
                _data += '\r';
            else if (ch == 't')
                _data += '\t';
            else
                _data += ch;
            _state = state_valuedq;
            break;

        case state_valueqend:
            if (ch == '\n')
                _state = state_0;
            else if (ch == L'#' || ch == L';')
                _state = state_comment;
            else if (!std::isspace(ch))
                ret = _event.onError();
            break;

        case state_comment:
            if (ch == '\n')
                _state = state_0;
            break;
    }

    if (ch == L'\n')
        ++_linenumber;

    return ret;
}

void IniParser::end()
{
    switch (_state)
    {
        case state_0:
        case state_valueqend:
        case state_comment:
            break;

        case state_section:
        case state_key:
        case state_key_sp:
        case state_valueesc:
        case state_valuesq:
        case state_valuesqesc:
        case state_valuedq:
        case state_valuedqesc:
            log_debug("onError");
            _event.onError();
            break;

        case state_value0:
            log_debug("onValue(\"\")");
            _event.onValue(String());
            _data.clear();
            break;

        case state_value:
            log_debug("onValue(" << _data << ')');
            _event.onValue(_data);
            _data.clear();
            break;
    }
}

const char* IniParser::expected() const
{
    switch (_state)
    {
        case state_0:           return "section start";
        case state_section:     return "]";
        case state_key:         return "=";
        case state_key_sp:
        case state_value0:
        case state_value:       return "value";
        case state_valueesc:    return "character";
        case state_valuesq:     return "single quote";
        case state_valuesqesc:  return "character";
        case state_valuedq:     return "double quote";
        case state_valuedqesc:  return "character";
        case state_valueqend:
        case state_comment:     return "end of line";
    }
}

}
