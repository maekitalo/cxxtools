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

#ifndef CXXTOOLS_JSON_H
#define CXXTOOLS_JSON_H

#include <cxxtools/jsonserializer.h>
#include <iostream>

namespace cxxtools
{
    /**
       Wrapper object to easyly print serializable objets as json to a output stream.

       JsonObject is a little wrapper which makes it easy to output serializable
       objects into s ostream. For this the JsonObject expects a reference to the
       wrapped object and has a output operator for a std::ostream, or actually
       a std::basic_ostream, which prints the object in json format.

       Example:
       \code
        std::vector<unsigned> v;
        v.push_back(4);
        v.push_back(17);
        v.push_back(12);
        std::cout << cxxtools::Json(v).beautify() << std::endl;
       \endcode
     */
    template <typename ObjectType>
    class JsonObject
    {
        const ObjectType& _object;
        bool _beautify;

      public:
        /// Constructor. Needs the wrapped object. Optionally a flag can be
        /// passed whether the json should be nicely formatted.
        explicit JsonObject(const ObjectType& object, bool beautify = false)
          : _object(object),
            _beautify(beautify)
        { }

        /// Sets the formatting for json. If the passed flag is true, enables
        /// nice formatting, otherwise disables it. The json is printed then
        /// without whitespace
        JsonObject& beautify(bool sw)
        { _beautify = sw; return *this; }

        bool beautify() const
        { return _beautify; }

        const ObjectType& object() const
        { return _object; }
    };

    /// The output operator for JsonObject. It does the actual work.
    template <typename CharType, typename ObjectType>
    std::basic_ostream<CharType>& operator<< (std::basic_ostream<CharType>& out, const JsonObject<ObjectType>& object)
    {
      JsonSerializer serializer(out);
      serializer.beautify(object.beautify());
      serializer.serialize(object.object())
                .finish();
      return out;
    }

    /// Function, which creates a JsonObject.
    /// This makes the syntactic sugar perfect. See the example at JsonObject
    /// for its use.
    template <typename ObjectType>
    JsonObject<ObjectType> Json(const ObjectType& object)
    {
      return JsonObject<ObjectType>(object);
    }
}

#endif // CXXTOOLS_JSON_H

