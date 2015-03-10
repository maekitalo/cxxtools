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
    std::streambuf* sb = _out->rdbuf();

    if (type == "int")
    {
        if (!value.empty() && (value[0] == L'-' || value[0] == L'+'))
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

        sb->sputc(static_cast<char>(plain ? Serializer::TypePlainBcdFloat : Serializer::TypeBcdFloat));

        if (!plain)
            outputString(name);

        if (value == L"nan")
        {
            sb->sputc('\xf0');
        }
        else if (value == L"inf")
        {
            sb->sputc('\xf1');
        }
        else if (value == L"-inf")
        {
            sb->sputc('\xf2');
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
                    sb->sputc(ch);
                }
                high = !high;
            }

            if (!high)
                sb->sputc(static_cast<char>(ch | '\xd'));
        }

        sb->sputc('\xff');
    }
    else if (type == "bool")
    {
        sb->sputc(static_cast<char>(plain ? Serializer::TypePlainBool : Serializer::TypeBool));

        if (!plain)
            outputString(name);

        sb->sputc((isTrue(value) ? '\1' : '\0'));
    }
    else
    {
        printTypeCode(type, plain);

        if (!plain)
            outputString(name);

        _ts << value;
        _ts.flush();
        sb->sputn("\0\xff", 2);
    }

}

void Formatter::addValueStdString(const std::string& name, const std::string& type, const std::string& value)
{
    log_trace("addValueStdString(\"" << name << "\", \"" << type << "\", \"" << value << "\")");

    bool plain = name.empty();
    std::streambuf* sb = _out->rdbuf();

    if (type == "int")
    {
        if (!value.empty() && (value[0] == L'-' || value[0] == L'+'))
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

        sb->sputc(static_cast<char>(plain ? Serializer::TypePlainBcdFloat : Serializer::TypeBcdFloat));

        if (!plain)
            outputString(name);

        if (value == "nan")
        {
            sb->sputc('\xf0');
        }
        else if (value == "inf")
        {
            sb->sputc('\xf1');
        }
        else if (value == "-inf")
        {
            sb->sputc('\xf2');
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
                    sb->sputc(ch);
                }
                high = !high;
            }

            if (!high)
                sb->sputc(static_cast<char>(ch | '\xd'));
        }

        sb->sputc('\xff');

    }
    else if (type == "bool")
    {
        sb->sputc(static_cast<char>(plain ? Serializer::TypePlainBool : Serializer::TypeBool));

        if (!plain)
            outputString(name);

        sb->sputc((isTrue(value) ? '\1' : '\0'));
    }
    else if (value.find('\0') != std::string::npos)
    {
        uint32_t v = value.size();
        if (v <= 0xffff)
        {
            sb->sputc(static_cast<char>(plain ? Serializer::TypePlainBinary2 : Serializer::TypeBinary2));

            if (!plain)
                outputString(name);
        }
        else
        {
            sb->sputc(static_cast<char>(plain ? Serializer::TypePlainBinary4 : Serializer::TypeBinary4));

            if (!plain)
                outputString(name);

            sb->sputc(static_cast<char>(v >> 24));
            sb->sputc(static_cast<char>(v >> 16));
        }

        sb->sputc(static_cast<char>(v >> 8));
        sb->sputc(static_cast<char>(v));
        *_out << value;
    }
    else
    {
        printTypeCode(type, plain);

        if (!plain)
            outputString(name);

        *_out << value.c_str();
        sb->sputn("\0\xff", 2);
    }

}

