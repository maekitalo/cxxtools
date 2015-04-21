/*
 * Copyright (C) 2007 Tommi Maekitalo
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

#ifndef CXXTOOLS_PROPERTIES_H
#define CXXTOOLS_PROPERTIES_H

#include <cxxtools/propertiesdeserializer.h>
#include <iostream>

namespace cxxtools
{
    /**
       Wrapper object to easily read properties from a input stream and convert them to serializable objects.

     */
    template <typename ObjectType>
    class PropertiesIObject
    {
        ObjectType& _object;
        std::string _name;
        bool _envSubst;
        bool _trim;

      public:
        /// Constructor. Needs the wrapped object.
        explicit PropertiesIObject(ObjectType& object, const std::string& name = std::string())
          : _object(object),
            _name(name),
            _envSubst(false),
            _trim(false)
        { }

        ObjectType& object()
        { return _object; }

        const std::string& name() const
        { return _name; }

        /// Sets the envSubst flag.
        /// When set flag is set, environment variables are substituted in the
        /// value using / `cxxtools::envSubst`.
        PropertiesIObject& envSubst(bool sw) { _envSubst = sw; return *this; }

        /// Returns the envSubst flag.
        bool envSubst() const
        { return _envSubst; }

        /// Sets the trim flag.
        /// When set flag is set, white space is removed from the beginning
        /// and end of the values.
        PropertiesIObject& trim(bool sw)
        { _trim = sw; return *this; }

        /// Returns the trim flag.
        bool trim() const
        { return _trim; }

    };

    /// The input operator for PropertiesIObject. It does the actual work.
    template <typename CharType, typename ObjectType>
    std::basic_istream<CharType>& operator>> (std::basic_istream<CharType>& in, PropertiesIObject<ObjectType> object)
    {
      PropertiesDeserializer deserializer(in);
      if (object.name().empty())
        deserializer.deserialize(object.object());
      else
        deserializer.deserialize(object.object(), object.name());
      return in;
    }

    template <typename ObjectType>
    PropertiesIObject<ObjectType> Properties(ObjectType& object, const std::string& name = std::string())
    {
      return PropertiesIObject<ObjectType>(object, name);
    }

}

#endif // CXXTOOLS_PROPERTIES_H
