/***************************************************************************
 *   Copyright (C) 2004-2007 by Marc Boris Duerner                         *
 *   Copyright (C) 2004-2007 by Stepan Beal                                *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this program; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef CXXTOOLS_CONVERT_H
#define CXXTOOLS_CONVERT_H

#include <cxxtools/Api.h>
#include <cxxtools/SourceInfo.h>
#include <sstream>
#include <string>
#include <stdexcept>
#include <iomanip>
#include <limits>

#define CXXTOOLS_CONVERSIONERROR(to, from) \
    "conversion to " #to " from " #from " failed", CXXTOOLS_SOURCEINFO

namespace cxxtools {

class CXXTOOLS_API ConversionError : public std::runtime_error
{
    public:
        ConversionError(const char* msg, const SourceInfo& si);

        virtual ~ConversionError() throw();

        const cxxtools::SourceInfo& where() const;

    private:
        cxxtools::SourceInfo _si;
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
