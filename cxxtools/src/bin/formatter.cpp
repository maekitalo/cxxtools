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

log_define("cxxtools.bin.formatter")

namespace cxxtools
{
namespace bin
{

namespace
{
    void printTypeCode(std::ostream& out, const std::string& type)
    {
        if (type.empty())
            out << static_cast<char>(Serializer::TypeEmpty);
        else if (type == "bool")
            out << static_cast<char>(Serializer::TypeBool);
        else if (type == "char")
            out << static_cast<char>(Serializer::TypeChar);
        else if (type == "string")
            out << static_cast<char>(Serializer::TypeString);
        else if (type == "int")
            out << static_cast<char>(Serializer::TypeInt);
        else if (type == "double")
            out << static_cast<char>(Serializer::TypeDouble);
        else if (type == "pair")
            out << static_cast<char>(Serializer::TypePair);
        else if (type == "array")
            out << static_cast<char>(Serializer::TypeArray);
        else if (type == "list")
            out << static_cast<char>(Serializer::TypeList);
        else if (type == "deque")
            out << static_cast<char>(Serializer::TypeDeque);
        else if (type == "set")
            out << static_cast<char>(Serializer::TypeSet);
        else if (type == "multiset")
            out << static_cast<char>(Serializer::TypeMultiset);
        else if (type == "map")
            out << static_cast<char>(Serializer::TypeMap);
        else if (type == "multimap")
            out << static_cast<char>(Serializer::TypeMultimap);
        else
            out << static_cast<char>(Serializer::TypeOther) << type << '\0';
    }

    void printInt(std::ostream& out, int64_t v)
    {
        if (v >= std::numeric_limits<int8_t>::min() && v <= std::numeric_limits<int8_t>::max())
        {
            out << static_cast<char>(Serializer::TypeInt8)
                << static_cast<char>(v);
        }
        else if (v >= std::numeric_limits<int16_t>::min() && v <= std::numeric_limits<int16_t>::max())
        {
            out << static_cast<char>(Serializer::TypeInt16)
                << static_cast<char>(v >> 8)
                << static_cast<char>(v);
        }
        else if (v >= std::numeric_limits<int32_t>::min() && v <= std::numeric_limits<int32_t>::max())
        {
            out << static_cast<char>(Serializer::TypeInt32)
                << static_cast<char>(v >> 24)
                << static_cast<char>(v >> 16)
                << static_cast<char>(v >> 8)
                << static_cast<char>(v);
        }
        else
        {
            out << static_cast<char>(Serializer::TypeInt64)
                << static_cast<char>(v >> 56)
                << static_cast<char>(v >> 48)
                << static_cast<char>(v >> 40)
                << static_cast<char>(v >> 32)
                << static_cast<char>(v >> 24)
                << static_cast<char>(v >> 16)
                << static_cast<char>(v >> 8)
                << static_cast<char>(v);
        }
    }

    void printUInt(std::ostream& out, uint64_t v)
    {
        if (v <= std::numeric_limits<uint8_t>::max())
        {
            out << static_cast<char>(Serializer::TypeUInt8)
                << static_cast<char>(v);
        }
        else if (v <= std::numeric_limits<uint16_t>::max())
        {
            out << static_cast<char>(Serializer::TypeUInt16)
                << static_cast<char>(v >> 8)
                << static_cast<char>(v);
        }
        else if (v <= std::numeric_limits<uint32_t>::max())
        {
            out << static_cast<char>(Serializer::TypeUInt32)
                << static_cast<char>(v >> 24)
                << static_cast<char>(v >> 16)
                << static_cast<char>(v >> 8)
                << static_cast<char>(v);
        }
        else
        {
            out << static_cast<char>(Serializer::TypeUInt64)
                << static_cast<char>(v >> 56)
                << static_cast<char>(v >> 48)
                << static_cast<char>(v >> 40)
                << static_cast<char>(v >> 32)
                << static_cast<char>(v >> 24)
                << static_cast<char>(v >> 16)
                << static_cast<char>(v >> 8)
                << static_cast<char>(v);
        }
    }

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
}

void Formatter::finish()
{
}

void Formatter::addValue(const std::string& name, const std::string& type,
                      const cxxtools::String& value, const std::string& id)
{
    *_out << static_cast<char>(SerializationInfo::Value)
          << name << '\0'
          << id << '\0';

    if (type == "int")
    {
        if (value.size() > 0 && (value[0] == L'-' || value[0] == L'+'))
        {
            int64_t v = convert<int64_t>(value);
            printInt(*_out, v);
        }
        else
        {
            uint64_t v = convert<uint64_t>(value);
            printUInt(*_out, v);
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

        *_out << static_cast<char>(Serializer::TypeBcdDouble);

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
    }
    else if (type == "bool")
    {
        *_out << static_cast<char>(Serializer::TypeBool)
              << (isTrue(value) ? '\1' : '\0');
    }
    else
    {
        printTypeCode(*_out, type);
        _ts << value;
        _ts.flush();
        *_out << '\0';
    }

    *_out << '\xff';
}

void Formatter::addValue(const std::string& name, const std::string& type,
                         const std::string& value, const std::string& id)
{
    *_out << static_cast<char>(SerializationInfo::Value)
          << name << '\0'
          << id << '\0';

    if (type == "int")
    {
        if (value.size() > 0 && (value[0] == L'-' || value[0] == L'+'))
        {
            int64_t v = convert<int64_t>(value);
            printInt(*_out, v);
        }
        else
        {
            uint64_t v = convert<uint64_t>(value);
            printUInt(*_out, v);
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

        *_out << static_cast<char>(Serializer::TypeBcdDouble);

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
    }
    else if (type == "bool")
    {
        *_out << static_cast<char>(Serializer::TypeBool)
              << (isTrue(value) ? '\1' : '\0');
    }
    else
    {
        printTypeCode(*_out, type);
        *_out << value << '\0';
    }

    *_out << '\xff';
}

void Formatter::addValue(const std::string& name, const std::string& type,
                         LongInt value, const std::string& id)
{
    *_out << static_cast<char>(SerializationInfo::Value)
          << name << '\0'
          << id << '\0';

    printInt(*_out, value);

    *_out << '\xff';
}

void Formatter::addValue(const std::string& name, const std::string& type,
                         ULongInt value, const std::string& id)
{
    *_out << static_cast<char>(SerializationInfo::Value)
          << name << '\0'
          << id << '\0';

    printInt(*_out, value);

    *_out << '\xff';
}

void Formatter::beginArray(const std::string& name, const std::string& type,
                           const std::string& id)
{
    *_out << static_cast<char>(SerializationInfo::Array)
          << name << '\0'
          << id << '\0';
    printTypeCode(*_out, type);
}

void Formatter::finishArray()
{
    *_out << '\xff';
}

void Formatter::beginObject(const std::string& name, const std::string& type,
                         const std::string& id)
{
    *_out << static_cast<char>(SerializationInfo::Object)
          << name << '\0'
          << id << '\0';
    printTypeCode(*_out, type);
}

void Formatter::beginMember(const std::string& name)
{
    *_out << '\1'
          << name << '\0';
}

void Formatter::finishMember()
{
}

void Formatter::finishObject()
{
    *_out << '\xff';
}

}
}
