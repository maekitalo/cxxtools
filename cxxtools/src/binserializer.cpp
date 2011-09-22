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

#include <cxxtools/binserializer.h>
#include <cxxtools/utf8codec.h>
#include <limits>
#include <stdint.h>

namespace cxxtools
{

namespace
{
  void printTypeCode(std::ostream& out, const std::string& type)
  {
      if (type.empty())
          out << static_cast<char>(BinSerializer::TypeEmpty);
      else if (type == "bool")
          out << static_cast<char>(BinSerializer::TypeBool);
      else if (type == "char")
          out << static_cast<char>(BinSerializer::TypeChar);
      else if (type == "string")
          out << static_cast<char>(BinSerializer::TypeString);
      else if (type == "int")
          out << static_cast<char>(BinSerializer::TypeInt);
      else if (type == "double")
          out << static_cast<char>(BinSerializer::TypeDouble);
      else if (type == "pair")
          out << static_cast<char>(BinSerializer::TypePair);
      else if (type == "array")
          out << static_cast<char>(BinSerializer::TypeArray);
      else if (type == "list")
          out << static_cast<char>(BinSerializer::TypeList);
      else if (type == "deque")
          out << static_cast<char>(BinSerializer::TypeDeque);
      else if (type == "set")
          out << static_cast<char>(BinSerializer::TypeSet);
      else if (type == "multiset")
          out << static_cast<char>(BinSerializer::TypeMultiset);
      else if (type == "map")
          out << static_cast<char>(BinSerializer::TypeMap);
      else if (type == "multimap")
          out << static_cast<char>(BinSerializer::TypeMultimap);
      else
          out << static_cast<char>(BinSerializer::TypeOther) << type << '\0';
  }
}

BinFormatter::BinFormatter()
    : _out(0),
      _ts(new Utf8Codec())
{
}

BinFormatter::BinFormatter(std::ostream& out)
    : _out(0),
      _ts(new Utf8Codec())
{
    begin(out);
}

void BinFormatter::begin(std::ostream& out)
{
    _out = &out;
    _ts.attach(out);
}

void BinFormatter::finish()
{
}

void BinFormatter::addValue(const std::string& name, const std::string& type,
                      const cxxtools::String& value, const std::string& id)
{
    *_out << static_cast<char>(SerializationInfo::Value)
          << name << '\0';

    if (type == "int")
    {
        if (value.size() > 0 && value[0] == L'-' || value[0] == L'+')
        {
            int64_t v = convert<int64_t>(value);
            if (v >= std::numeric_limits<int8_t>::min() && v <= std::numeric_limits<int8_t>::max())
            {
                *_out << static_cast<char>(BinSerializer::TypeInt8)
                      << id << '\0'
                      << static_cast<char>(v);
            }
            else if (v >= std::numeric_limits<int16_t>::min() && v <= std::numeric_limits<int16_t>::max())
            {
                *_out << static_cast<char>(BinSerializer::TypeInt16)
                      << id << '\0'
                      << static_cast<char>(v)
                      << static_cast<char>(v >> 8);
            }
            else if (v >= std::numeric_limits<int32_t>::min() && v <= std::numeric_limits<int32_t>::max())
            {
                *_out << static_cast<char>(BinSerializer::TypeInt32)
                      << id << '\0'
                      << static_cast<char>(v)
                      << static_cast<char>(v >> 8)
                      << static_cast<char>(v >> 16)
                      << static_cast<char>(v >> 24);
            }
            else
            {
                *_out << static_cast<char>(BinSerializer::TypeInt64)
                      << id << '\0'
                      << static_cast<char>(v)
                      << static_cast<char>(v >> 8)
                      << static_cast<char>(v >> 16)
                      << static_cast<char>(v >> 24)
                      << static_cast<char>(v >> 32)
                      << static_cast<char>(v >> 40)
                      << static_cast<char>(v >> 48)
                      << static_cast<char>(v >> 56);
            }
        }
        else
        {
            uint64_t v = convert<uint64_t>(value);
            if (v <= std::numeric_limits<uint8_t>::max())
            {
                *_out << static_cast<char>(BinSerializer::TypeUInt8)
                      << id << '\0'
                      << static_cast<char>(v);
            }
            else if (v <= std::numeric_limits<uint16_t>::max())
            {
                *_out << static_cast<char>(BinSerializer::TypeUInt16)
                      << id << '\0'
                      << static_cast<char>(v)
                      << static_cast<char>(v >> 8);
            }
            else if (v <= std::numeric_limits<uint32_t>::max())
            {
                *_out << static_cast<char>(BinSerializer::TypeUInt32)
                      << id << '\0'
                      << static_cast<char>(v)
                      << static_cast<char>(v >> 8)
                      << static_cast<char>(v >> 16)
                      << static_cast<char>(v >> 24);
            }
            else
            {
                *_out << static_cast<char>(BinSerializer::TypeUInt64)
                      << id << '\0'
                      << static_cast<char>(v)
                      << static_cast<char>(v >> 8)
                      << static_cast<char>(v >> 16)
                      << static_cast<char>(v >> 24)
                      << static_cast<char>(v >> 32)
                      << static_cast<char>(v >> 40)
                      << static_cast<char>(v >> 48)
                      << static_cast<char>(v >> 56);
            }
        }
    }
    else
    {
        printTypeCode(*_out, type);
        *_out << id << '\0';
        _ts << value;
        _ts.flush();
        *_out << '\0';
    }

    *_out << '\xff';
}

void BinFormatter::addReference(const std::string& name, const cxxtools::String& value)
{
    *_out << static_cast<char>(SerializationInfo::Reference)
          << name << '\0';

    _ts << value;
    _ts.flush();
    *_out << '\0' << '\xff';
}

void BinFormatter::beginArray(const std::string& name, const std::string& type,
                        const std::string& id)
{
    *_out << static_cast<char>(SerializationInfo::Array)
          << name << '\0';
    printTypeCode(*_out, type);
    *_out << id << '\0';
}

void BinFormatter::finishArray()
{
    *_out << '\xff';
}

void BinFormatter::beginObject(const std::string& name, const std::string& type,
                         const std::string& id)
{
    *_out << static_cast<char>(SerializationInfo::Object)
          << name << '\0';
    printTypeCode(*_out, type);
    *_out << id << '\0';
}

void BinFormatter::beginMember(const std::string& name)
{
    *_out << '\1'
          << name << '\0';
}

void BinFormatter::finishMember()
{
}

void BinFormatter::finishObject()
{
    *_out << '\xff';
}

}
