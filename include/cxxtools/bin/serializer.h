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

#ifndef CXXTOOLS_BIN_SERIALIZER_H
#define CXXTOOLS_BIN_SERIALIZER_H

#include <cxxtools/serializationinfo.h>
#include <iosfwd>

namespace cxxtools
{
namespace bin
{

class Serializer
{
public:
    enum class Type : uint8_t
    {
        Empty = 0x00,
        Bool = 0x01,
        Char = 0x02,
        String = 0x03,
        Int = 0x04,
        Binary2 = 0x06,    // followed by zero terminated, 2 byte length field + data
        Binary4 = 0x07,    // followed by zero terminated, 4 byte length field + data
        Int8 = 0x10,
        Int16 = 0x11,
        Int32 = 0x12,
        Int64 = 0x13,
        UInt8 = 0x18,
        UInt16 = 0x19,
        UInt32 = 0x1a,
        UInt64 = 0x1b,
        BcdFloat = 0x20,
        ShortFloat = 0x21, // 1 bit sign, 7 bit exponent, 16 bit mantissa (3 byte)
        MediumFloat = 0x22, // 1 bit sign, 7 bit exponent, 32 bit mantissa (5 byte)
        LongFloat = 0x23,  // 1 bit sign, 15 bit exponent, 64 bit mantissa (10 byte)
        Pair = 0x30,
        Array = 0x31,
        Vector = 0x32,
        List = 0x33,
        Deque = 0x34,
        Set = 0x35,
        Multiset = 0x36,
        Map = 0x37,
        Multimap = 0x38,
        Bcd = 0x3e,
        Other = 0x3f,      // followed by zero terminated type name, data is zero terminated
        PlainEmpty = 0x40,
        PlainBool = 0x41,
        PlainChar = 0x42,
        PlainString = 0x43,
        PlainInt = 0x44,
        PlainBinary2 = 0x46,
        PlainBinary4 = 0x47,
        PlainInt8 = 0x50,
        PlainInt16 = 0x51,
        PlainInt32 = 0x52,
        PlainInt64 = 0x53,
        PlainUInt8 = 0x58,
        PlainUInt16 = 0x59,
        PlainUInt32 = 0x5a,
        PlainUInt64 = 0x5b,
        PlainBcdFloat = 0x60,
        PlainShortFloat = 0x61, // 1 bit sign, 7 bit exponent, 16 bit mantissa
        PlainMediumFloat = 0x62,  // 1 bit sign, 7 bit exponent, 32 bit mantissa
        PlainLongFloat = 0x63,  // 1 bit sign, 15 bit exponent, 64 bit mantissa
        PlainPair = 0x70,
        PlainArray = 0x71,
        PlainVector = 0x72,
        PlainList = 0x73,
        PlainDeque = 0x74,
        PlainSet = 0x75,
        PlainMultiset = 0x76,
        PlainMap = 0x77,
        PlainMultimap = 0x78,
        PlainBcd = 0x7e,
        PlainOther = 0x7f,      // followed by zero terminated type name, data is zero terminated
        CategoryObject = 0xa0,
        CategoryArray = 0xa1,
        CategoryReference = 0xa2,
        RpcRequest = 0xc0,
        RpcResponse = 0xc1,
        RpcException = 0xc2,
        Eod = 0xff
    };

    /// object interface
    Serializer() = default;

    /// object interface
    explicit Serializer(std::ostream& out);

    /// object interface
    Serializer& begin(std::ostream& out);

    /// object interface
    template <typename T>
    Serializer& serialize(const T& v, const std::string& name)
    { serialize(*_streambuf, v, name); return *this; }

    /// object interface
    template <typename T>
    Serializer& serialize(const T& v)
    { serialize(_streambuf, v); return *this; }

    void finish()
    { }

    /// static interface
    static void serialize(std::ostream& out, const SerializationInfo& si);

    /// static interface
    template <typename T>
    static void serialize(std::ostream& out, const T& v)
    {
        SerializationInfo si;
        si <<= v;
        serialize(out, si);
    }

    /// static interface
    template <typename T>
    static void serialize(std::ostream& out, const T& v, const std::string& name)
    {
        SerializationInfo si;
        si <<= v;
        si.setName(name);
        serialize(out, si);
    }

    /// static interface
    static void serialize(std::streambuf& out, const SerializationInfo& si);

    template <typename T>
    static void serialize(std::streambuf& out, const T& v, const std::string& name)
    {
        SerializationInfo si;
        si <<= v;
        si.setName(name);
        serialize(out, si);
    }

    /// static interface
    template <typename T>
    static void serialize(std::streambuf& out, const T& v)
    {
        SerializationInfo si;
        si <<= v;
        serialize(out, si);
    }

    /// static interface
    static std::string toString(const SerializationInfo& si);

    /// static interface
    template <typename T>
    static std::string toString(const T& obj)
    {
        SerializationInfo si;
        si <<= obj;
        return toString(si);
    }

private:
    std::streambuf* _streambuf = nullptr;
};

}
}

#endif // CXXTOOLS_BIN_SERIALIZER_H
