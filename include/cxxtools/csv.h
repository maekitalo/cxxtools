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

#ifndef CXXTOOLS_CSV_H
#define CXXTOOLS_CSV_H

#include <cxxtools/csvserializer.h>
#include <iostream>

namespace cxxtools
{
    /**
       Wrapper object to easyly print serializable objets as csv to a output stream.

       CsvObject is a little wrapper which makes it easy to output serializable
       objects into s ostream. For this the CsvObject expects a reference to the
       wrapped object and has a output operator for a std::ostream, or actually
       a std::basic_ostream, which prints the object in json format.

       Example:
       \code
        std::vector<std::vector<int> > v(2);
        v[0].push_back(4);
        v[0].push_back(17);
        v[0].push_back(12);
        v[1].push_back(45);
        v[1].push_back(-3);
        v[1].push_back(76);
        std::cout << cxxtools::Csv(v) << std::endl;
       \endcode
     */
    template <typename ObjectType>
    class CsvObject
    {
        const ObjectType& _object;

      public:
        /// Constructor. Needs the wrapped object.
        explicit CsvObject(const ObjectType& object)
          : _object(object)
        { }

        const ObjectType& object() const
        { return _object; }
    };

    /// The output operator for CsvObject. It does the actual work.
    template <typename CharType, typename ObjectType>
    std::basic_ostream<CharType>& operator<< (std::basic_ostream<CharType>& out, const CsvObject<ObjectType>& object)
    {
      CsvSerializer serializer(out);
      serializer.serialize(object.object());
      return out;
    }

    /// Function, which creates a CsvObject.
    /// This makes the syntactic sugar perfect. See the example at CsvObject
    /// for its use.
    template <typename ObjectType>
    CsvObject<ObjectType> Csv(const ObjectType& object)
    {
      return CsvObject<ObjectType>(object);
    }
}

#endif // CXXTOOLS_CSV_H

