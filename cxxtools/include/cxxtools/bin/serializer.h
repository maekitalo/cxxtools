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
#include <cxxtools/serializer.h>
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
                    TypeEmpty = 0,
                    TypeBool = 1,
                    TypeChar = 2,
                    TypeString = 3,
                    TypeInt = 4,
                    TypeDouble = 5,
                    TypeBcdDouble = 6,
                    TypeBinary2 = 7,
                    TypeBinary4 = 8,
                    TypeInt8 = 16,
                    TypeInt16 = 17,
                    TypeInt32 = 18,
                    TypeInt64 = 19,
                    TypeUInt8 = 24,
                    TypeUInt16 = 25,
                    TypeUInt32 = 26,
                    TypeUInt64 = 27,
                    TypePair = 32,
                    TypeArray = 64,
                    TypeList = 65,
                    TypeDeque = 66,
                    TypeSet = 67,
                    TypeMultiset = 68,
                    TypeMap = 69,
                    TypeMultimap = 70,
                    TypeOther = 255
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
                    cxxtools::Serializer<T> s;
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

