/*
 * Copyright (C) 2005-2008 by Dr. Marc Boris Duerner
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
#ifndef cxxtools_SerializationError_h
#define cxxtools_SerializationError_h

#include <string>
#include <stdexcept>

namespace cxxtools
{
class SerializationInfo;

/** @brief Error during serialization of a type

    This Exception indicates a error during serialization caused by
    missing or invalid object attributes.
*/
class SerializationError : public std::runtime_error
{
    public:
        /** @brief Construct with a message
        */
        explicit SerializationError(const std::string& msg);

        /** @brief throws Serialization error
         *  This saves some bytes in library size when using this function instead of throwing directly.
         */
        static void doThrow(const std::string& msg);
};

class SerializationMemberNotFound : public SerializationError
{
        std::string _member;

    public:
        explicit SerializationMemberNotFound(const std::string& member);
        SerializationMemberNotFound(const SerializationInfo& si, const std::string& member);
        SerializationMemberNotFound(const SerializationInfo& si, unsigned idx);
        ~SerializationMemberNotFound() throw()
        { }

        const std::string& member() const
        { return _member; }

};

class SerializationConversionError : public SerializationError
{
    public:
        explicit SerializationConversionError(const std::string& msg)
            : SerializationError(msg)
            { }

        static void doThrow(const SerializationInfo& si, const std::string& typefrom, const std::string& typeto, const std::string& value = std::string());
};

} // namespace cxxtools


#endif