void Formatter::addValueBool(const std::string& name, const std::string& type,
                         bool value)
{
    log_trace("addValueBool(\"" << name << "\", \"" << type << "\", " << value << ')');

    bool plain = name.empty();
    std::streambuf* sb = _out->rdbuf();

    sb->sputc(static_cast<char>(plain ? Serializer::TypePlainBool : Serializer::TypeBool));

    if (!plain)
        outputString(name);

    sb->sputc((value ? '\1' : '\0'));
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

    std::streambuf* sb = _out->rdbuf();

    if (value != value)
    {
        // NaN
        sb->sputc(static_cast<char>(name.empty() ? Serializer::TypePlainShortFloat : Serializer::TypeShortFloat));
        if (!name.empty())
            outputString(name);
        sb->sputn("\x7f\x1\0", 3);
    }
    else if (value == std::numeric_limits<long double>::infinity())
    {
        sb->sputc(static_cast<char>(name.empty() ? Serializer::TypePlainShortFloat : Serializer::TypeShortFloat));
        if (!name.empty())
            outputString(name);
        sb->sputn("\x7f\0\0", 3);
    }
    else if (value == -std::numeric_limits<long double>::infinity())
    {
        sb->sputc(static_cast<char>(name.empty() ? Serializer::TypePlainShortFloat : Serializer::TypeShortFloat));
        if (!name.empty())
            outputString(name);
        sb->sputn("\xff\0\0", 3);
    }
    else if (value == 0.0)
    {
        log_debug("value is zero");
        sb->sputc(static_cast<char>(name.empty() ? Serializer::TypePlainShortFloat : Serializer::TypeShortFloat));
        if (!name.empty())
            outputString(name);
        sb->sputn("\0\0\0", 3);
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
            sb->sputc(static_cast<char>(name.empty() ? Serializer::TypePlainLongFloat : Serializer::TypeLongFloat));
            if (!name.empty())
                outputString(name);
            sb->sputc(static_cast<char>(e >> 8));
            sb->sputc(static_cast<char>(e));
            sb->sputc(static_cast<char>(m >> 56));
            sb->sputc(static_cast<char>(m >> 48));
            sb->sputc(static_cast<char>(m >> 40));
            sb->sputc(static_cast<char>(m >> 32));
            sb->sputc(static_cast<char>(m >> 24));
            sb->sputc(static_cast<char>(m >> 16));
            sb->sputc(static_cast<char>(m >> 8));
            sb->sputc(static_cast<char>(m));
        }
        else if (areLowerBitsSet(m, 48))
        {
            log_debug("output medium float");

            uint16_t e = exp + 63;
            if (isNeg)
                e |= 0x80;
            sb->sputc(static_cast<char>(name.empty() ? Serializer::TypePlainMediumFloat : Serializer::TypeMediumFloat));
            if (!name.empty())
                outputString(name);
            sb->sputc(static_cast<char>(e));
            sb->sputc(static_cast<char>(m >> 56));
            sb->sputc(static_cast<char>(m >> 48));
            sb->sputc(static_cast<char>(m >> 40));
            sb->sputc(static_cast<char>(m >> 32));
        }
        else
        {
            log_debug("output short float");

            uint8_t e = exp + 63;
            if (isNeg)
                e |= 0x80;
            sb->sputc(static_cast<char>(name.empty() ? Serializer::TypePlainShortFloat : Serializer::TypeShortFloat));
            if (!name.empty())
                outputString(name);
            sb->sputc(static_cast<char>(e));
            sb->sputc(static_cast<char>(m >> 56));
            sb->sputc(static_cast<char>(m >> 48));
        }
    }
}

void Formatter::addNull(const std::string& name, const std::string& type)
{
    log_trace("addNull(\"" << name << "\", \"" << type << "\")");

    std::streambuf* sb = _out->rdbuf();

    sb->sputc(static_cast<char>(name.empty() ? Serializer::TypePlainEmpty : Serializer::TypeEmpty));

    if (!name.empty())
        outputString(name);

    sb->sputc('\xff');
}

void Formatter::beginArray(const std::string& name, const std::string& type)
{
    log_trace("beginArray(\"" << name << "\", \"" << type << "\")");

    std::streambuf* sb = _out->rdbuf();

    sb->sputc(static_cast<char>(Serializer::CategoryArray));
    outputString(name);
    printTypeCode(type, name.empty());
}

