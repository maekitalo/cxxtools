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

#include <cxxtools/bindeserializer.h>
#include <cxxtools/utf8codec.h>
#include <iostream>

namespace cxxtools
{
BinDeserializer::BinDeserializer(std::istream& in)
    : _in(in)
{ }

BinDeserializer::BinDeserializer(BinDeserializer& s)
    : _in(s._in)
{ }

void BinDeserializer::get(IDeserializer* deser)
{
    SerializationInfo::Category category;
    std::string name;
    std::string id;

    char ch;
    if (!_in.get(ch))
        throw SerializationError(CXXTOOLS_ERROR_MSG("category expected")); 
    category = static_cast<SerializationInfo::Category>(ch);

    read(name);
    deser->setName(name);

    if (category != SerializationInfo::Reference)
    {
        read(_type);
        deser->setTypeName(_type);

        read(id);
        deser->setId(id);
    }

    switch (category)
    {
        case SerializationInfo::Value:
            processValueData(deser);
            break;

        case SerializationInfo::Object:
            processObjectMembers(deser);
            break;

        case SerializationInfo::Array:
            processArrayData(deser);
            break;

        case SerializationInfo::Reference:
            processReference(deser);
            break;

        default:
            throw SerializationError(CXXTOOLS_ERROR_MSG("unknown category")); 
    }
}

void BinDeserializer::read(std::string& str)
{
    char ch;
    while (_in.get(ch) && ch != '\0')
        str += ch;
}

void BinDeserializer::processValueData(IDeserializer* deser)
{
    char ch;

    std::string value;
    while (_in.get(ch) && ch != '\0')
        value += ch;
    deser->setValue(Utf8Codec::decode(value.data(), value.size()));

    if (_in.get(ch) && ch != '\xff')
        throw SerializationError(CXXTOOLS_ERROR_MSG("end of data marker expected")); 
}

void BinDeserializer::processObjectMembers(IDeserializer* deser)
{
    char ch;
    while (_in.get(ch) && ch != '\xff')
    {
        if (ch != '\1')
            throw SerializationError(CXXTOOLS_ERROR_MSG("object member marker expected")); 

        std::string memberName;
        read(memberName);
        deser->beginMember(memberName, std::string(), SerializationInfo::Void);
        BinDeserializer deserializer(*this);
        deserializer.get(deser);
        deser->leaveMember();
    }
}

void BinDeserializer::processArrayData(IDeserializer* deser)
{
    char ch;
    while ((ch = _in.peek(), _in) && ch != '\xff')
    {
        deser->beginMember(std::string(), std::string(), SerializationInfo::Void);
        BinDeserializer deserializer(*this);
        deserializer.get(deser);
        deser->leaveMember();
    }
    _in.get();
}

void BinDeserializer::processReference(IDeserializer* deser)
{
    std::string ref;
    read(ref);
    deser->setReference(ref);
    char ch = '\0';
    if (_in.get(ch) && ch != '\xff')
        throw SerializationError(CXXTOOLS_ERROR_MSG("end of data marker expected")); 
}

}
