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


#include <cxxtools/jsonformatter.h>
#include <cxxtools/log.h>
#include <limits>

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
    _level = 0;
    _lastLevel = -1;
}

void JsonFormatter::finish()
{
    log_trace("finish");
    if (_beautify)
        *_ts << Char(L'\n');
}

void JsonFormatter::addValue(const std::string& name, const std::string& type,
                      const String& value, const std::string& id)
{
    log_trace("addValue String name=\"" << name << "\", type=\"" << type << "\", \" value=\"" << value << '"');

    beginValue(name);

    if (type == "null")
    {
        *_ts << String(L"null");
    }
    else
    {
        *_ts << Char(L'"');
        stringOut(value);
        *_ts << Char(L'"');
    }

    finishValue();
}

void JsonFormatter::addValue(const std::string& name, const std::string& type,
                      const std::string& value, const std::string& id)
{
    log_trace("addValue string name=\"" << name << "\", type=\"" << type << "\", \" value=\"" << value << '"');

    beginValue(name);

    *_ts << Char(L'"');
    stringOut(value);
    *_ts << Char(L'"');

    finishValue();
}

void JsonFormatter::addValue(const std::string& name, const std::string& type,
                      int_type value, const std::string& id)
{
    log_trace("addValue int name=\"" << name << "\", type=\"" << type << "\", \" value=\"" << value << '"');

    beginValue(name);

    if (type == "bool")
        *_ts << (value ? String(L"true") : String(L"false"));
    else
        *_ts << value;

    finishValue();
}

void JsonFormatter::addValue(const std::string& name, const std::string& type,
                      unsigned_type value, const std::string& id)
{
    log_trace("addValue unsigned name=\"" << name << "\", type=\"" << type << "\", \" value=\"" << value << '"');

    beginValue(name);

    if (type == "bool")
        *_ts << (value ? String(L"true") : String(L"false"));
    else
        *_ts << value;

    finishValue();
}

void JsonFormatter::addValue(const std::string& name, const std::string& type,
                      long double value, const std::string& id)
{
    log_trace("addValue float name=\"" << name << "\", type=\"" << type << "\", \" value=\"" << value << '"');

    beginValue(name);

    if (value != value  // check for nan
        || value == std::numeric_limits<long double>::infinity()
        || value == -std::numeric_limits<long double>::infinity())
    {
        *_ts << String(L"null");
    }
    else
    {
        *_ts << value;
    }

    finishValue();
}

void JsonFormatter::addNull(const std::string& name, const std::string& type,
                            const std::string& id)
{
    beginValue(name);
    *_ts << String(L"null");
    finishValue();
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
        else if (static_cast<unsigned char>(*it) >= 0x80)
        {
            *_ts << Char(L'\\')
                 << Char(L'u');
            static const char hex[] = "0123456789abcdef";
            uint32_t v = static_cast<unsigned char>(*it);
            for (uint32_t s = 16; s > 0; s -= 4)
            {
                *_ts << Char(hex[(v >> (s - 4)) & 0xf]);
            }
        }
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

void JsonFormatter::beginValue(const std::string& name)
{
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

    ++_level;
}

void JsonFormatter::finishValue()
{
    --_level;
}

}