void Formatter::finishArray()
{
    log_trace("finishArray()");

    std::streambuf* sb = _out->rdbuf();

    sb->sputc('\xff');
}

void Formatter::beginObject(const std::string& name, const std::string& type)
{
    log_trace("beginObject(\"" << name << "\", \"" << type << ')');

    std::streambuf* sb = _out->rdbuf();

    sb->sputc(static_cast<char>(Serializer::CategoryObject));
    outputString(name);
    printTypeCode(type, false);
}

void Formatter::beginMember(const std::string& name)
{
    log_trace("beginMember(\"" << name << ')');
}

void Formatter::finishMember()
{
    log_trace("finishMember()");
}

void Formatter::finishObject()
{
    log_trace("finishObject()");

    std::streambuf* sb = _out->rdbuf();

    sb->sputc('\xff');
}

void Formatter::printTypeCode(const std::string& type, bool plain)
{
    std::streambuf* sb = _out->rdbuf();

    if (type.empty())
        sb->sputc(static_cast<char>(plain ? Serializer::TypePlainEmpty : Serializer::TypeEmpty));
    else if (type == "bool")
        sb->sputc(static_cast<char>(plain ? Serializer::TypePlainBool : Serializer::TypeBool));
    else if (type == "char")
        sb->sputc(static_cast<char>(plain ? Serializer::TypePlainChar : Serializer::TypeChar));
    else if (type == "string")
        sb->sputc(static_cast<char>(plain ? Serializer::TypePlainString : Serializer::TypeString));
    else if (type == "int")
        sb->sputc(static_cast<char>(plain ? Serializer::TypePlainInt : Serializer::TypeInt));
    else if (type == "double")
        sb->sputc(static_cast<char>(plain ? Serializer::TypePlainBcdFloat : Serializer::TypeBcdFloat));
    else if (type == "pair")
        sb->sputc(static_cast<char>(plain ? Serializer::TypePlainPair : Serializer::TypePair));
    else if (type == "array")
        sb->sputc(static_cast<char>(plain ? Serializer::TypePlainArray : Serializer::TypeArray));
    else if (type == "list")
        sb->sputc(static_cast<char>(plain ? Serializer::TypePlainList : Serializer::TypeList));
    else if (type == "deque")
        sb->sputc(static_cast<char>(plain ? Serializer::TypePlainDeque : Serializer::TypeDeque));
    else if (type == "set")
        sb->sputc(static_cast<char>(plain ? Serializer::TypePlainSet : Serializer::TypeSet));
    else if (type == "multiset")
        sb->sputc(static_cast<char>(plain ? Serializer::TypePlainMultiset : Serializer::TypeMultiset));
    else if (type == "map")
        sb->sputc(static_cast<char>(plain ? Serializer::TypePlainMap : Serializer::TypeMap));
    else if (type == "multimap")
        sb->sputc(static_cast<char>(plain ? Serializer::TypePlainMultimap : Serializer::TypeMultimap));
    else
    {
        sb->sputc(static_cast<char>(plain ? Serializer::TypePlainOther : Serializer::TypeOther));
        outputString(type);
    }
}

