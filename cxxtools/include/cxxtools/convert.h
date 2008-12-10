/*
 * Copyright (C) 2004-2007 by Marc Boris Duerner
 * Copyright (C) 2004-2007 by Stepan Beal
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
#ifndef CXXTOOLS_CONVERT_H
#define CXXTOOLS_CONVERT_H

#include <cxxtools/sourceinfo.h>
#include <sstream>
#include <string>
#include <stdexcept>
#include <iomanip>
#include <limits>

#define CXXTOOLS_CONVERSIONERROR(to, from) \
    CXXTOOLS_ERROR_MSG("conversion from " #from " to " #to " failed")

namespace cxxtools {

class ConversionError : public std::runtime_error
{
    public:
        ConversionError(const char* msg);

        ~ConversionError() throw()
        {}
};


template <typename T>
inline void convert(std::string& s, const T& value)
{
    std::ostringstream os;
    os << value;
    s = os.str();
}


template <typename T>
inline void convert(T& t, const std::string& str)
{
    std::istringstream is(str);
    is >> t;
}


template<typename T, typename S>
void convert(T& to, const S& from)
{
    std::stringstream ss;
    if( !(ss << from && ss >> to) )
        throw ConversionError( CXXTOOLS_CONVERSIONERROR(streamable, streamable) );
}


template<typename T, typename S>
struct Convert
{
    T operator()(const S& from) const
    {
        T value = T();
        convert(value, from);
        return value;
    }
};


template<typename T, typename S>
T convert(const S& from)
{
    T value = T();
    convert(value, from);
    return value;
}

} // namespace cxxtools

#endif
