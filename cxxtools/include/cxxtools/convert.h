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
#include <cxxtools/sourceinfo.h>
#include <sstream>
#include <string>
#include <stdexcept>
#include <iomanip>
#include <limits>
#include <iostream>
#include <typeinfo>
 
namespace cxxtools
{

template <typename T>
inline void convert(cxxtools::String& s, const T& value)
{
    cxxtools::StringStream os;
    os << value;
    s = os.str();
}


template <typename T>
inline void convert(T& t, const cxxtools::String& str)
{
    cxxtools::StringStream is(str);
    cxxtools::Char ch;
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


inline void convert(cxxtools::String& s, const cxxtools::String& str)
{
    s = str;
}


inline void convert(cxxtools::String& s, bool value)
{
    s = value ?
        cxxtools::String::widen("true") :
        cxxtools::String::widen("false");
}

inline void convert(bool& n, const cxxtools::String& str)
{
    if( ( str.size() == 4 ) &&
        ( str[0] == cxxtools::Char('t') ) &&
        ( str[1] == cxxtools::Char('r') ) &&
        ( str[2] == cxxtools::Char('u') ) &&
        ( str[3] == cxxtools::Char('e') ) )
    {
        n = true;
    }
    else if( ( str.size() == 5 ) &&
        ( str[0] == cxxtools::Char('f') ) &&
        ( str[1] == cxxtools::Char('a') ) &&
        ( str[2] == cxxtools::Char('l') ) &&
        ( str[3] == cxxtools::Char('s') ) &&
        ( str[4] == cxxtools::Char('e') ) )
    {
        n = false;
    }
    else if( ( str.size() == 1 ) &&
        ( str[0] == cxxtools::Char('1') ) )
    {
        n = true;
    }
    else if( ( str.size() == 1 ) &&
        ( str[0] == cxxtools::Char('0') ) )
    {
        n = false;
    }
    else
    {
        ConversionError::doThrow("bool", "cxxtools::String");
    }
}

inline void convert(bool& n, const std::string& str)
{
    if (str == "true" || str == "1")
        n = true;
    else if (str == "false" || str == "0")
        n = false;
    else
        ConversionError::doThrow("bool", "std::string");
}

inline void convert(cxxtools::String& s, char value)
{
    s = cxxtools::String( 1, cxxtools::Char(value) );
}


inline void convert(char& n, const cxxtools::String& str)
{
    if( str.empty() )
        ConversionError::doThrow("char", "cxxtools::String");

    n = str[0].narrow('*');
}


inline void convert(cxxtools::String& s, unsigned char value)
{
    cxxtools::StringStream ss;
    unsigned int i = static_cast<unsigned int>(value);
    ss << i;
    s = ss.str();
}


inline void convert(unsigned char& n, const cxxtools::String& str)
{
    if( str.empty() )
        ConversionError::doThrow("unsigned char", "cxxtools::String");

    // interpret as numeric value
    cxxtools::StringStream ss(str);
    cxxtools::Char ch;
    unsigned int i = 0;
    ss >> i;
    if (ss.fail()
      || i > std::numeric_limits<unsigned char>::max()
      || i < std::numeric_limits<unsigned char>::min()
      || !(ss >> ch).eof())
    {
        ConversionError::doThrow("unsigned char", "cxxtools::String");
    }

    n = static_cast<unsigned char>(i);
}


inline void convert(cxxtools::String& s, signed char value)
{
    cxxtools::StringStream ss;
    int i = static_cast<signed int>(value);
    ss << i;
    s = ss.str();
}


inline void convert(signed char& n, const cxxtools::String& str)
{
    if( str.empty() )
        ConversionError::doThrow("signed char", "cxxtools::String");
        
    // interpret as numeric value
    cxxtools::StringStream ss(str);
    cxxtools::Char ch;
    int i = 0;
    ss >> i;
    if (ss.fail()
      || i > std::numeric_limits<signed char>::max()
      || i < std::numeric_limits<signed char>::min()
      || !(ss >> ch).eof())
    {
        ConversionError::doThrow("signed char", "cxxtools::String");
    }
    n = static_cast<signed char>(i);
}


inline void convert(cxxtools::String& s, const std::string& value)
{
    s = cxxtools::String::widen(value);
}


inline void convert(std::string& s,const cxxtools::String& str)
{
    s = str.narrow();
}


inline void convert(cxxtools::String& s, float value)
{
    // not a number
    if(value != value)
    {
        s = L"NAN";
        return;
    }

    cxxtools::StringStream os;
    os << value;
    s = os.str();
}


inline void convert(float& n, const cxxtools::String& str)
{
    // not a number
    if(str == L"NAN")
    {
        n = std::numeric_limits<float>::quiet_NaN();
        return;
    }

    cxxtools::StringStream is(str);
    cxxtools::Char ch;
    is >> n;

    if (is.fail() || !(is >> ch).eof())
    {
        ConversionError::doThrow("float", "cxxtools::String");
    }
}


inline void convert(cxxtools::String& s, double value)
{
    // not a number
    if(value != value)
    {
        s = L"NAN";
        return;
    }

    cxxtools::StringStream os;
    os << std::fixed << std::setprecision(15) << value;
    s = os.str();
}


inline void convert(double& n, const cxxtools::String& str)
{
    // not a number
    if(str == L"NAN")
    {
        n = std::numeric_limits<float>::quiet_NaN();
        return;
    }

    cxxtools::StringStream is(str);
    cxxtools::Char ch;
    is >> std::fixed >> std::setprecision(15) >> n;

    if (is.fail() || !(is >> ch).eof())
    {
        ConversionError::doThrow("double", "cxxtools::String");
    }
}

inline void convert(std::string& s, float value)
{
    // not a number
    if(value != value)
    {
        s = "NAN";
        return;
    }

    std::ostringstream os;
    os << value;
    s = os.str();
}


inline void convert(float& n, const std::string& str)
{
    // not a number
    if(str == "NAN")
    {
        n = std::numeric_limits<float>::quiet_NaN();
        return;
    }

    std::istringstream is(str);
    char ch;
    is >> n;

    if (is.fail() || !(is >> ch).eof())
    {
        ConversionError::doThrow("float", "std::string");
    }
}


inline void convert(std::string& s, double value)
{
    // not a number
    if(value != value)
    {
        s = "NAN";
        return;
    }

    std::ostringstream os;
    os << std::fixed << std::setprecision(15) << value;
    s = os.str();
}

inline void convert(double& n, const std::string& str)
{
    // not a number
    if(str == "NAN")
    {
        n = std::numeric_limits<float>::quiet_NaN();
        return;
    }

    std::stringstream is(str);
    char ch;
    is >> std::fixed >> std::setprecision(15) >> n;

    if (is.fail() || !(is >> ch).eof())
    {
        ConversionError::doThrow("double", "std::string");
    }
}

template<typename T, typename S>
void convert(T& to, const S& from)
{
    cxxtools::StringStream ss;
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
