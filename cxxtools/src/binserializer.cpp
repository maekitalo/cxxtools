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

namespace cxxtools
{
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
          << name << '\0'
          << type << '\0'
          << id << '\0';

    _ts << value;
    _ts.flush();
    *_out << '\0' << '\xff';
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
          << name << '\0'
          << type << '\0'
          << id << '\0';
}

void BinFormatter::finishArray()
{
    *_out << '\xff';
}

void BinFormatter::beginObject(const std::string& name, const std::string& type,
                         const std::string& id)
{
    *_out << static_cast<char>(SerializationInfo::Object)
          << name << '\0'
          << type << '\0'
          << id << '\0';
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
