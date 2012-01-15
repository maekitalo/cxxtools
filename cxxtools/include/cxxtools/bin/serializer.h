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

#ifndef CXXTOOLS_BIN_SERIALIZER_H
#define CXXTOOLS_BIN_SERIALIZER_H

#include <cxxtools/bin/formatter.h>
#include <cxxtools/decomposer.h>
#include <cxxtools/noncopyable.h>

namespace cxxtools
{
    namespace bin
    {
        class Serializer : private NonCopyable
        {
            public:
                enum TypeCode
                {
                    TypeEmpty = 0x00,
                    TypeBool = 0x01,
                    TypeChar = 0x02,
                    TypeString = 0x03,
                    TypeInt = 0x04,
                    TypeShortFloat = 0x05, // 1 bit sign, 7 bit exponent, 16 bit mantissa
                    TypeLongFloat = 0x06,  // 1 bit sign, 15 bit exponent, 64 bit mantissa
                    TypeBcdFloat = 0x07,
                    TypeBinary2 = 0x08,    // followed by zero terminated, 2 byte length field + data
                    TypeBinary4 = 0x09,    // followed by zero terminated, 4 byte length field + data
                    TypeInt8 = 0x10,
                    TypeInt16 = 0x11,
                    TypeInt32 = 0x12,
                    TypeInt64 = 0x13,
                    TypeUInt8 = 0x18,
                    TypeUInt16 = 0x19,
                    TypeUInt32 = 0x1a,
                    TypeUInt64 = 0x1b,
                    TypePair = 0x30,
                    TypeArray = 0x31,
                    TypeVector = 0x32,
                    TypeList = 0x33,
                    TypeDeque = 0x34,
                    TypeSet = 0x35,
                    TypeMultiset = 0x36,
                    TypeMap = 0x37,
                    TypeMultimap = 0x38,
                    TypeOther = 0x3f,      // followed by zero terminated type name, data is zero terminated
                    TypePlainEmpty = 0x40,
                    TypePlainBool = 0x41,
                    TypePlainChar = 0x42,
                    TypePlainString = 0x43,
                    TypePlainInt = 0x44,
                    TypePlainShortFloat = 0x45, // 1 bit sign, 7 bit exponent, 16 bit mantissa
                    TypePlainLongFloat = 0x46,  // 1 bit sign, 15 bit exponent, 64 bit mantissa
                    TypePlainBcdFloat = 0x47,
                    TypePlainBinary2 = 0x48,
                    TypePlainBinary4 = 0x49,
                    TypePlainInt8 = 0x50,
                    TypePlainInt16 = 0x51,
                    TypePlainInt32 = 0x52,
                    TypePlainInt64 = 0x53,
                    TypePlainUInt8 = 0x58,
                    TypePlainUInt16 = 0x59,
                    TypePlainUInt32 = 0x5a,
                    TypePlainUInt64 = 0x5b,
                    TypePlainPair = 0x70,
                    TypePlainArray = 0x71,
                    TypePlainVector = 0x72,
                    TypePlainList = 0x73,
                    TypePlainDeque = 0x74,
                    TypePlainSet = 0x75,
                    TypePlainMultiset = 0x76,
                    TypePlainMap = 0x77,
                    TypePlainMultimap = 0x78,
                    TypePlainOther = 0x7f,      // followed by zero terminated type name, data is zero terminated
                    CategoryObject = 0xa0,
                    CategoryArray = 0xa1,
                    CategoryReference = 0xa2,
                    RpcRequest = 0xc0,
                    RpcResponse = 0xc1,
                    RpcException = 0xc2,
                    Eod = 0xff,
                };

                Serializer()
                {
                }

                explicit Serializer(std::ostream& out)
                {
                    _formatter.begin(out);
                }

                Serializer& begin(std::ostream& out)
                {
                    _formatter.begin(out);
                    return *this;
                }

                template <typename T>
                Serializer& serialize(const T& v, const std::string& name)
                {
                    Decomposer<T> s;
                    s.begin(v);
                    s.setName(name);
                    s.format(_formatter);
                    return *this;
                }

                void finish()
                { }

            private:
                Formatter _formatter;
        };
    }
}

#endif // CXXTOOLS_BIN_SERIALIZER_H

