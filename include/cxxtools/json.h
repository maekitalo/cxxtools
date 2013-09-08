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
#include <cxxtools/jsondeserializer.h>
#include <iostream>

namespace cxxtools
{
    /**
       Wrapper object to easily print serializable objects as json to a output stream.

       JsonOObject is a little wrapper which makes it easy to output serializable
       objects into s ostream. For this the JsonOObject expects a reference to the
       wrapped object and has a output operator for a std::ostream, or actually
       a std::basic_ostream, which prints the object in json format.

       Example:
       \code
        std::vector<unsigned> v;
        v.push_back(4);
        v.push_back(17);
        v.push_back(12);
        std::cout << cxxtools::Json(v).beautify(true) << std::endl;
       \endcode
     */
    template <typename ObjectType>
    class JsonOObject
    {
        const ObjectType& _constObject;
        bool _beautify;

      public:
        /// Constructor. Needs the wrapped object. Optionally a flag can be
        /// passed whether the json should be nicely formatted.
        explicit JsonOObject(const ObjectType& object, bool beautify = false)
          : _constObject(object),
            _beautify(beautify)
        { }

        /// Sets the formatting for json. If the passed flag is true, enables
        /// nice formatting, otherwise disables it. The json is printed then
        /// without whitespace
        JsonOObject& beautify(bool sw)
        { _beautify = sw; return *this; }

        bool beautify() const
        { return _beautify; }

        const ObjectType& object() const
        { return _constObject; }
    };

    /// The output operator for JsonOObject. It does the actual work.
    template <typename CharType, typename ObjectType>
    std::basic_ostream<CharType>& operator<< (std::basic_ostream<CharType>& out, const JsonOObject<ObjectType>& object)
    {
      JsonSerializer serializer(out);
      serializer.beautify(object.beautify());
      serializer.serialize(object.object())
                .finish();
      return out;
    }

    /// Function, which creates a JsonOObject.
    /// This makes the syntactic sugar perfect. See the example at JsonOObject
    /// for its use.
    template <typename ObjectType>
    JsonOObject<ObjectType> Json(const ObjectType& object, bool beautify = false)
    {
      return JsonOObject<ObjectType>(object, beautify);
    }

    template <typename ObjectType>
    class JsonIOObject : public JsonOObject<ObjectType>
    {
        ObjectType& _object;

      public:
        explicit JsonIOObject(ObjectType& object)
          : JsonOObject<ObjectType>(object),
            _object(object)
        { }

        ObjectType& object()
        { return _object; }
    };

    /// The input operator for JsonIOObject. It does the actual work.
    template <typename CharType, typename ObjectType>
    std::basic_istream<CharType>& operator>> (std::basic_istream<CharType>& in, JsonIOObject<ObjectType> object)
    {
      JsonDeserializer deserializer(in);
      deserializer.deserialize(object.object());
      return in;
    }

    template <typename ObjectType>
    JsonIOObject<ObjectType> Json(ObjectType& object)
    {
      return JsonIOObject<ObjectType>(object);
    }

}

#endif // CXXTOOLS_JSON_H

