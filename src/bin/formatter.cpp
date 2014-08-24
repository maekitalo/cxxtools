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

#include <cxxtools/bin/formatter.h>
#include <cxxtools/bin/serializer.h>
#include <cxxtools/utf8codec.h>
#include <cxxtools/convert.h>
#include <cxxtools/log.h>
#include <limits>
#include <stdint.h>
#include <math.h>

log_define("cxxtools.bin.formatter")

namespace cxxtools
{
namespace bin
{

namespace
{
    template <typename StringT>
    bool isTrue(const StringT& s)
    {
      return !s.empty()
          && (s[0] == '1'
           || s[0] == 't'
           || s[0] == 'T'
           || s[0] == 'y'
           || s[0] == 'Y');
    }

    template <typename T>
    bool areLowerBitsSet(T v, unsigned bits)
    {
        return v >> bits << bits != v;
    }
}

Formatter::Formatter()
    : _out(0),
      _ts(new Utf8Codec())
{
}

Formatter::Formatter(std::ostream& out)
    : _out(0),
      _ts(new Utf8Codec())
{
    begin(out);
}

void Formatter::begin(std::ostream& out)
{
    _out = &out;
    _ts.attach(out);
    _dictionary.clear();
}

void Formatter::finish()
{
}

void Formatter::addValueString(const std::string& name, const std::string& type,
                      const cxxtools::String& value)
{
    log_trace("addValueString(\"" << name << "\", \"" << type << "\", \"" << value << "\")");

    bool plain = name.empty();

    if (type == "int")
    {
        if (value.size() > 0 && (value[0] == L'-' || value[0] == L'+'))
        {
            int64_t v = convert<int64_t>(value);
            printInt(v, name);
        }
        else
        {
            uint64_t v = convert<uint64_t>(value);
            printUInt(v, name);
        }
    }
    else if (type == "double")
    {
        static const char d[257] = "                " // 00-0f
                                   "                " // 10-1f
                                   "           \xa \xb\xc " // 20-2f
                                   "\x0\x1\x2\x3\x4\x5\x6\x7\x8\x9      " // 30-3f
                                   "                " // 40-4f
                                   "                " // 50-5f
                                   "     \xe          " // 60-6f
                                   "                " // 70-7f
                                   "                " // 80-8f
                                   "                " // 90-9f
                                   "                " // a0-af
                                   "                " // b0-bf
                                   "                " // c0-cf
                                   "                " // d0-df
                                   "                " // e0-ef
                                   "                "; // f0-ff

        *_out << static_cast<char>(plain ? Serializer::TypePlainBcdFloat : Serializer::TypeBcdFloat);

        if (!plain)
            outputString(name);

        if (value == L"nan")
        {
            *_out << '\xf0';
        }
        else if (value == L"inf")
        {
            *_out << '\xf1';
        }
        else if (value == L"-inf")
        {
            *_out << '\xf2';
        }
        else
        {
            bool high = true;
            char ch;
            for (String::const_iterator it = value.begin(); it != value.end(); ++it)
            {
                int v = it->value();
                if (high)
                    ch = d[v] << 4;
                else
                {
                    ch |= d[v];
                    *_out << ch;
                }
                high = !high;
            }

            if (!high)
                *_out << static_cast<char>(ch | '\xd');
        }

        *_out << '\xff';
    }
    else if (type == "bool")
    {
        *_out << static_cast<char>(plain ? Serializer::TypePlainBool : Serializer::TypeBool);

        if (!plain)
            outputString(name);

        *_out << (isTrue(value) ? '\1' : '\0');
    }
    else
    {
        printTypeCode(type, plain);

        if (!plain)
            outputString(name);

        _ts << value;
        _ts.flush();
        *_out << '\0'
              << '\xff';
    }

}

void Formatter::addValueStdString(const std::string& name, const std::string& type, const std::string& value)
{
    log_trace("addValueStdString(\"" << name << "\", \"" << type << "\", \"" << value << "\")");

    bool plain = name.empty();

    if (type == "int")
    {
        if (value.size() > 0 && (value[0] == L'-' || value[0] == L'+'))
        {
            int64_t v = convert<int64_t>(value);
            printInt(v, name);
        }
        else
        {
            uint64_t v = convert<uint64_t>(value);
            printUInt(v, name);
        }
    }
    else if (type == "double")
    {
        static const char d[257] = "                " // 00-0f
                                   "                " // 10-1f
                                   "           \xa \xb\xc " // 20-2f
                                   "\x0\x1\x2\x3\x4\x5\x6\x7\x8\x9      " // 30-3f
                                   "                " // 40-4f
                                   "                " // 50-5f
                                   "     \xe          " // 60-6f
                                   "                " // 70-7f
                                   "                " // 80-8f
                                   "                " // 90-9f
                                   "                " // a0-af
                                   "                " // b0-bf
                                   "                " // c0-cf
                                   "                " // d0-df
                                   "                " // e0-ef
                                   "                "; // f0-ff

        *_out << static_cast<char>(plain ? Serializer::TypePlainBcdFloat : Serializer::TypeBcdFloat);

        if (!plain)
            outputString(name);

        if (value == "nan")
        {
            *_out << '\xf0';
        }
        else if (value == "inf")
        {
            *_out << '\xf1';
        }
        else if (value == "-inf")
        {
            *_out << '\xf2';
        }
        else
        {
            bool high = true;
            char ch;
            for (std::string::const_iterator it = value.begin(); it != value.end(); ++it)
            {
                int v = (*it);
                if (high)
                    ch = d[v] << 4;
                else
                {
                    ch |= d[v];
                    *_out << ch;
                }
                high = !high;
            }

            if (!high)
                *_out << static_cast<char>(ch | '\xd');
        }

        *_out << '\xff';

    }
    else if (type == "bool")
    {
        *_out << static_cast<char>(plain ? Serializer::TypePlainBool : Serializer::TypeBool);

        if (!plain)
            outputString(name);

        *_out << (isTrue(value) ? '\1' : '\0');
    }
    else if (value.find('\0') != std::string::npos)
    {
        uint32_t v = value.size();
        if (v <= 0xffff)
        {
            *_out << static_cast<char>(plain ? Serializer::TypePlainBinary2 : Serializer::TypeBinary2);

            if (!plain)
                outputString(name);
        }
        else
        {
            *_out << static_cast<char>(plain ? Serializer::TypePlainBinary4 : Serializer::TypeBinary4);

            if (!plain)
                outputString(name);

            *_out << static_cast<char>(v >> 24)
                  << static_cast<char>(v >> 16);
        }

        *_out << static_cast<char>(v >> 8)
              << static_cast<char>(v)
              << value;
    }
    else
    {
        printTypeCode(type, plain);

        if (!plain)
            outputString(name);

        *_out << value << '\0'
              << '\xff';
    }

}

void Formatter::addValueBool(const std::string& name, const std::string& type,
                         bool value)
{
    log_trace("addValueBool(\"" << name << "\", \"" << type << "\", " << value << ')');

    bool plain = name.empty();

    *_out << static_cast<char>(plain ? Serializer::TypePlainBool : Serializer::TypeBool);

    if (!plain)
        outputString(name);

    *_out << (value ? '\1' : '\0');
}

void Formatter::addValueInt(const std::string& name, const std::string& type,
                         int_type value)
{
    log_trace("addValueInt(\"" << name << "\", \"" << type << "\", " << value << ')');
    printInt(value, name);
}

void Formatter::addValueUnsigned(const std::string& name, const std::string& type,
                         unsigned_type value)
{
    log_trace("addValueUnsigned(\"" << name << "\", \"" << type << "\", " << value << ')');
    printUInt(value, name);
}

void Formatter::addValueFloat(const std::string& name, const std::string& type,
                      long double value)
{
    log_trace("addValueFloat(\"" << name << "\", \"" << type << "\", " << value << ')');

    if (value != value)
    {
        // NaN
        *_out << static_cast<char>(name.empty() ? Serializer::TypePlainShortFloat : Serializer::TypeShortFloat);
        if (!name.empty())
            outputString(name);
        *_out << '\x7f' << '\x1' << '\0';
    }
    else if (value == std::numeric_limits<long double>::infinity())
    {
        *_out << static_cast<char>(name.empty() ? Serializer::TypePlainShortFloat : Serializer::TypeShortFloat);
        if (!name.empty())
            outputString(name);
        *_out << '\x7f' << '\x0' << '\0';
    }
    else if (value == -std::numeric_limits<long double>::infinity())
    {
        *_out << static_cast<char>(name.empty() ? Serializer::TypePlainShortFloat : Serializer::TypeShortFloat);
        if (!name.empty())
            outputString(name);
        *_out << '\xff' << '\x0' << '\0';
    }
    else if (value == 0.0)
    {
        log_debug("value is zero");
        *_out << static_cast<char>(name.empty() ? Serializer::TypePlainShortFloat : Serializer::TypeShortFloat);
        if (!name.empty())
            outputString(name);
        *_out << '\0' << '\0' << '\0';
    }
    else
    {
        bool isNeg = value < 0;
        int exp;
        long double s = frexp(isNeg ? -value : value, &exp);
        uint64_t m = static_cast<uint64_t>((std::numeric_limits<uint64_t>::max() + 1.0l) * (s * 2.0l - 1.0l));
        if (m < 5 && s > .9)
        {
            // this must be an overflow, which may happen when long double has a very high resolution
            m = std::numeric_limits<uint64_t>::max();
        }

        log_debug("value=" << value << " s=" << s << " man=" << std::hex << m << std::dec << " exp=" << exp << " neg=" << isNeg);

        if (areLowerBitsSet(m, 32) || exp > 63 || exp < -63)
        {
            log_debug("output long float");

            uint16_t e = exp + 16383;
            if (isNeg)
                e |= 0x8000;
            *_out << static_cast<char>(name.empty() ? Serializer::TypePlainLongFloat : Serializer::TypeLongFloat);
            if (!name.empty())
                outputString(name);
            *_out << static_cast<char>(e >> 8)
                  << static_cast<char>(e)
                  << static_cast<char>(m >> 56)
                  << static_cast<char>(m >> 48)
                  << static_cast<char>(m >> 40)
                  << static_cast<char>(m >> 32)
                  << static_cast<char>(m >> 24)
                  << static_cast<char>(m >> 16)
                  << static_cast<char>(m >> 8)
                  << static_cast<char>(m);
        }
        else if (areLowerBitsSet(m, 48))
        {
            log_debug("output medium float");

            uint16_t e = exp + 63;
            if (isNeg)
                e |= 0x80;
            *_out << static_cast<char>(name.empty() ? Serializer::TypePlainMediumFloat : Serializer::TypeMediumFloat);
            if (!name.empty())
                outputString(name);
            *_out << static_cast<char>(e)
                  << static_cast<char>(m >> 56)
                  << static_cast<char>(m >> 48)
                  << static_cast<char>(m >> 40)
                  << static_cast<char>(m >> 32);
        }
        else
        {
            log_debug("output short float");

            uint8_t e = exp + 63;
            if (isNeg)
                e |= 0x80;
            *_out << static_cast<char>(name.empty() ? Serializer::TypePlainShortFloat : Serializer::TypeShortFloat);
            if (!name.empty())
                outputString(name);
            *_out << static_cast<char>(e)
                  << static_cast<char>(m >> 56)
                  << static_cast<char>(m >> 48);
        }
    }
}

void Formatter::addNull(const std::string& name, const std::string& type)
{
    log_trace("addNull(\"" << name << "\", \"" << type << "\")");
    *_out << static_cast<char>(name.empty() ? Serializer::TypePlainEmpty : Serializer::TypeEmpty);

    if (!name.empty())
        outputString(name);

    *_out << '\xff';
}

void Formatter::beginArray(const std::string& name, const std::string& type)
{
    log_trace("beginArray(\"" << name << "\", \"" << type << ')');
    *_out << static_cast<char>(Serializer::CategoryArray);
    outputString(name);
    printTypeCode(type, name.empty());
}

void Formatter::finishArray()
{
    log_trace("finishArray()");
    *_out << '\xff';
}

void Formatter::beginObject(const std::string& name, const std::string& type)
{
    log_trace("beginObject(\"" << name << "\", \"" << type << ')');
    *_out << static_cast<char>(Serializer::CategoryObject);
    outputString(name);
    printTypeCode(type, false);
}

void Formatter::beginMember(const std::string& name)
{
    log_trace("beginMember(\"" << name << ')');
    *_out << '\1';
}

void Formatter::finishMember()
{
    log_trace("finishMember()");
}

void Formatter::finishObject()
{
    log_trace("finishObject()");
    *_out << '\xff';
}

void Formatter::printTypeCode(const std::string& type, bool plain)
{
    if (type.empty())
        *_out << static_cast<char>(plain ? Serializer::TypePlainEmpty : Serializer::TypeEmpty);
    else if (type == "bool")
        *_out << static_cast<char>(plain ? Serializer::TypePlainBool : Serializer::TypeBool);
    else if (type == "char")
        *_out << static_cast<char>(plain ? Serializer::TypePlainChar : Serializer::TypeChar);
    else if (type == "string")
        *_out << static_cast<char>(plain ? Serializer::TypePlainString : Serializer::TypeString);
    else if (type == "int")
        *_out << static_cast<char>(plain ? Serializer::TypePlainInt : Serializer::TypeInt);
    else if (type == "double")
        *_out << static_cast<char>(plain ? Serializer::TypePlainBcdFloat : Serializer::TypeBcdFloat);
    else if (type == "pair")
        *_out << static_cast<char>(plain ? Serializer::TypePlainPair : Serializer::TypePair);
    else if (type == "array")
        *_out << static_cast<char>(plain ? Serializer::TypePlainArray : Serializer::TypeArray);
    else if (type == "list")
        *_out << static_cast<char>(plain ? Serializer::TypePlainList : Serializer::TypeList);
    else if (type == "deque")
        *_out << static_cast<char>(plain ? Serializer::TypePlainDeque : Serializer::TypeDeque);
    else if (type == "set")
        *_out << static_cast<char>(plain ? Serializer::TypePlainSet : Serializer::TypeSet);
    else if (type == "multiset")
        *_out << static_cast<char>(plain ? Serializer::TypePlainMultiset : Serializer::TypeMultiset);
    else if (type == "map")
        *_out << static_cast<char>(plain ? Serializer::TypePlainMap : Serializer::TypeMap);
    else if (type == "multimap")
        *_out << static_cast<char>(plain ? Serializer::TypePlainMultimap : Serializer::TypeMultimap);
    else
    {
        *_out << static_cast<char>(plain ? Serializer::TypePlainOther : Serializer::TypeOther);
        outputString(type);
    }
}

void Formatter::printUInt(uint64_t v, const std::string& name)
{
    if (v <= std::numeric_limits<uint8_t>::max())
    {
        *_out << static_cast<char>(name.empty() ? Serializer::TypePlainUInt8 : Serializer::TypeUInt8);
        if (!name.empty())
            outputString(name);
        *_out << static_cast<char>(v);
    }
    else if (v <= std::numeric_limits<uint16_t>::max())
    {
        *_out << static_cast<char>(name.empty() ? Serializer::TypePlainUInt16 : Serializer::TypeUInt16);
        if (!name.empty())
            outputString(name);
        *_out << static_cast<char>(v >> 8)
            << static_cast<char>(v);
    }
    else if (v <= std::numeric_limits<uint32_t>::max())
    {
        *_out << static_cast<char>(name.empty() ? Serializer::TypePlainUInt32 : Serializer::TypeUInt32);
        if (!name.empty())
            outputString(name);
        *_out << static_cast<char>(v >> 24)
            << static_cast<char>(v >> 16)
            << static_cast<char>(v >> 8)
            << static_cast<char>(v);
    }
    else
    {
        *_out << static_cast<char>(name.empty() ? Serializer::TypePlainUInt64 : Serializer::TypeUInt64);
        if (!name.empty())
            outputString(name);
        *_out << static_cast<char>(v >> 56)
            << static_cast<char>(v >> 48)
            << static_cast<char>(v >> 40)
            << static_cast<char>(v >> 32)
            << static_cast<char>(v >> 24)
            << static_cast<char>(v >> 16)
            << static_cast<char>(v >> 8)
            << static_cast<char>(v);
    }
}

void Formatter::printInt(int64_t v, const std::string& name)
{
    if (v >= 0)
    {
        printUInt(v, name);
    }
    else if (v >= std::numeric_limits<int8_t>::min() && v <= std::numeric_limits<int8_t>::max())
    {
        *_out << static_cast<char>(name.empty() ? Serializer::TypePlainInt8 : Serializer::TypeInt8);
        if (!name.empty())
            outputString(name);
        *_out << static_cast<char>(v);
    }
    else if (v >= std::numeric_limits<int16_t>::min() && v <= std::numeric_limits<int16_t>::max())
    {
        *_out << static_cast<char>(name.empty() ? Serializer::TypePlainInt16 : Serializer::TypeInt16);
        if (!name.empty())
            outputString(name);
        *_out << static_cast<char>(v >> 8)
            << static_cast<char>(v);
    }
    else if (v >= std::numeric_limits<int32_t>::min() && v <= std::numeric_limits<int32_t>::max())
    {
        *_out << static_cast<char>(name.empty() ? Serializer::TypePlainInt32 : Serializer::TypeInt32);
        if (!name.empty())
            outputString(name);
        *_out << static_cast<char>(v >> 24)
            << static_cast<char>(v >> 16)
            << static_cast<char>(v >> 8)
            << static_cast<char>(v);
    }
    else
    {
        *_out << static_cast<char>(name.empty() ? Serializer::TypePlainInt64 : Serializer::TypeInt64);
        if (!name.empty())
            outputString(name);
        *_out << static_cast<char>(v >> 56)
            << static_cast<char>(v >> 48)
            << static_cast<char>(v >> 40)
            << static_cast<char>(v >> 32)
            << static_cast<char>(v >> 24)
            << static_cast<char>(v >> 16)
            << static_cast<char>(v >> 8)
            << static_cast<char>(v);
    }
}

void Formatter::outputString(const std::string& value)
{
    if (value.empty())
    {
        *_out << '\0';
        return;
    }

    unsigned idx;
    for (idx = 0; idx < _dictionary.size(); ++idx)
    {
        if (_dictionary[idx] == value)
        {
            *_out << '\1' << static_cast<char>(idx >> 8) << static_cast<char>(idx);
            return;
        }
    }

    if (_dictionary.size() <= 0xffff)
    {
        log_debug("add dictionary value \"" << value << "\" idx=" << _dictionary.size());
        _dictionary.push_back(value);
    }

    *_out << value
          << '\0';
}

}
}
