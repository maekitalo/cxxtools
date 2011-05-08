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

#include <cxxtools/api.h>
#include <cxxtools/string.h>
#include <cxxtools/stringstream.h>
#include <cxxtools/conversionerror.h>
#include <sstream>
#include <string>
#include <typeinfo>
 
namespace cxxtools
{

template <typename T>
inline void convert(String& s, const T& value)
{
    OStringStream os;
    os << value;
    s = os.str();
}

template <typename T>
inline void convert(T& t, const String& str)
{
    IStringStream is(str);
    Char ch;
    is >> t;
    if (is.fail() || !(is >> ch).eof())
        ConversionError::doThrow(typeid(T).name(), "cxxtools::String");
}

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
    char ch;
    is >> t;
    if (is.fail() || !(is >> ch).eof())
        ConversionError::doThrow(typeid(T).name(), "std::string");
}

inline void convert(String& s, const String& str)
{
    s = str;
}

inline void convert(std::string& s, const std::string& str)
{
    s = str;
}

CXXTOOLS_API void convert(String& s, bool value);

CXXTOOLS_API void convert(bool& n, const String& str);

CXXTOOLS_API void convert(bool& n, const std::string& str);

CXXTOOLS_API void convert(String& s, char value);

CXXTOOLS_API void convert(char& n, const String& str);

CXXTOOLS_API void convert(String& s, unsigned char value);

CXXTOOLS_API void convert(unsigned char& n, const String& str);

CXXTOOLS_API void convert(String& s, signed char value);

CXXTOOLS_API void convert(signed char& n, const String& str);

inline void convert(String& s, const std::string& value)
{
    s = String::widen(value);
}

inline void convert(std::string& s,const String& str)
{
    s = str.narrow();
}

CXXTOOLS_API void convert(String& s, float value);

CXXTOOLS_API void convert(float& n, const String& str);

CXXTOOLS_API void convert(String& s, double value);

CXXTOOLS_API void convert(double& n, const String& str);

CXXTOOLS_API void convert(std::string& s, float value);

CXXTOOLS_API void convert(float& n, const std::string& str);

CXXTOOLS_API void convert(std::string& s, double value);

CXXTOOLS_API void convert(double& n, const std::string& str);

inline void convert(float& n, const wchar_t* str)
{ convert(n, String(str)); }

inline void convert(float& n, const char* str)
{ convert(n, std::string(str)); }

inline void convert(double& n, const char* str)
{ convert(n, std::string(str)); }

inline void convert(double& n, const wchar_t* str)
{ convert(n, String(str)); }

template<typename T, typename S>
void convert(T& to, const S& from)
{
    StringStream ss;
    if( !(ss << from && ss >> to) )
        ConversionError::doThrow(typeid(T).name(), typeid(S).name());
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
