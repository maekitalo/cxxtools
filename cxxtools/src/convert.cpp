/*
 * Copyright (C) 2011 Tommi Maekitalo
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

#include <cxxtools/convert.h>
#include <iomanip>
#include <limits>
#include <cctype>

namespace cxxtools
{

namespace
{
  template <typename CharT>
  bool _stricmpL(const CharT* p1, const CharT* p2)
  {
    unsigned n;
    for (n = 0; p1[n] && p2[n]; ++n)
        if (std::tolower(p1[n]) != std::tolower(p2[n]))
            return false;
    return p1[n] == p2[n];
  }

  bool _stricmpL(const String& s, const wchar_t* p)
  {
    String::size_type n = 0;
    for ( ; n < s.size(); ++n)
        if (cxxtools::tolower(s[n]) != p[n])
            return false;
    return p[n] == L'\0';
  }

}

void convert(String& s, bool value)
{
    static const String trueValue = L"true";
    static const String falseValue = L"false";
    s = value ? trueValue : falseValue;
}

void convert(bool& n, const String& str)
{
    if (str == L"true" || str == L"1")
        n = true;
    else if (str == L"false" || str == L"0")
        n = false;
    else
        ConversionError::doThrow("bool", "cxxtools::String");
}

void convert(bool& n, const std::string& str)
{
    if (str == "true" || str == "1")
        n = true;
    else if (str == "false" || str == "0")
        n = false;
    else
        ConversionError::doThrow("bool", "std::string");
}

void convert(String& s, char value)
{
    s = String( 1, Char(value) );
}


void convert(char& n, const String& str)
{
    if ( str.empty() )
        ConversionError::doThrow("char", "cxxtools::String");

    n = str[0].narrow('*');
}


void convert(String& s, unsigned char value)
{
    OStringStream ss;
    unsigned int i = static_cast<unsigned int>(value);
    ss << i;
    s = ss.str();
}


void convert(unsigned char& n, const String& str)
{
    if ( str.empty() )
        ConversionError::doThrow("unsigned char", "cxxtools::String");

    // interpret as numeric value
    IStringStream ss(str);
    Char ch;
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


void convert(String& s, signed char value)
{
    OStringStream ss;
    int i = static_cast<signed int>(value);
    ss << i;
    s = ss.str();
}


void convert(signed char& n, const String& str)
{
    if ( str.empty() )
        ConversionError::doThrow("signed char", "cxxtools::String");
        
    // interpret as numeric value
    IStringStream ss(str);
    Char ch;
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

void convert(String& s, float value)
{
    // not a number
    if (value != value)
    {
        s = L"nan";
        return;
    }

    OStringStream os;
    os << value;
    s = os.str();
}


void convert(float& n, const String& str)
{
    // not a number
    if (_stricmpL(str, L"nan"))
    {
        n = std::numeric_limits<float>::quiet_NaN();
        return;
    }

    // inf
    if (_stricmpL(str, L"inf"))
    {
        n = std::numeric_limits<float>::infinity();
        return;
    }

    // -inf
    if (_stricmpL(str, L"-inf"))
    {
        n = -std::numeric_limits<float>::infinity();
        return;
    }

    IStringStream is(str);
    Char ch;
    is >> n;

    if (is.fail() || !(is >> ch).eof())
    {
        ConversionError::doThrow("float", "cxxtools::String");
    }
}


void convert(String& s, double value)
{
    // not a number
    if (value != value)
    {
        s = L"nan";
        return;
    }

    OStringStream os;
    os << std::fixed << std::setprecision(15) << value;
    s = os.str();
}


void convert(double& n, const String& str)
{
    // not a number
    if (_stricmpL(str, L"nan"))
    {
        n = std::numeric_limits<double>::quiet_NaN();
        return;
    }

    // inf
    if (_stricmpL(str, L"inf"))
    {
        n = std::numeric_limits<double>::infinity();
        return;
    }

    // -inf
    if (_stricmpL(str, L"-inf"))
    {
        n = -std::numeric_limits<double>::infinity();
        return;
    }

    IStringStream is(str);
    Char ch;
    is >> std::fixed >> std::setprecision(15) >> n;

    if (is.fail() || !(is >> ch).eof())
    {
        ConversionError::doThrow("double", "cxxtools::String");
    }
}

void convert(std::string& s, float value)
{
    // not a number
    if (value != value)
    {
        s = "nan";
        return;
    }

    std::ostringstream os;
    os << value;
    s = os.str();
}


void convert(float& n, const std::string& str)
{
    // not a number
    if (_stricmpL(str.c_str(), "nan"))
    {
        n = std::numeric_limits<float>::quiet_NaN();
        return;
    }

    // inf
    if (_stricmpL(str.c_str(), "inf"))
    {
        n = std::numeric_limits<float>::infinity();
        return;
    }

    // -inf
    if (_stricmpL(str.c_str(), "-inf"))
    {
        n = -std::numeric_limits<float>::infinity();
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


void convert(std::string& s, double value)
{
    // not a number
    if (value != value)
    {
        s = "nan";
    }
    else if (value == std::numeric_limits<double>::infinity())
    {
        s = "inf";
    }
    else if (value == -std::numeric_limits<double>::infinity())
    {
        s = "-inf";
    }
    else
    {
        std::ostringstream os;
        os << std::fixed << std::setprecision(15) << value;
        s = os.str();
    }
}

void convert(double& n, const std::string& str)
{
    // not a number
    if (_stricmpL(str.c_str(), "nan"))
    {
        n = std::numeric_limits<double>::quiet_NaN();
        return;
    }

    // inf
    if (_stricmpL(str.c_str(), "inf"))
    {
        n = std::numeric_limits<double>::infinity();
        return;
    }

    // -inf
    if (_stricmpL(str.c_str(), "-inf"))
    {
        n = -std::numeric_limits<double>::infinity();
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

}
