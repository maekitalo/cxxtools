/*
 * Copyright (C) 2007 Marc Boris Duerner
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
#ifndef CXXTOOLS_STRINGSTREAM_H
#define CXXTOOLS_STRINGSTREAM_H

#include <cxxtools/api.h>
#include <cxxtools/char.h>
#include <cxxtools/string.h>
#include <sstream>

namespace cxxtools {

class CXXTOOLS_API StringStreamBuffer : public std::basic_stringbuf<cxxtools::Char>
{
    public:
        explicit StringStreamBuffer(std::ios::openmode mode = std::ios::in | std::ios::out);
        explicit StringStreamBuffer(const cxxtools::String& str,
                                    std::ios::openmode mode = std::ios::in | std::ios::out);
};

} // namespace cxxtools


namespace std {

template<>
class CXXTOOLS_API basic_stringstream<cxxtools::Char> : public basic_iostream<cxxtools::Char>
{
    public:
        typedef cxxtools::Char char_type;
        typedef std::char_traits<cxxtools::Char> traits_type;
        typedef std::allocator<cxxtools::Char> allocator_type;
        typedef traits_type::int_type int_type;
        typedef traits_type::pos_type pos_type;
        typedef traits_type::off_type off_type;

    public:
        explicit basic_stringstream(ios_base::openmode mode = ios_base::in | ios_base::out);

        explicit basic_stringstream(const cxxtools::String& str,
                                    std::ios_base::openmode mode = std::ios_base::in | std::ios_base::out);

        virtual ~basic_stringstream();

        basic_stringbuf<cxxtools::Char>* rdbuf() const;

        cxxtools::String str() const;

        void str(const cxxtools::String& str);

    private:
        cxxtools::StringStreamBuffer _buffer;
};

} // namespace std


namespace cxxtools {

    typedef std::basic_stringstream<cxxtools::Char> StringStream;

} // namespace cxxtools

#endif
