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

#include <iostream>
#include <limits>

log_define("cxxtools.json.formatter")

namespace cxxtools
{

namespace
{

    bool isplain(const std::string& str)
    {
        if (str.empty())
            return false;

        if (!std::isalpha(str[0]))
            return false;

        for (std::string::size_type n = 1; n < str.size(); ++n)
            if (!std::isalnum(str[n]))
                return false;

        return true;
    }

}

void JsonFormatter::begin(std::ostream& out)
{
    _os = &out;
    _level = 0;
    _lastLevel = std::numeric_limits<unsigned>::max();
}

void JsonFormatter::finish()
{
    log_trace("finish");
    if (_beautify)
        *_os << '\n';
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
            *_os << value;
        }
        else if (type == "null")
        {
            *_os << "null";
        }
        else
        {
            *_os << '"';
            stringOut(value);
            *_os << '"';
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
            *_os << value;
        }
        else if (type == "null")
        {
            *_os << "null";
        }
        else
        {
            *_os << '"';
            stringOut(value);
            *_os << '"';
        }

        finishValue();
    }
}

void JsonFormatter::addValueBool(const std::string& name, const std::string& type,
                      bool value)
{
    log_trace("addValueBool name=\"" << name << "\", type=\"" << type << "\", \" value=\"" << value << '"');

    beginValue(name);

    *_os << (value ? "true" : "false");

    finishValue();
}

void JsonFormatter::addValueInt(const std::string& name, const std::string& type,
                      int_type value)
{
    log_trace("addValueInt name=\"" << name << "\", type=\"" << type << "\", \" value=" << value);

    beginValue(name);

    if (type == "bool")
        *_os << (value ? "true" : "false");
    else
        putInt(std::ostreambuf_iterator<char>(*_os), value);

    finishValue();
}

void JsonFormatter::addValueUnsigned(const std::string& name, const std::string& type,
                      unsigned_type value)
{
    log_trace("addValueUnsigned name=\"" << name << "\", type=\"" << type << "\", \" value=" << value);

    beginValue(name);

    if (type == "bool")
        *_os << (value ? "true" : "false");
    else
        putInt(std::ostreambuf_iterator<char>(*_os), value);

    finishValue();
}

void JsonFormatter::addValueFloat(const std::string& name, const std::string& type,
                      float value)
{
    log_trace("addValueFloat name=\"" << name << "\", type=\"" << type << "\", \" value=" << value);

    beginValue(name);

    if (value != value  // check for nan
        || value == std::numeric_limits<float>::infinity()
        || value == -std::numeric_limits<float>::infinity())
    {
        *_os << "null";
    }
    else
    {
        putFloat(std::ostreambuf_iterator<char>(*_os), value);
    }

    finishValue();
}

void JsonFormatter::addValueDouble(const std::string& name, const std::string& type,
                      double value)
{
    log_trace("addValueDouble name=\"" << name << "\", type=\"" << type << "\", \" value=" << value);

    beginValue(name);

    if (value != value  // check for nan
        || value == std::numeric_limits<double>::infinity()
        || value == -std::numeric_limits<double>::infinity())
    {
        *_os << "null";
    }
    else
    {
        putFloat(std::ostreambuf_iterator<char>(*_os), value);
    }

    finishValue();
}

void JsonFormatter::addValueLongDouble(const std::string& name, const std::string& type,
                      long double value)
{
    log_trace("addValueLongDouble name=\"" << name << "\", type=\"" << type << "\", \" value=" << value);

    beginValue(name);

    if (value != value  // check for nan
        || value == std::numeric_limits<long double>::infinity()
        || value == -std::numeric_limits<long double>::infinity())
    {
        *_os << "null";
    }
    else
    {
        putFloat(std::ostreambuf_iterator<char>(*_os), value);
    }

    finishValue();
}

void JsonFormatter::addNull(const std::string& name, const std::string& /*type*/)
{
    beginValue(name);
    *_os << "null";
    finishValue();
}