void Formatter::printUInt(uint64_t v, const std::string& name)
{
    std::streambuf* sb = _out->rdbuf();

    if (v <= std::numeric_limits<uint8_t>::max())
    {
        sb->sputc(static_cast<char>(name.empty() ? Serializer::TypePlainUInt8 : Serializer::TypeUInt8));
        if (!name.empty())
            outputString(name);
        sb->sputc(static_cast<char>(v));
    }
    else if (v <= std::numeric_limits<uint16_t>::max())
    {
        sb->sputc(static_cast<char>(name.empty() ? Serializer::TypePlainUInt16 : Serializer::TypeUInt16));
        if (!name.empty())
            outputString(name);
        sb->sputc(static_cast<char>(v >> 8));
        sb->sputc(static_cast<char>(v));
    }
    else if (v <= std::numeric_limits<uint32_t>::max())
    {
        sb->sputc(static_cast<char>(name.empty() ? Serializer::TypePlainUInt32 : Serializer::TypeUInt32));
        if (!name.empty())
            outputString(name);
        sb->sputc(static_cast<char>(v >> 24));
        sb->sputc(static_cast<char>(v >> 16));
        sb->sputc(static_cast<char>(v >> 8));
        sb->sputc(static_cast<char>(v));
    }
    else
    {
        sb->sputc(static_cast<char>(name.empty() ? Serializer::TypePlainUInt64 : Serializer::TypeUInt64));
        if (!name.empty())
            outputString(name);
        sb->sputc(static_cast<char>(v >> 56));
        sb->sputc(static_cast<char>(v >> 48));
        sb->sputc(static_cast<char>(v >> 40));
        sb->sputc(static_cast<char>(v >> 32));
        sb->sputc(static_cast<char>(v >> 24));
        sb->sputc(static_cast<char>(v >> 16));
        sb->sputc(static_cast<char>(v >> 8));
        sb->sputc(static_cast<char>(v));
    }
}

void Formatter::printInt(int64_t v, const std::string& name)
{
    std::streambuf* sb = _out->rdbuf();

    if (v >= 0)
    {
        printUInt(v, name);
    }
    else if (v >= std::numeric_limits<int8_t>::min() && v <= std::numeric_limits<int8_t>::max())
    {
        sb->sputc(static_cast<char>(name.empty() ? Serializer::TypePlainInt8 : Serializer::TypeInt8));
        if (!name.empty())
            outputString(name);
        sb->sputc(static_cast<char>(v));
    }
    else if (v >= std::numeric_limits<int16_t>::min() && v <= std::numeric_limits<int16_t>::max())
    {
        sb->sputc(static_cast<char>(name.empty() ? Serializer::TypePlainInt16 : Serializer::TypeInt16));
        if (!name.empty())
            outputString(name);
        sb->sputc(static_cast<char>(v >> 8));
        sb->sputc(static_cast<char>(v));
    }
    else if (v >= std::numeric_limits<int32_t>::min() && v <= std::numeric_limits<int32_t>::max())
    {
        sb->sputc(static_cast<char>(name.empty() ? Serializer::TypePlainInt32 : Serializer::TypeInt32));
        if (!name.empty())
            outputString(name);
        sb->sputc(static_cast<char>(v >> 24));
        sb->sputc(static_cast<char>(v >> 16));
        sb->sputc(static_cast<char>(v >> 8));
        sb->sputc(static_cast<char>(v));
    }
    else
    {
        sb->sputc(static_cast<char>(name.empty() ? Serializer::TypePlainInt64 : Serializer::TypeInt64));
        if (!name.empty())
            outputString(name);
        sb->sputc(static_cast<char>(v >> 56));
        sb->sputc(static_cast<char>(v >> 48));
        sb->sputc(static_cast<char>(v >> 40));
        sb->sputc(static_cast<char>(v >> 32));
        sb->sputc(static_cast<char>(v >> 24));
        sb->sputc(static_cast<char>(v >> 16));
        sb->sputc(static_cast<char>(v >> 8));
        sb->sputc(static_cast<char>(v));
    }
}

void Formatter::outputString(const std::string& value)
{
    std::streambuf* sb = _out->rdbuf();

    if (value.empty())
    {
        sb->sputc('\0');
        return;
    }

    unsigned idx;
    for (idx = 0; idx < _dictionary.size(); ++idx)
    {
        if (_dictionary[idx] == value)
        {
            sb->sputc('\1');
            sb->sputc(static_cast<char>(idx >> 8));
            sb->sputc(static_cast<char>(idx));
            return;
        }
    }

    if (_dictionary.size() <= 0xffff)
    {
        log_debug("add dictionary value \"" << value << "\" idx=" << _dictionary.size());
        _dictionary.push_back(value);
    }

    *_out << value;
    sb->sputc('\0');
}

}
}
