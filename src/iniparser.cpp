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
    TextIStream tin(in, codec);
    parse(tin);
}

void IniParser::parse(std::basic_istream<Char>& in)
{
    Char ch;
    int ret;
    while (in.get(ch))
    {
        ret = parse(ch);
        if (ret == -1)
            in.putback(ch);
        if (ret != 0)
            break;
    }

    if (in.rdstate() & std::ios::badbit)
        SerializationError::doThrow("parsing ini file failed");

    end();
}

bool IniParser::parse(Char ch)
{
    bool ret = false;
    switch (state)
    {
        case state_0:
            if (ch == L'[')
                state = state_section;
            else if (std::isalnum(ch))
            {
                data = ch;
                state = state_key;
            }
            else if (ch == L'#' || ch == L';')
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
            if (ch == L']')
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
            if (ch == L'=')
            {
                log_debug("onKey(" << data << ')');
                ret = event.onKey(data);
                data.clear();
                state = state_value0;
            }
            else if (std::isspace(ch))
            {
                log_debug("onKey(" << data << ')');
                ret = event.onKey(data);
                data.clear();
                state = state_key_sp;
            }
            else
                data += ch;
            break;

        case state_key_sp:
            if (ch == L'=')
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
                ret = event.onValue(String());
                state = state_0;
            }
            else if (ch == '\'')
            {
                data.clear();
                state = state_valuesq;
            }
            else if (ch == '"')
            {
                data.clear();
                state = state_valuedq;
            }
            else if (ch == '\\')
                state = state_valueesc;
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
                rtrimi(data);
                ret = event.onValue(data);
                data.clear();
                state = state_0;
            }
            else if (ch == '\\')
                state = state_valueesc;
            else
                data += ch;
            break;

        case state_valueesc:
            if (ch == 'b')
                data += '\b';
            else if (ch == 'f')
                data += '\f';
            else if (ch == 'n')
                data += '\n';
            else if (ch == 'r')
                data += '\r';
            else if (ch == 't')
                data += '\t';
            else
                data += ch;
            state = state_value;
            break;

        case state_valuesq:
            if (ch == '\'')
            {
                log_debug("onValue(" << data << ')');
                ret = event.onValue(data);
                data.clear();
                state = state_valueqend;
            }
            else if (ch == '\\')
                state = state_valuesqesc;
            else
                data += ch;
            break;

        case state_valuesqesc:
            if (ch == 'b')
                data += '\b';
            else if (ch == 'f')
                data += '\f';
            else if (ch == 'n')
                data += '\n';
            else if (ch == 'r')
                data += '\r';
            else if (ch == 't')
                data += '\t';
            else
                data += ch;
            state = state_valuesq;
            break;

        case state_valuedq:
            if (ch == '"')
            {
                log_debug("onValue(" << data << ')');
                ret = event.onValue(data);
                data.clear();
                state = state_valueqend;
            }
            else if (ch == '\\')
                state = state_valuedqesc;
            else
                data += ch;
            break;

        case state_valuedqesc:
            if (ch == 'b')
                data += '\b';
            else if (ch == 'f')
                data += '\f';
            else if (ch == 'n')
                data += '\n';
            else if (ch == 'r')
                data += '\r';
            else if (ch == 't')
                data += '\t';
            else
                data += ch;
            state = state_valuedq;
            break;

        case state_valueqend:
            if (ch == '\n')
                state = state_0;
            else if (ch == L'#' || ch == L';')
                state = state_comment;
            else if (!std::isspace(ch))
                ret = event.onError();
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
            event.onError();
            break;

        case state_value0:
            log_debug("onValue(\"\")");
            event.onValue(String());
            data.clear();
            break;

        case state_value:
            log_debug("onValue(" << data << ')');
            event.onValue(data);
            data.clear();
            break;
    }
}

}
