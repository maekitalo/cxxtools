/*
 * Copyright (C) 2009 Tommi Maekitalo, Marc Boris Duerner
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


#include <cxxtools/jsonserializer.h>
#include <cxxtools/log.h>
#include <cxxtools/utf8codec.h>

log_define("cxxtools.jsonformatter")

namespace cxxtools
{

namespace
{

void checkTs(std::basic_ostream<Char>* _ts)
{
    if (_ts == 0)
        throw std::logic_error("textstream is not set in JsonFormatter");
}

}

void JsonFormatter::begin(std::basic_ostream<Char>& ts)
{
    _ts = &ts;
    _level = 1;
    _lastLevel = 0;
}

void JsonFormatter::finish()
{
    log_trace("finish");
}

void JsonFormatter::addValue(const std::string& name, const std::string& type,
                      const cxxtools::String& value, const std::string& id)
{
    log_trace("addValue name=\"" << name << "\", type=\"" << type << "\", \" value=\"" << value.narrow() << '"');

    checkTs(_ts);

    if (_level == _lastLevel)
    {
        *_ts << Char(L',');
        if (_beautify)
        {
            if (name.empty())
                *_ts << Char(L' ');
            else
            {
                *_ts << Char(L'\n');
                indent();
            }
        }
    }
    else
    {
        _lastLevel = _level;
        if (_beautify)
            indent();
    }

    if (!name.empty())
    {
        *_ts << Char(L'"');
        stringOut(name);
        *_ts << Char(L'"')
             << Char(L':');
        if (_beautify)
            *_ts << Char(L' ');
    }

    if (type == "int" || type == "double" || type == "bool")
    {
        if (value == L"nan" || value == L"inf" || value == L"-inf")
            *_ts << cxxtools::String(L"null");
        else
            stringOut(value);
    }
    else
    {
        *_ts << Char(L'"');
        stringOut(value);
        *_ts << Char(L'"');
    }
}

void JsonFormatter::beginArray(const std::string& name, const std::string& type,
                               const std::string& id)
{
    checkTs(_ts);

    if (_level == _lastLevel)
    {
        *_ts << Char(L',');
        if (_beautify)
            *_ts << Char(L'\n');
    }
    else
        _lastLevel = _level;

    if (_beautify)
        indent();

    ++_level;

    if (!name.empty())
    {
        *_ts << Char(L'"');
        stringOut(name);
        *_ts << Char(L'"')
             << Char(L':');
        if (_beautify)
            *_ts << Char(L' ');
    }

    *_ts << Char(L'[');
    if (_beautify)
        *_ts << Char(L'\n');
}

void JsonFormatter::finishArray()
{
    checkTs(_ts);

    --_level;
    _lastLevel = _level;
    if (_beautify)
    {
        *_ts << Char(L'\n');
        indent();
    }
    *_ts << Char(L']');
}

void JsonFormatter::beginObject(const std::string& name, const std::string& type,
                                const std::string& id)
{
    checkTs(_ts);

    log_trace("beginObject name=\"" << name << '"');

    if (_level == _lastLevel)
    {
        *_ts << Char(L',');
        if (_beautify)
            *_ts << Char(L'\n');
    }
    else
        _lastLevel = _level;

    if (_beautify)
        indent();

    ++_level;

    if (!name.empty())
    {
        *_ts << Char(L'"');
        stringOut(name);
        *_ts << Char(L'"')
             << Char(L':');
        if (_beautify)
            *_ts << Char(L' ');
    }

    *_ts << Char(L'{');
    if (_beautify)
        *_ts << Char(L'\n');
}

void JsonFormatter::beginMember(const std::string& name)
{
}

void JsonFormatter::finishMember()
{
}

void JsonFormatter::finishObject()
{
    checkTs(_ts);

    log_trace("finishObject");

    --_level;
    _lastLevel = _level;
    if (_beautify)
    {
        *_ts << Char(L'\n');
        indent();
    }
    *_ts << Char(L'}');
}

void JsonFormatter::indent()
{
    for (unsigned n = 0; n < _level; ++n)
        *_ts << Char(L'\t');
}

void JsonFormatter::stringOut(const std::string& str)
{
    for (std::string::const_iterator it = str.begin(); it != str.end(); ++it)
    {
        if (*it == '"')
            *_ts << Char(L'\\')
                << Char(L'\"');
        else if (*it == '\\')
            *_ts << Char(L'\\')
                << Char(L'\\');
        else if (*it == '\b')
            *_ts << Char(L'\\')
                << Char(L'b');
        else if (*it == '\f')
            *_ts << Char(L'\\')
                << Char(L'f');
        else if (*it == '\n')
            *_ts << Char(L'\\')
                << Char(L'n');
        else if (*it == '\r')
            *_ts << Char(L'\\')
                << Char(L'r');
        else if (*it == '\t')
            *_ts << Char(L'\\')
                << Char(L't');
        else
            *_ts << Char(*it);
    }
}

void JsonFormatter::stringOut(const cxxtools::String& str)
{
    for (cxxtools::String::const_iterator it = str.begin(); it != str.end(); ++it)
    {
        if (*it == Char(L'"'))
            *_ts << Char(L'\\')
                << Char(L'\"');
        else if (*it == Char(L'\\'))
            *_ts << Char(L'\\')
                << Char(L'\\');
        else if (*it == Char(L'/'))
            *_ts << Char(L'\\')
                << Char(L'/');
        else if (*it == Char(L'\b'))
            *_ts << Char(L'\\')
                << Char(L'b');
        else if (*it == Char(L'\f'))
            *_ts << Char(L'\\')
                << Char(L'f');
        else if (*it == Char(L'\n'))
            *_ts << Char(L'\\')
                << Char(L'n');
        else if (*it == Char(L'\r'))
            *_ts << Char(L'\\')
                << Char(L'r');
        else if (*it == Char(L'\t'))
            *_ts << Char(L'\\')
                << Char(L't');
        else if (it->value() >= 0x80)
        {
            *_ts << Char(L'\\')
                 << Char(L'u');
            static const char hex[] = "0123456789abcdef";
            uint32_t v = it->value();
            for (uint32_t s = 16; s > 0; s -= 4)
            {
                *_ts << Char(hex[(v >> (s - 4)) & 0xf]);
            }
        }
        else
            *_ts << *it;
    }
}

JsonSerializer::JsonSerializer(std::ostream& os,
    TextCodec<Char, char>* codec)
    : _ts(new TextOStream(os, codec ? codec : new Utf8Codec()))
{
    _formatter.begin(*_ts);
}

JsonSerializer& JsonSerializer::begin(std::ostream& os,
    TextCodec<Char, char>* codec)
{
    delete _ts;
    _ts = new TextOStream(os, codec ? codec : new Utf8Codec());
    _formatter.begin(*_ts);
    return *this;
}


}
