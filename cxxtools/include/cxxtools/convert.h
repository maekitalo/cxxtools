/***************************************************************************
 *   Copyright (C) 2004-2007 by Marc Boris Duerner                         *
 *   Copyright (C) 2004-2007 by Stepan Beal                                *
 *   Copyright (C) 2008      by Tommi Maekitalo                            *
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

#include <cxxtools/api.h>
#include <cxxtools/sourceinfo.h>
#include <sstream>
#include <string>
#include <stdexcept>
#include <locale>

namespace cxxtools {

class CXXTOOLS_API ConversionError : public std::runtime_error
{
    public:
        ConversionError(const std::type_info& to, const std::type_info& from,
            const SourceInfo& si);

        const cxxtools::SourceInfo& where() const  { return _si; }
        const std::type_info& to()    { return _to; }
        const std::type_info& from()  { return _from; }

    private:
        cxxtools::SourceInfo _si;
        const std::type_info& _to;
        const std::type_info& _from;
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
        throw ConversionError(typeid(to), typeid(from), CXXTOOLS_SOURCEINFO);
}


template<typename T>
struct Convert
{
    template <typename S>
    T operator()(const S& from) const
    {
        T value;
        convert(value, from);
        return value;
    }
};


template<typename T, typename S>
T convert(const S& from)
{
    T value;
    convert(value, from);
    return value;
}


template <typename T>
inline void convert(std::string& s, const T& value, const std::locale& loc)
{
    std::ostringstream os;
    os.imbue(loc);
    os << value;
    s = os.str();
}


template <typename T>
inline void convert(T& t, const std::string& str, const std::locale& loc)
{
    std::istringstream is(str);
    is.imbue(loc);
    is >> t;
}


template<typename T, typename S>
void convert(T& to, const S& from, const std::locale& loc)
{
    std::stringstream ss;
    ss.imbue(loc);
    if( !(ss << from && ss >> to) )
        throw ConversionError(typeid(from), typeid(to), CXXTOOLS_SOURCEINFO);
}


template<typename T>
class ConvertLocale
{
        std::locale _loc;

    public:
        explicit ConvertLocale(const std::locale& loc = std::locale(""))
            : _loc(loc)
            { }

        template <typename S>
        T operator()(const S& from) const
        {
            T value;
            convert(value, from, _loc);
            return value;
        }
};


template<typename T, typename S>
T convert(const S& from, const std::locale& loc)
{
    T value;
    convert(value, from, loc);
    return value;
}

} // namespace cxxtools

#endif
