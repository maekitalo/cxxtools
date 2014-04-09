/*
 * Copyright (C) 2014 by Tommi Maekitalo
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

#ifndef CXXTOOLS_UTF8_H
#define CXXTOOLS_UTF8_H

#include <cxxtools/utf8codec.h>
#include <cxxtools/string.h>
#include <cxxtools/serializationinfo.h>

namespace cxxtools
{
    class Utf8OString
    {
        String _constStr;

    public:
        explicit Utf8OString(const String& str)
            : _constStr(str)
        { }

        const String& ustr() const
        { return _constStr; }

        std::string str() const
        { return encode<Utf8Codec>(_constStr); }

        operator std::string() const
        { return encode<Utf8Codec>(_constStr); }

    };

    class Utf8IOString : public Utf8OString
    {
        std::string& _str;

    public:
        explicit Utf8IOString(std::string& str)
            : Utf8OString(decode<Utf8Codec>(str)),
              _str(str)
        { }

        std::string& str()
        { return _str; }

        String ustr() const
        { return decode<Utf8Codec>(_str); }

        operator String() const
        { return decode<Utf8Codec>(_str); }

    };

    /// Function, which creates a Utf8OString.
    /// This makes the syntactic sugar perfect. See the example at Utf8OString
    /// for its use.
    inline Utf8OString Utf8(const String& str)
    {
        return Utf8OString(str);
    }

    inline Utf8OString Utf8(const std::string& str)
    {
        return Utf8OString(decode<Utf8Codec>(str));
    }

    /// Creates a Utf8IString with a reference to a deserializable object.
    /// See Utf8IString for a usage example.
    inline Utf8IOString Utf8(std::string& str)
    {
        return Utf8IOString(str);
    }

    inline void operator<<= (SerializationInfo& si, const Utf8OString& str)
    {
        si <<= str.ustr();
    }

    inline void operator>>= (SerializationInfo& si, Utf8IOString& str)
    {
        String s;
        si >>= s;
        str.str() = encode<Utf8Codec>(s);
    }

}

#endif // CXXTOOLS_UTF8_H