void JsonFormatter::beginArray(const std::string& name, const std::string& /*type*/)
{
    if (_level == _lastLevel)
    {
        *_os << ',';
        if (_beautify)
            *_os << '\n';
    }
    else
        _lastLevel = _level;

    if (_beautify)
        indent();

    ++_level;

    if (!name.empty())
    {
        if (_plainkey && isplain(name))
        {
            *_os << name;
        }
        else
        {
          *_os << '"';
          stringOut(name);
          *_os << '"';
        }

        *_os << ':';

        if (_beautify)
            *_os << ' ';
    }

    *_os << '[';
    if (_beautify)
        *_os << '\n';
}

void JsonFormatter::finishArray()
{
    --_level;
    _lastLevel = _level;
    if (_beautify)
    {
        *_os << '\n';
        indent();
    }
    *_os << ']';
}

void JsonFormatter::beginObject(const std::string& name, const std::string& /*type*/)
{
    log_trace("beginObject name=\"" << name << '"');

    if (_level == _lastLevel)
    {
        *_os << ',';
        if (_beautify)
            *_os << '\n';
    }
    else
        _lastLevel = _level;

    if (_beautify)
        indent();

    ++_level;

    if (!name.empty())
    {
        if (_plainkey && isplain(name))
        {
            *_os << name;
        }
        else
        {
          *_os << '"';
          stringOut(name);
          *_os << '"';
        }

        *_os << ':';

        if (_beautify)
            *_os << ' ';
    }

    *_os << '{';
    if (_beautify)
        *_os << '\n';
}

void JsonFormatter::beginMember(const std::string& /*name*/)
{
}

void JsonFormatter::finishMember()
{
}

void JsonFormatter::finishObject()
{
    log_trace("finishObject");

    --_level;
    _lastLevel = _level;
    if (_beautify)
    {
        *_os << '\n';
        indent();
    }
    *_os << '}';
}

void JsonFormatter::indent()
{
    for (unsigned n = 0; n < _level; ++n)
        *_os << '\t';
}

void JsonFormatter::stringOut(const std::string& str)
{
    for (std::string::const_iterator it = str.begin(); it != str.end(); ++it)
    {
        if (*it == '"')
            *_os << "\\\"";
        else if (*it == '\\')
            *_os << "\\\\";
        else if (*it == '\b')
            *_os << "\\b";
        else if (*it == '\f')
            *_os << "\\f";
        else if (*it == '\n')
            *_os << "\\n";
        else if (*it == '\r')
            *_os << "\\r";
        else if (*it == '\t')
            *_os << "\\t";
        else if ((!_inputUtf8 && static_cast<unsigned char>(*it) >= 0x80) ||
                                 static_cast<unsigned char>(*it) < 0x20)
        {
            *_os << "\\u";
            static const char hex[] = "0123456789abcdef";
            uint32_t v = static_cast<unsigned char>(*it);
            for (uint32_t s = 16; s > 0; s -= 4)
            {
                *_os << hex[(v >> (s - 4)) & 0xf];
            }
        }
        else
            *_os << *it;
    }
}

void JsonFormatter::stringOut(const cxxtools::String& str)
{
    for (cxxtools::String::const_iterator it = str.begin(); it != str.end(); ++it)
    {
        if (*it == L'"')
            *_os << "\\\"";
        else if (*it == L'\\')
            *_os << "\\\\";
        else if (*it == L'\b')
            *_os << "\\b";
        else if (*it == L'\f')
            *_os << "\\f";
        else if (*it == L'\n')
            *_os << "\\n";
        else if (*it == L'\r')
            *_os << "\\r";
        else if (*it == L'\t')
            *_os << "\\t";
        else if (it->value() >= 0x80 || it->value() < 0x20)
        {
            *_os << "\\u";
            static const char hex[] = "0123456789abcdef";
            uint32_t v = it->value();
            for (uint32_t s = 16; s > 0; s -= 4)
            {
                *_os << (hex[(v >> (s - 4)) & 0xf]);
            }
        }
        else
            *_os << *it;
    }
}

void JsonFormatter::beginValue(const std::string& name)
{
    if (_level == _lastLevel)
    {
        *_os << ',';
        if (_beautify)
        {
            *_os << '\n';
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
        if (_plainkey && isplain(name))
        {
            *_os << name;
        }
        else
        {
            *_os << '"';
            stringOut(name);
            *_os << '"';
        }

        *_os << ':';
        if (_beautify)
            *_os << ' ';
    }

    ++_level;
}

void JsonFormatter::finishValue()
{
    --_level;
}

}
