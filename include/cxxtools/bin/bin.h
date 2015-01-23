/*
 * Copyright (C) 2013 Tommi Maekitalo
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

#ifndef CXXTOOLS_BIN_H
#define CXXTOOLS_BIN_H

#include <cxxtools/bin/serializer.h>
#include <cxxtools/bin/deserializer.h>
#include <iostream>

namespace cxxtools
{
    namespace bin
    {
        /**
           Wrapper object to easily print serializable objects in cxxtools binary
           format to a output stream.

           BinOObject is a little wrapper which makes it easy to output serializable
           objects into s ostream. For this the BinOObject expects a reference to the
           wrapped object and has a output operator for a std::ostream, or actually
           a std::basic_ostream, which prints the object in cxxtools binary format.

           Example:
           \code
            std::vector<unsigned> v;
            v.push_back(4);
            v.push_back(17);
            v.push_back(12);
            std::cout << cxxtools::bin::Bin(v) << std::endl;
           \endcode
         */
        template <typename ObjectType>
        class BinOObject
        {
            const ObjectType& _constObject;

          public:
            /// Constructor. Needs the wrapped object.
            explicit BinOObject(const ObjectType& object)
              : _constObject(object)
            { }

            const ObjectType& object() const
            { return _constObject; }
        };

        /// The output operator for BinOObject. It does the actual work.
        template <typename CharType, typename ObjectType>
        std::basic_ostream<CharType>& operator<< (std::basic_ostream<CharType>& out, const BinOObject<ObjectType>& object)
        {
          try
          {
            Serializer serializer(out);
            serializer.serialize(object.object())
                      .finish();
          }
          catch (const std::exception&)
          {
            out.setstate(std::ios::failbit);
          }

          return out;
        }

        /// Function, which creates a BinOObject.
        /// This makes the syntactic sugar perfect. See the example at BinOObject
        /// for its use.
        template <typename ObjectType>
        BinOObject<ObjectType> Bin(const ObjectType& object)
        {
          return BinOObject<ObjectType>(object);
        }

        template <typename ObjectType>
        class BinIOObject : public BinOObject<ObjectType>
        {
            ObjectType& _object;

          public:
            explicit BinIOObject(ObjectType& object)
              : BinOObject<ObjectType>(object),
                _object(object)
            { }

            ObjectType& object()
            { return _object; }
        };

        /// The input operator for BinIOObject. It does the actual work.
        template <typename CharType, typename ObjectType>
        std::basic_istream<CharType>& operator>> (std::basic_istream<CharType>& in, BinIOObject<ObjectType> object)
        {
          try
          {
            Deserializer deserializer(in);
            deserializer.deserialize(object.object());
          }
          catch (const std::exception&)
          {
            in.setstate(std::ios::failbit);
          }
          return in;
        }

        template <typename ObjectType>
        BinIOObject<ObjectType> Bin(ObjectType& object)
        {
          return BinIOObject<ObjectType>(object);
        }

    }
}

#endif // CXXTOOLS_BIN_H

