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

#ifndef CXXTOOLS_XML_XML_H
#define CXXTOOLS_XML_XML_H

#include <cxxtools/xml/xmlserializer.h>
#include <cxxtools/xml/xmldeserializer.h>
#include <iostream>

namespace cxxtools
{
namespace xml
{
    /**
       Wrapper object to easyly print serializable objects as json to a output stream.

       XmlOObject is a little wrapper which makes it easy to output serializable
       objects into s ostream. For this the XmlOObject expects a reference to the
       wrapped object and has a output operator for a std::ostream, or actually
       a std::basic_ostream, which prints the object in json format.

       Example:
       \code
        std::vector<unsigned> v;
        v.push_back(4);
        v.push_back(17);
        v.push_back(12);
        std::cout << cxxtools::xml::Xml(v) << std::endl;
       \endcode
     */
    template <typename ObjectType>
    class XmlOObject
    {
        const ObjectType& _object;
        std::string _name;
        bool _beautify;
        bool _useAttributes;

      public:
        /// Constructor. Needs the wrapped object. Optionally a flag can be
        /// passed whether the json should be nicely formatted.
        XmlOObject(const ObjectType& object, const std::string& name)
          : _object(object),
            _name(name),
            _beautify(false),
            _useAttributes(true)
        { }

        XmlOObject& beautify(bool sw)
        { _beautify = sw; return *this; }

        bool beautify() const
        { return _beautify; }

        XmlOObject& useAttributes(bool sw)
        { _useAttributes = sw; return *this; }

        bool useAttributes() const
        { return _useAttributes; }

        const ObjectType& object() const
        { return _object; }

        const std::string& name() const
        { return _name; }
    };

    /// The output operator for XmlOObject. It does the actual work.
    template <typename CharType, typename ObjectType>
    std::basic_ostream<CharType>& operator<< (std::basic_ostream<CharType>& out, const XmlOObject<ObjectType>& object)
    {
      XmlSerializer serializer(out);

      serializer.useIndent(object.beautify());
      serializer.useEndl(object.beautify());
      serializer.useAttributes(object.useAttributes());

      serializer.serialize(object.object(), object.name());

      return out;
    }

    /// Function, which creates a XmlOObject.
    /// This makes the syntactic sugar perfect. See the example at XmlOObject
    /// for its use.
    template <typename ObjectType>
    XmlOObject<ObjectType> Xml(const ObjectType& object, const std::string& name)
    {
      return XmlOObject<ObjectType>(object, name);
    }

    template <typename ObjectType>
    class XmlIObject
    {
        ObjectType& _object;

      public:
        explicit XmlIObject(ObjectType& object)
          : _object(object)
        { }

        ObjectType& object()
        { return _object; }
    };

    /// The input operator for XmlIObject. It does the actual work.
    template <typename CharType, typename ObjectType>
    std::basic_istream<CharType>& operator>> (std::basic_istream<CharType>& in, XmlIObject<ObjectType> object)
    {
      XmlDeserializer deserializer(in);
      deserializer.deserialize(object.object());
      return in;
    }

    template <typename ObjectType>
    XmlIObject<ObjectType> Xml(ObjectType& object)
    {
      return XmlIObject<ObjectType>(object);
    }

}
}

#endif // CXXTOOLS_XML_XML_H

