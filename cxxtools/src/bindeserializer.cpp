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
#include <cxxtools/binserializer.h>
#include <cxxtools/utf8codec.h>
#include <cxxtools/log.h>
#include <iostream>
#include <sstream>
#include <stdint.h>

log_define("cxxtools.bindeserializer")

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

    TypeCode typeCode = BinSerializer::TypeEmpty;

    if (category != SerializationInfo::Reference)
    {
        read(id);
        typeCode = readType(_type);
        deser->setId(id);
        deser->setTypeName(_type);
    }

    switch (category)
    {
        case SerializationInfo::Value:
            processValueData(deser, typeCode);
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

BinDeserializer::TypeCode BinDeserializer::readType(std::string& str)
{
    TypeCode typeCode = static_cast<TypeCode>(_in.get());
    if (_in)
    {
        switch (typeCode)
        {
            case BinSerializer::TypeEmpty: str.clear(); break;
            case BinSerializer::TypeBool: str = "bool"; break;
            case BinSerializer::TypeChar: str = "char"; break;
            case BinSerializer::TypeString: str = "string"; break;
            case BinSerializer::TypeInt:
            case BinSerializer::TypeInt8:
            case BinSerializer::TypeInt16:
            case BinSerializer::TypeInt32:
            case BinSerializer::TypeInt64:
            case BinSerializer::TypeUInt8:
            case BinSerializer::TypeUInt16:
            case BinSerializer::TypeUInt32:
            case BinSerializer::TypeUInt64: str = "int"; break;
            case BinSerializer::TypeDouble:
            case BinSerializer::TypeBcdDouble: str = "double"; break;
            case BinSerializer::TypePair: str = "pair"; break;
            case BinSerializer::TypeArray: str = "array"; break;
            case BinSerializer::TypeList: str = "list"; break;
            case BinSerializer::TypeDeque: str = "deque"; break;
            case BinSerializer::TypeSet: str = "set"; break;
            case BinSerializer::TypeMultiset: str = "multiset"; break;
            case BinSerializer::TypeMap: str = "map"; break;
            case BinSerializer::TypeMultimap: str = "multimap"; break;
            case BinSerializer::TypeOther: read(str); break;
            default:
            {
                std::ostringstream msg;
                msg << "unknown serialization type code " << typeCode;
                throw SerializationError(msg.str(), CXXTOOLS_SOURCEINFO);
            }
        }
    }

    return typeCode;
}

void BinDeserializer::processValueData(IDeserializer* deser, TypeCode typeCode)
{
    std::string value;
    char ch;

    switch (typeCode)
    {
        case BinSerializer::TypeInt8:
            {
                int8_t ch = static_cast<int8_t>(_in.get());
                value = convert<std::string>(ch);
            }
            break;

        case BinSerializer::TypeUInt8:
            {
                uint8_t ch = static_cast<uint8_t>(_in.get());
                value = convert<std::string>(ch);
            }
            break;

        case BinSerializer::TypeInt16:
            {
                int16_t v;
                uint8_t ch = static_cast<uint8_t>(_in.get());
                v = static_cast<uint16_t>(ch);
                ch = static_cast<uint8_t>(_in.get());
                v |= static_cast<uint16_t>(ch) << 8;
                value = convert<std::string>(v);
            }
            break;

        case BinSerializer::TypeUInt16:
            {
                uint16_t v;
                uint8_t ch = static_cast<uint8_t>(_in.get());
                v = static_cast<uint16_t>(ch);
                ch = static_cast<uint8_t>(_in.get());
                v |= static_cast<uint16_t>(ch) << 8;
                value = convert<std::string>(v);
            }
            break;

        case BinSerializer::TypeInt32:
            {
                int32_t v;
                uint8_t ch = static_cast<uint8_t>(_in.get());
                v = static_cast<uint32_t>(ch);
                ch = static_cast<uint8_t>(_in.get());
                v |= static_cast<uint32_t>(ch) << 8;
                ch = static_cast<uint8_t>(_in.get());
                v |= static_cast<uint32_t>(ch) << 16;
                ch = static_cast<uint8_t>(_in.get());
                v |= static_cast<uint32_t>(ch) << 24;
                value = convert<std::string>(v);
            }
            break;

        case BinSerializer::TypeUInt32:
            {
                uint32_t v;
                uint8_t ch = static_cast<uint8_t>(_in.get());
                v = static_cast<uint32_t>(ch);
                ch = static_cast<uint8_t>(_in.get());
                v |= static_cast<uint32_t>(ch) << 8;
                ch = static_cast<uint8_t>(_in.get());
                v |= static_cast<uint32_t>(ch) << 16;
                ch = static_cast<uint8_t>(_in.get());
                v |= static_cast<uint32_t>(ch) << 24;
                value = convert<std::string>(v);
            }
            break;

        case BinSerializer::TypeInt64:
            {
                int64_t v;
                uint8_t ch = static_cast<uint8_t>(_in.get());
                v = static_cast<int64_t>(ch);
                ch = static_cast<uint8_t>(_in.get());
                v |= static_cast<uint64_t>(ch) << 8;
                ch = static_cast<uint8_t>(_in.get());
                v |= static_cast<uint64_t>(ch) << 16;
                ch = static_cast<uint8_t>(_in.get());
                v |= static_cast<uint64_t>(ch) << 24;
                ch = static_cast<uint8_t>(_in.get());
                v |= static_cast<uint64_t>(ch) << 32;
                ch = static_cast<uint8_t>(_in.get());
                v |= static_cast<uint64_t>(ch) << 40;
                ch = static_cast<uint8_t>(_in.get());
                v |= static_cast<uint64_t>(ch) << 48;
                ch = static_cast<uint8_t>(_in.get());
                v |= static_cast<uint64_t>(ch) << 56;
                value = convert<std::string>(v);
            }
            break;

        case BinSerializer::TypeUInt64:
            {
                uint64_t v;
                uint8_t ch = static_cast<uint8_t>(_in.get());
                v = static_cast<uint64_t>(ch);
                ch = static_cast<uint8_t>(_in.get());
                v |= static_cast<uint64_t>(ch) << 8;
                ch = static_cast<uint8_t>(_in.get());
                v |= static_cast<uint64_t>(ch) << 16;
                ch = static_cast<uint8_t>(_in.get());
                v |= static_cast<uint64_t>(ch) << 24;
                ch = static_cast<uint8_t>(_in.get());
                v |= static_cast<uint64_t>(ch) << 32;
                ch = static_cast<uint8_t>(_in.get());
                v |= static_cast<uint64_t>(ch) << 40;
                ch = static_cast<uint8_t>(_in.get());
                v |= static_cast<uint64_t>(ch) << 48;
                ch = static_cast<uint8_t>(_in.get());
                v |= static_cast<uint64_t>(ch) << 56;
                value = convert<std::string>(v);
            }
            break;

        case BinSerializer::TypeBcdDouble:
            {
                if (_in.get(ch))
                {
                    if (ch == '\xf0')
                    {
                        value = "nan";
                    }
                    else if (ch == '\xf1')
                    {
                        value = "inf";
                    }
                    else if (ch == '\xf2')
                    {
                        value = "-inf";
                    }
                    else
                    {
                        static const char d[16] = "0123456789+-. e";
                        while (_in && ch != '\xff')
                        {
                            value += d[static_cast<uint8_t>(ch) >> 4];
                            if (d[static_cast<uint8_t>(ch) & 0xf] != 0xd)
                                value += d[static_cast<uint8_t>(ch) & 0xf];
                            _in.get(ch);
                        }
                        _in.putback(ch);
                    }
                }
            }
            break;

        case BinSerializer::TypeBool:
            _in.get(ch);
            value = (ch ? "true" : "false");
            break;

        default:
            {
                while (_in.get(ch) && ch != '\0')
                    value += ch;
            }
            break;
    }

    if (_in.get(ch) && ch != '\xff')
        throw SerializationError(CXXTOOLS_ERROR_MSG("end of data marker expected"));

    log_debug("type code=" << typeCode << " value=" << value);

    deser->setValue(Utf8Codec::decode(value.data(), value.size()));
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
