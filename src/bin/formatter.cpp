/*
 * Copyright (C) 2011,2024 Tommi Maekitalo
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
#include <cxxtools/decomposer.h>
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

    static const char bcd[257] = "                " // 00-0f
                                 "                " // 10-1f
                                 "\xe          \xa \xb\xc " // 20-2f
                                 "\x0\x1\x2\x3\x4\x5\x6\x7\x8\x9\xd     " // 30-3f
                                 "                " // 40-4f
                                 "                " // 50-5f
                                 "                " // 60-6f
                                 "                " // 70-7f
                                 "                " // 80-8f
                                 "                " // 90-9f
                                 "                " // a0-af
                                 "                " // b0-bf
                                 "                " // c0-cf
                                 "                " // d0-df
                                 "                " // e0-ef
                                 "                "; // f0-ff

    inline bool isAscii(const std::string& s)
    {
        for (std::string::const_iterator it = s.begin(); it != s.end(); ++it)
            if (*it > '\x7f' || *it < '\1')
                return false;
        return true;
    }
}

void Formatter::format(const SerializationInfo& si)
{
    IDecomposer::formatEach(si, *this);
}

void Formatter::begin(std::streambuf& out)
{
    _out = &out;
    _dictionary.clear();
}

void Formatter::finish()
{
    _dictionary.clear();
    _out = 0;
}

void Formatter::addValueString(const std::string& name, const std::string& type,
                      String&& value)
{
    log_trace("addValueString(\"" << name << "\", \"" << type << "\", \"" << value << "\")");

    bool plain = name.empty();

    if (type == "int"
     || type == "long")
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
    else if (value.find_first_not_of(L"0123456789+-.: ") == std::string::npos
        || value == L"nan" || value == L"inf" || value == L"-inf")
    {
        bool isDouble = (type == "double");
        _out->sputc(static_cast<char>(isDouble ? plain ? Serializer::Type::PlainBcdFloat : Serializer::Type::BcdFloat
                                             : plain ? Serializer::Type::PlainBcd : Serializer::Type::Bcd));
        if (!isDouble)
            outputString(type);

        if (!plain)
            outputString(name);

        if (value == L"nan")
        {
            _out->sputc('\xf0');
        }
        else if (value == L"inf")
        {
            _out->sputc('\xf1');
        }
        else if (value == L"-inf")
        {
            _out->sputc('\xf2');
        }
        else
        {
            bool high = true;
            char ch;
            for (String::const_iterator it = value.begin(); it != value.end(); ++it)
            {
                int v = it->value();
                if (high)
                    ch = bcd[v] << 4;
                else
                {
                    ch |= bcd[v];
                    _out->sputc(ch);
                }
                high = !high;
            }

            if (!high)
                _out->sputc(static_cast<char>(ch | '\xf'));
        }

        _out->sputc('\xff');
    }
    else if (type == "bool")
    {
        _out->sputc(static_cast<char>(plain ? Serializer::Type::PlainBool : Serializer::Type::Bool));

        if (!plain)
            outputString(name);

        _out->sputc((isTrue(value) ? '\1' : '\0'));
    }
    else if (value.find(L'\0') != String::npos)
    {
        std::string svalue = Utf8Codec::encode(value);
        uint32_t v = svalue.size();
        if (v <= 0xffff)
        {
            _out->sputc(static_cast<char>(plain ? Serializer::Type::PlainBinary2 : Serializer::Type::Binary2));

            if (!plain)
                outputString(name);
        }
        else
        {
            _out->sputc(static_cast<char>(plain ? Serializer::Type::PlainBinary4 : Serializer::Type::Binary4));

            if (!plain)
                outputString(name);

            _out->sputc(static_cast<char>(v >> 24));
            _out->sputc(static_cast<char>(v >> 16));
        }

        _out->sputc(static_cast<char>(v >> 8));
        _out->sputc(static_cast<char>(v));
        _out->sputn(svalue.data(), svalue.size());
    }
    else
    {
        printTypeCode(type, plain);

        if (!plain)
            outputString(name);

        std::string svalue = Utf8Codec::encode(value);
        _out->sputn(svalue.data(), svalue.size());
        _out->sputn("\0\xff", 2);
    }

}

void Formatter::addValueStdString(const std::string& name, const std::string& type, std::string&& value)
{
    log_trace("addValueStdString(\"" << name << "\", \"" << type << "\", \"" << value << "\")");

    bool plain = name.empty();

    if (type == "int"
     || type == "long")
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
    else if (value.find_first_not_of("0123456789+-.: ") == std::string::npos
        || value == "nan" || value == "inf" || value == "-inf")
    {
        bool isDouble = (type == "double");
        _out->sputc(static_cast<char>(isDouble ? plain ? Serializer::Type::PlainBcdFloat : Serializer::Type::BcdFloat
                                             : plain ? Serializer::Type::PlainBcd : Serializer::Type::Bcd));
        if (!isDouble)
            outputString(type);

        if (!plain)
            outputString(name);

        if (value == "nan")
        {
            _out->sputc('\xf0');
        }
        else if (value == "inf")
        {
            _out->sputc('\xf1');
        }
        else if (value == "-inf")
        {
            _out->sputc('\xf2');
        }
        else
        {
            bool high = true;
            char ch;
            for (std::string::const_iterator it = value.begin(); it != value.end(); ++it)
            {
                int v = (*it);
                if (high)
                    ch = bcd[v] << 4;
                else
                {
                    ch |= bcd[v];
                    _out->sputc(ch);
                }
                high = !high;
            }

            if (!high)
                _out->sputc(static_cast<char>(ch | '\xf'));
        }

        _out->sputc('\xff');

    }
    else if (type == "bool")
    {
        _out->sputc(static_cast<char>(plain ? Serializer::Type::PlainBool : Serializer::Type::Bool));

        if (!plain)
            outputString(name);

        _out->sputc((isTrue(value) ? '\1' : '\0'));
    }
    else if (!isAscii(value))
    {
        uint32_t v = value.size();
        if (v <= 0xffff)
        {
            _out->sputc(static_cast<char>(plain ? Serializer::Type::PlainBinary2 : Serializer::Type::Binary2));

            if (!plain)
                outputString(name);
        }
        else
        {
            _out->sputc(static_cast<char>(plain ? Serializer::Type::PlainBinary4 : Serializer::Type::Binary4));

            if (!plain)
                outputString(name);

            _out->sputc(static_cast<char>(v >> 24));
            _out->sputc(static_cast<char>(v >> 16));
        }

        _out->sputc(static_cast<char>(v >> 8));
        _out->sputc(static_cast<char>(v));
        _out->sputn(value.data(), value.size());
    }
    else
    {
        printTypeCode(type, plain);

        if (!plain)
            outputString(name);

        _out->sputn(value.data(), value.size());
        _out->sputn("\0\xff", 2);
    }

}

void Formatter::addValueChar(const std::string& name, const std::string& type,
                         char value)
{
    log_trace("addValueChar(\"" << name << "\", \"" << type << "\", " << value << ')');

    bool plain = name.empty();

    _out->sputc(static_cast<char>(plain ? Serializer::Type::PlainChar : Serializer::Type::Char));

    if (!plain)
        outputString(name);

    _out->sputc(value);
}

void Formatter::addValueBool(const std::string& name, const std::string& type,
                         bool value)
{
    log_trace("addValueBool(\"" << name << "\", \"" << type << "\", " << value << ')');

    bool plain = name.empty();

    _out->sputc(static_cast<char>(plain ? Serializer::Type::PlainBool : Serializer::Type::Bool));

    if (!plain)
        outputString(name);

    _out->sputc((value ? '\1' : '\0'));
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
                      float value)
{
    log_trace("addValueFloat(\"" << name << "\", \"" << type << "\", " << value << ')');

    if (value != value)
    {
        // NaN
        _out->sputc(static_cast<char>(name.empty() ? Serializer::Type::PlainShortFloat : Serializer::Type::ShortFloat));
        if (!name.empty())
            outputString(name);
        _out->sputn("\x7f\x1\0", 3);
    }
    else if (value == std::numeric_limits<float>::infinity())
    {
        _out->sputc(static_cast<char>(name.empty() ? Serializer::Type::PlainShortFloat : Serializer::Type::ShortFloat));
        if (!name.empty())
            outputString(name);
        _out->sputn("\x7f\0\0", 3);
    }
    else if (value == -std::numeric_limits<float>::infinity())
    {
        _out->sputc(static_cast<char>(name.empty() ? Serializer::Type::PlainShortFloat : Serializer::Type::ShortFloat));
        if (!name.empty())
            outputString(name);
        _out->sputn("\xff\0\0", 3);
    }
    else if (value == 0.0)
    {
        log_debug("value is zero");
        _out->sputc(static_cast<char>(name.empty() ? Serializer::Type::PlainShortFloat : Serializer::Type::ShortFloat));
        if (!name.empty())
            outputString(name);
        _out->sputn("\0\0\0", 3);
    }
    else
    {
        bool isNeg = value < 0;
        int exp;
        float s = frexpf(isNeg ? -value : value, &exp);
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
            _out->sputc(static_cast<char>(name.empty() ? Serializer::Type::PlainLongFloat : Serializer::Type::LongFloat));
            if (!name.empty())
                outputString(name);
            _out->sputc(static_cast<char>(e >> 8));
            _out->sputc(static_cast<char>(e));
            _out->sputc(static_cast<char>(m >> 56));
            _out->sputc(static_cast<char>(m >> 48));
            _out->sputc(static_cast<char>(m >> 40));
            _out->sputc(static_cast<char>(m >> 32));
            _out->sputc(static_cast<char>(m >> 24));
            _out->sputc(static_cast<char>(m >> 16));
            _out->sputc(static_cast<char>(m >> 8));
            _out->sputc(static_cast<char>(m));
        }
        else if (areLowerBitsSet(m, 48))
        {
            log_debug("output medium float");

            uint16_t e = exp + 63;
            if (isNeg)
                e |= 0x80;
            _out->sputc(static_cast<char>(name.empty() ? Serializer::Type::PlainMediumFloat : Serializer::Type::MediumFloat));
            if (!name.empty())
                outputString(name);
            _out->sputc(static_cast<char>(e));
            _out->sputc(static_cast<char>(m >> 56));
            _out->sputc(static_cast<char>(m >> 48));
            _out->sputc(static_cast<char>(m >> 40));
            _out->sputc(static_cast<char>(m >> 32));
        }
        else
        {
            log_debug("output short float");

            uint8_t e = exp + 63;
            if (isNeg)
                e |= 0x80;
            _out->sputc(static_cast<char>(name.empty() ? Serializer::Type::PlainShortFloat : Serializer::Type::ShortFloat));
            if (!name.empty())
                outputString(name);
            _out->sputc(static_cast<char>(e));
            _out->sputc(static_cast<char>(m >> 56));
            _out->sputc(static_cast<char>(m >> 48));
        }
    }
}

void Formatter::addValueDouble(const std::string& name, const std::string& type,
                      double value)
{
    log_trace("addValueDouble(\"" << name << "\", \"" << type << "\", " << value << ')');

    if (value != value)
    {
        // NaN
        _out->sputc(static_cast<char>(name.empty() ? Serializer::Type::PlainShortFloat : Serializer::Type::ShortFloat));
        if (!name.empty())
            outputString(name);
        _out->sputn("\x7f\x1\0", 3);
    }
    else if (value == std::numeric_limits<double>::infinity())
    {
        _out->sputc(static_cast<char>(name.empty() ? Serializer::Type::PlainShortFloat : Serializer::Type::ShortFloat));
        if (!name.empty())
            outputString(name);
        _out->sputn("\x7f\0\0", 3);
    }
    else if (value == -std::numeric_limits<double>::infinity())
    {
        _out->sputc(static_cast<char>(name.empty() ? Serializer::Type::PlainShortFloat : Serializer::Type::ShortFloat));
        if (!name.empty())
            outputString(name);
        _out->sputn("\xff\0\0", 3);
    }
    else if (value == 0.0)
    {
        log_debug("value is zero");
        _out->sputc(static_cast<char>(name.empty() ? Serializer::Type::PlainShortFloat : Serializer::Type::ShortFloat));
        if (!name.empty())
            outputString(name);
        _out->sputn("\0\0\0", 3);
    }
    else
    {
        bool isNeg = value < 0;
        int exp;
        double s = frexp(isNeg ? -value : value, &exp);
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
            _out->sputc(static_cast<char>(name.empty() ? Serializer::Type::PlainLongFloat : Serializer::Type::LongFloat));
            if (!name.empty())
                outputString(name);
            _out->sputc(static_cast<char>(e >> 8));
            _out->sputc(static_cast<char>(e));
            _out->sputc(static_cast<char>(m >> 56));
            _out->sputc(static_cast<char>(m >> 48));
            _out->sputc(static_cast<char>(m >> 40));
            _out->sputc(static_cast<char>(m >> 32));
            _out->sputc(static_cast<char>(m >> 24));
            _out->sputc(static_cast<char>(m >> 16));
            _out->sputc(static_cast<char>(m >> 8));
            _out->sputc(static_cast<char>(m));
        }
        else if (areLowerBitsSet(m, 48))
        {
            log_debug("output medium float");

            uint16_t e = exp + 63;
            if (isNeg)
                e |= 0x80;
            _out->sputc(static_cast<char>(name.empty() ? Serializer::Type::PlainMediumFloat : Serializer::Type::MediumFloat));
            if (!name.empty())
                outputString(name);
            _out->sputc(static_cast<char>(e));
            _out->sputc(static_cast<char>(m >> 56));
            _out->sputc(static_cast<char>(m >> 48));
            _out->sputc(static_cast<char>(m >> 40));
            _out->sputc(static_cast<char>(m >> 32));
        }
        else
        {
            log_debug("output short float");

            uint8_t e = exp + 63;
            if (isNeg)
                e |= 0x80;
            _out->sputc(static_cast<char>(name.empty() ? Serializer::Type::PlainShortFloat : Serializer::Type::ShortFloat));
            if (!name.empty())
                outputString(name);
            _out->sputc(static_cast<char>(e));
            _out->sputc(static_cast<char>(m >> 56));
            _out->sputc(static_cast<char>(m >> 48));
        }
    }
}

void Formatter::addValueLongDouble(const std::string& name, const std::string& type,
                      long double value)
{
    log_trace("addValueLongDouble(\"" << name << "\", \"" << type << "\", " << value << ')');

    if (value != value)
    {
        // NaN
        _out->sputc(static_cast<char>(name.empty() ? Serializer::Type::PlainShortFloat : Serializer::Type::ShortFloat));
        if (!name.empty())
            outputString(name);
        _out->sputn("\x7f\x1\0", 3);
    }
    else if (value == std::numeric_limits<long double>::infinity())
    {
        _out->sputc(static_cast<char>(name.empty() ? Serializer::Type::PlainShortFloat : Serializer::Type::ShortFloat));
        if (!name.empty())
            outputString(name);
        _out->sputn("\x7f\0\0", 3);
    }
    else if (value == -std::numeric_limits<long double>::infinity())
    {
        _out->sputc(static_cast<char>(name.empty() ? Serializer::Type::PlainShortFloat : Serializer::Type::ShortFloat));
        if (!name.empty())
            outputString(name);
        _out->sputn("\xff\0\0", 3);
    }
    else if (value == 0.0)
    {
        log_debug("value is zero");
        _out->sputc(static_cast<char>(name.empty() ? Serializer::Type::PlainShortFloat : Serializer::Type::ShortFloat));
        if (!name.empty())
            outputString(name);
        _out->sputn("\0\0\0", 3);
    }
    else
    {
        bool isNeg = value < 0;
        int exp;
        long double s = frexpl(isNeg ? -value : value, &exp);
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
            _out->sputc(static_cast<char>(name.empty() ? Serializer::Type::PlainLongFloat : Serializer::Type::LongFloat));
            if (!name.empty())
                outputString(name);
            _out->sputc(static_cast<char>(e >> 8));
            _out->sputc(static_cast<char>(e));
            _out->sputc(static_cast<char>(m >> 56));
            _out->sputc(static_cast<char>(m >> 48));
            _out->sputc(static_cast<char>(m >> 40));
            _out->sputc(static_cast<char>(m >> 32));
            _out->sputc(static_cast<char>(m >> 24));
            _out->sputc(static_cast<char>(m >> 16));
            _out->sputc(static_cast<char>(m >> 8));
            _out->sputc(static_cast<char>(m));
        }
        else if (areLowerBitsSet(m, 48))
        {
            log_debug("output medium float");

            uint16_t e = exp + 63;
            if (isNeg)
                e |= 0x80;
            _out->sputc(static_cast<char>(name.empty() ? Serializer::Type::PlainMediumFloat : Serializer::Type::MediumFloat));
            if (!name.empty())
                outputString(name);
            _out->sputc(static_cast<char>(e));
            _out->sputc(static_cast<char>(m >> 56));
            _out->sputc(static_cast<char>(m >> 48));
            _out->sputc(static_cast<char>(m >> 40));
            _out->sputc(static_cast<char>(m >> 32));
        }
        else
        {
            log_debug("output short float");

            uint8_t e = exp + 63;
            if (isNeg)
                e |= 0x80;
            _out->sputc(static_cast<char>(name.empty() ? Serializer::Type::PlainShortFloat : Serializer::Type::ShortFloat));
            if (!name.empty())
                outputString(name);
            _out->sputc(static_cast<char>(e));
            _out->sputc(static_cast<char>(m >> 56));
            _out->sputc(static_cast<char>(m >> 48));
        }
    }
}

void Formatter::addNull(const std::string& name, const std::string& type)
{
    log_trace("addNull(\"" << name << "\", \"" << type << "\")");

    _out->sputc(static_cast<char>(name.empty() ? Serializer::Type::PlainEmpty : Serializer::Type::Empty));

    if (!name.empty())
        outputString(name);

    _out->sputc('\xff');
}

void Formatter::beginArray(const std::string& name, const std::string& type)
{
    log_trace("beginArray(\"" << name << "\", \"" << type << "\")");

    _out->sputc(static_cast<char>(Serializer::Type::CategoryArray));
    outputString(name);
    printTypeCode(type, true);
}

void Formatter::finishArray()
{
    log_trace("finishArray()");

    _out->sputc('\xff');
}

void Formatter::beginObject(const std::string& name, const std::string& type)
{
    log_trace("beginObject(\"" << name << "\", \"" << type << "\")");

    _out->sputc(static_cast<char>(Serializer::Type::CategoryObject));
    outputString(name);
    printTypeCode(type, false);
}

void Formatter::beginMember(const std::string& name)
{
    log_trace("beginMember(\"" << name << "\")");
}

void Formatter::finishMember()
{
    log_trace("finishMember()");
}

void Formatter::finishObject()
{
    log_trace("finishObject()");

    _out->sputc('\xff');
}

void Formatter::printTypeCode(const std::string& type, bool plain)
{
    if (type == "bool")
        _out->sputc(static_cast<char>(plain ? Serializer::Type::PlainBool : Serializer::Type::Bool));
    else if (type == "char")
        _out->sputc(static_cast<char>(plain ? Serializer::Type::PlainChar : Serializer::Type::Char));
    else if (type == "string")
        _out->sputc(static_cast<char>(plain ? Serializer::Type::PlainString : Serializer::Type::String));
    else if (type == "int")
        _out->sputc(static_cast<char>(plain ? Serializer::Type::PlainInt : Serializer::Type::Int));
    else if (type == "double")
        _out->sputc(static_cast<char>(plain ? Serializer::Type::PlainBcdFloat : Serializer::Type::BcdFloat));
    else if (type == "pair")
        _out->sputc(static_cast<char>(plain ? Serializer::Type::PlainPair : Serializer::Type::Pair));
    else if (type == "array")
        _out->sputc(static_cast<char>(plain ? Serializer::Type::PlainArray : Serializer::Type::Array));
    else if (type == "list")
        _out->sputc(static_cast<char>(plain ? Serializer::Type::PlainList : Serializer::Type::List));
    else if (type == "deque")
        _out->sputc(static_cast<char>(plain ? Serializer::Type::PlainDeque : Serializer::Type::Deque));
    else if (type == "set")
        _out->sputc(static_cast<char>(plain ? Serializer::Type::PlainSet : Serializer::Type::Set));
    else if (type == "multiset")
        _out->sputc(static_cast<char>(plain ? Serializer::Type::PlainMultiset : Serializer::Type::Multiset));
    else if (type == "map")
        _out->sputc(static_cast<char>(plain ? Serializer::Type::PlainMap : Serializer::Type::Map));
    else if (type == "multimap")
        _out->sputc(static_cast<char>(plain ? Serializer::Type::PlainMultimap : Serializer::Type::Multimap));
    else
    {
        _out->sputc(static_cast<char>(plain ? Serializer::Type::PlainOther : Serializer::Type::Other));
        outputString(type);
    }
}

void Formatter::printUInt(uint64_t v, const std::string& name)
{
    if (v <= std::numeric_limits<uint8_t>::max())
    {
        _out->sputc(static_cast<char>(name.empty() ? Serializer::Type::PlainUInt8 : Serializer::Type::UInt8));
        if (!name.empty())
            outputString(name);
        _out->sputc(static_cast<char>(v));
    }
    else if (v <= std::numeric_limits<uint16_t>::max())
    {
        _out->sputc(static_cast<char>(name.empty() ? Serializer::Type::PlainUInt16 : Serializer::Type::UInt16));
        if (!name.empty())
            outputString(name);
        _out->sputc(static_cast<char>(v >> 8));
        _out->sputc(static_cast<char>(v));
    }
    else if (v <= std::numeric_limits<uint32_t>::max())
    {
        _out->sputc(static_cast<char>(name.empty() ? Serializer::Type::PlainUInt32 : Serializer::Type::UInt32));
        if (!name.empty())
            outputString(name);
        _out->sputc(static_cast<char>(v >> 24));
        _out->sputc(static_cast<char>(v >> 16));
        _out->sputc(static_cast<char>(v >> 8));
        _out->sputc(static_cast<char>(v));
    }
    else
    {
        _out->sputc(static_cast<char>(name.empty() ? Serializer::Type::PlainUInt64 : Serializer::Type::UInt64));
        if (!name.empty())
            outputString(name);
        _out->sputc(static_cast<char>(v >> 56));
        _out->sputc(static_cast<char>(v >> 48));
        _out->sputc(static_cast<char>(v >> 40));
        _out->sputc(static_cast<char>(v >> 32));
        _out->sputc(static_cast<char>(v >> 24));
        _out->sputc(static_cast<char>(v >> 16));
        _out->sputc(static_cast<char>(v >> 8));
        _out->sputc(static_cast<char>(v));
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
        _out->sputc(static_cast<char>(name.empty() ? Serializer::Type::PlainInt8 : Serializer::Type::Int8));
        if (!name.empty())
            outputString(name);
        _out->sputc(static_cast<char>(v));
    }
    else if (v >= std::numeric_limits<int16_t>::min() && v <= std::numeric_limits<int16_t>::max())
    {
        _out->sputc(static_cast<char>(name.empty() ? Serializer::Type::PlainInt16 : Serializer::Type::Int16));
        if (!name.empty())
            outputString(name);
        _out->sputc(static_cast<char>(v >> 8));
        _out->sputc(static_cast<char>(v));
    }
    else if (v >= std::numeric_limits<int32_t>::min() && v <= std::numeric_limits<int32_t>::max())
    {
        _out->sputc(static_cast<char>(name.empty() ? Serializer::Type::PlainInt32 : Serializer::Type::Int32));
        if (!name.empty())
            outputString(name);
        _out->sputc(static_cast<char>(v >> 24));
        _out->sputc(static_cast<char>(v >> 16));
        _out->sputc(static_cast<char>(v >> 8));
        _out->sputc(static_cast<char>(v));
    }
    else
    {
        _out->sputc(static_cast<char>(name.empty() ? Serializer::Type::PlainInt64 : Serializer::Type::Int64));
        if (!name.empty())
            outputString(name);
        _out->sputc(static_cast<char>(v >> 56));
        _out->sputc(static_cast<char>(v >> 48));
        _out->sputc(static_cast<char>(v >> 40));
        _out->sputc(static_cast<char>(v >> 32));
        _out->sputc(static_cast<char>(v >> 24));
        _out->sputc(static_cast<char>(v >> 16));
        _out->sputc(static_cast<char>(v >> 8));
        _out->sputc(static_cast<char>(v));
    }
}

void Formatter::outputString(const std::string& value)
{
    if (value.empty())
    {
        _out->sputc('\0');
        return;
    }

    unsigned idx;
    for (idx = 0; idx < _dictionary.size(); ++idx)
    {
        if (_dictionary[idx] == value)
        {
            log_debug("use dictionary value \"" << value << "\" idx=" << idx);
            _out->sputc('\1');
            _out->sputc(static_cast<char>(idx >> 8));
            _out->sputc(static_cast<char>(idx));
            return;
        }
    }

    if (_dictionary.size() <= 0xffff)
    {
        log_debug("add dictionary value \"" << value << "\" idx=" << _dictionary.size());
        _dictionary.push_back(value);
    }

    _out->sputn(value.data(), value.size());
    _out->sputc('\0');
}

std::string Formatter::rawString(const std::string& value)
{
    if (value.find_first_not_of("0123456789+-.: ") != std::string::npos)
        return value;

    bool high = true;
    char ch;
    std::string ret;
    for (std::string::const_iterator it = value.begin(); it != value.end(); ++it)
    {
        int v = (*it);
        if (high)
            ch = bcd[v] << 4;
        else
        {
            ch |= bcd[v];
            ret += ch;
        }
        high = !high;
    }

    if (!high)
        ret += static_cast<char>(ch | '\xf');

    return ret;
}

}

}
