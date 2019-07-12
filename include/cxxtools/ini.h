/*
 * Copyright (C) 2019 Tommi Maekitalo
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

#ifndef CXXTOOLS_INI_H
#define CXXTOOLS_INI_H

#include <cxxtools/iniserializer.h>
#include <cxxtools/inideserializer.h>
#include <iostream>

namespace cxxtools
{
    /**
       Wrapper object to easily print serializable objects as ini to a output stream.

       IniOObject is a little wrapper which makes it easy to output serializable
       objects into a ostream. For this the IniOObject expects a reference to the
       wrapped object and has a output operator for a std::ostream, or actually
       a std::ostream, which prints the object in ini format.
     */
    template <typename ObjectType>
    class IniOObject
    {
        const ObjectType& _constObject;

    public:
        /// Constructor. Needs the wrapped object. Optionally a flag can be
        /// passed whether the ini should be nicely formatted.
        explicit IniOObject(const ObjectType& object)
          : _constObject(object)
        { }

        const ObjectType& object() const
        { return _constObject; }
    };

    /// The output operator for IniOObject. It does the actual work.
    template <typename ObjectType>
    std::ostream& operator<< (std::ostream& out, const IniOObject<ObjectType>& object)
    {
        try
        {
            IniSerializer serializer(out);
            serializer.serialize(object.object());
        }
        catch (const std::exception&)
        {
            out.setstate(std::ios::failbit);
            throw;
        }

        return out;
    }

    /// Function, which creates a IniOObject.
    /// This makes the syntactic sugar perfect. See the example at IniOObject
    /// for its use.
    template <typename ObjectType>
    IniOObject<ObjectType> Ini(const ObjectType& object)
    {
        return IniOObject<ObjectType>(object);
    }

    /**
       Wrapper object to easily read objects as ini from a input stream.

       IniIObject is a little wrapper which makes it easy to read objects
       from a istream. For this the IniIObject expects a reference to the
       wrapped object and has a input operator for a std::istream, or actually
       a std::basic_istream, which reads the object from ini format.
     */
    template <typename ObjectType>
    class IniIOObject : public IniOObject<ObjectType>
    {
        ObjectType& _object;

    public:
        explicit IniIOObject(ObjectType& object)
          : IniOObject<ObjectType>(object),
            _object(object)
        { }

        ObjectType& object()
        { return _object; }
    };

    /// The input operator for IniIOObject. It does the actual work.
    template <typename CharType, typename ObjectType>
    std::basic_istream<CharType>& operator>> (std::basic_istream<CharType>& in, IniIOObject<ObjectType> object)
    {
        try
        {
            IniDeserializer deserializer(in);
            deserializer.deserialize(object.object());
        }
        catch (const std::exception&)
        {
            in.setstate(std::ios::failbit);
            throw;
        }
        return in;
    }

    /// Creates a IniIObject with a reference to a deserializable object.
    /// See IniIObject for a usage example.
    template <typename ObjectType>
    IniIOObject<ObjectType> Ini(ObjectType& object)
    {
        return IniIOObject<ObjectType>(object);
    }

}

#endif // CXXTOOLS_INI_H

