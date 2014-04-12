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
#include <cxxtools/convert.h>
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
    _lastLevel = std::numeric_limits<unsigned>::max();
}

void JsonFormatter::finish()
{
    log_trace("finish");
    if (_beautify)
        *_ts << L'\n';
    _level = 0;
    _lastLevel = std::numeric_limits<unsigned>::max();
}

void JsonFormatter::addValueString(const std::string& name, const std::string& type,
                      const String& value)
{
    log_trace("addValueString name=\"" << name << "\", type=\"" << type << "\", value=\"" << value << '"');

    if (type == "bool")
    {
        addValueBool(name, type, convert<bool>(value));
    }
    else
    {
        beginValue(name);

        if (type == "int" || type == "double")
        {
            stringOut(value);
        }
        else if (type == "json")
        {
            *_ts << value;
        }
        else if (type == "null")
        {
            *_ts << L"null";
        }
        else
        {
            *_ts << L'"';
            stringOut(value);
            *_ts << L'"';
        }

        finishValue();
    }
}

void JsonFormatter::addValueStdString(const std::string& name, const std::string& type,
                      const std::string& value)
{
    log_trace("addValueStdString name=\"" << name << "\", type=\"" << type << "\", \" value=\"" << value << '"');

    if (type == "bool")
    {
        addValueBool(name, type, convert<bool>(value));
    }
    else
    {
        beginValue(name);

        if (type == "int" || type == "double")
        {
            stringOut(value);
        }
        else if (type == "json")
        {
            *_ts << String(value);
        }
        else if (type == "null")
        {
            *_ts << L"null";
        }
        else
        {
            *_ts << L'"';
            stringOut(value);
            *_ts << L'"';
        }

        finishValue();
    }
}

void JsonFormatter::addValueBool(const std::string& name, const std::string& type,
                      bool value)
{
    log_trace("addValueBool name=\"" << name << "\", type=\"" << type << "\", \" value=\"" << value << '"');

    beginValue(name);

    *_ts << (value ? L"true" : L"false");

    finishValue();
}

void JsonFormatter::addValueInt(const std::string& name, const std::string& type,
                      int_type value)
{
    log_trace("addValueInt name=\"" << name << "\", type=\"" << type << "\", \" value=" << value);

    beginValue(name);

    if (type == "bool")
        *_ts << (value ? L"true" : L"false");
    else
        *_ts << value;

    finishValue();
}

void JsonFormatter::addValueUnsigned(const std::string& name, const std::string& type,
                      unsigned_type value)
{
    log_trace("addValueUnsigned name=\"" << name << "\", type=\"" << type << "\", \" value=" << value);

    beginValue(name);

    if (type == "bool")
        *_ts << (value ? L"true" : L"false");
    else
        *_ts << value;

    finishValue();
}

void JsonFormatter::addValueFloat(const std::string& name, const std::string& type,
                      long double value)
{
    log_trace("addValueFloat name=\"" << name << "\", type=\"" << type << "\", \" value=" << value);

    beginValue(name);

    if (value != value  // check for nan
        || value == std::numeric_limits<long double>::infinity()
        || value == -std::numeric_limits<long double>::infinity())
    {
        *_ts << L"null";
    }
    else
    {
        *_ts << convert<String>(value);
    }

    finishValue();
}

void JsonFormatter::addNull(const std::string& name, const std::string& type)
{
    beginValue(name);
    *_ts << L"null";
    finishValue();
}

void JsonFormatter::beginArray(const std::string& name, const std::string& type)
{
    checkTs(_ts);

    if (_level == _lastLevel)
    {
        *_ts << L',';
        if (_beautify)
            *_ts << L'\n';
    }
    else
        _lastLevel = _level;

    if (_beautify)
        indent();

    ++_level;

    if (!name.empty())
    {
        *_ts << L'"';
        stringOut(name);
        *_ts << L'"'
             << L':';
        if (_beautify)
            *_ts << L' ';
    }

    *_ts << L'[';
    if (_beautify)
        *_ts << L'\n';
}

void JsonFormatter::finishArray()
{
    checkTs(_ts);

    --_level;
    _lastLevel = _level;
    if (_beautify)
    {
        *_ts << L'\n';
        indent();
    }
    *_ts << L']';
}

void JsonFormatter::beginObject(const std::string& name, const std::string& type)
{
    checkTs(_ts);

    log_trace("beginObject name=\"" << name << '"');

    if (_level == _lastLevel)
    {
        *_ts << L',';
        if (_beautify)
            *_ts << L'\n';
    }
    else
        _lastLevel = _level;

    if (_beautify)
        indent();

    ++_level;

    if (!name.empty())
    {
        *_ts << L'"';
        stringOut(name);
        *_ts << L'"'
             << L':';
        if (_beautify)
            *_ts << L' ';
    }

    *_ts << L'{';
    if (_beautify)
        *_ts << L'\n';
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
        *_ts << L'\n';
        indent();
    }
    *_ts << L'}';
}

void JsonFormatter::indent()
{
    for (unsigned n = 0; n < _level; ++n)
        *_ts << L'\t';
}

void JsonFormatter::stringOut(const std::string& str)
{
    for (std::string::const_iterator it = str.begin(); it != str.end(); ++it)
    {
        if (*it == '"')
            *_ts << L'\\'
                << L'\"';
        else if (*it == '\\')
            *_ts << L'\\'
                << L'\\';
        else if (*it == '\b')
            *_ts << L'\\'
                << L'b';
        else if (*it == '\f')
            *_ts << L'\\'
                << L'f';
        else if (*it == '\n')
            *_ts << L'\\'
                << L'n';
        else if (*it == '\r')
            *_ts << L'\\'
                << L'r';
        else if (*it == '\t')
            *_ts << L'\\'
                << L't';
        else if (static_cast<unsigned char>(*it) >= 0x80 || static_cast<unsigned char>(*it) < 0x20)
        {
            *_ts << L'\\'
                 << L'u';
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
        if (*it == L'"')
            *_ts << L'\\'
                << L'\"';
        else if (*it == L'\\')
            *_ts << L'\\'
                << L'\\';
        else if (*it == L'\b')
            *_ts << L'\\'
                << L'b';
        else if (*it == L'\f')
            *_ts << L'\\'
                << L'f';
        else if (*it == L'\n')
            *_ts << L'\\'
                << L'n';
        else if (*it == L'\r')
            *_ts << L'\\'
                << L'r';
        else if (*it == L'\t')
            *_ts << L'\\'
                << L't';
        else if (it->value() >= 0x80 || it->value() < 0x20)
        {
            *_ts << L'\\'
                 << L'u';
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
        *_ts << L',';
        if (_beautify)
        {
            *_ts << L'\n';
            indent();
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
        *_ts << L'"';
        stringOut(name);
        *_ts << L'"'
             << L':';
        if (_beautify)
            *_ts << L' ';
    }

    ++_level;
}

void JsonFormatter::finishValue()
{
    --_level;
}

}
