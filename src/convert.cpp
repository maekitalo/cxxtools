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
#include <iterator>

namespace cxxtools
{

template <typename IterT>
void _skipws(IterT& it, IterT end)
{
    while (it != end && isspace(*it))
        ++it;
}

template<typename T>
class nullterm_array_iterator
{
    public:
        using iterator_category = std::input_iterator_tag;
        using value_type = T;
        using difference_type = int;
        using pointer = T*;
        using reference = T&;
        using const_pointer = const T*;
        using const_reference = const T&;

        nullterm_array_iterator()
        : _ptr(0)
        { }

        explicit nullterm_array_iterator(const T* ptr)
        : _ptr(ptr)
        {
            if(*_ptr == '\0')
                _ptr = 0;
        }

        nullterm_array_iterator<T>& operator=(const nullterm_array_iterator<T>& it)
        {
            _ptr = it._ptr;
            return *this;
        }

        bool operator==(const nullterm_array_iterator<T>& it) const
        {
            return _ptr == it._ptr;
        }

        bool operator!=(const nullterm_array_iterator<T>& it) const
        {
            return _ptr != it._ptr;
        }

        const T& operator*() const
        {
            return *_ptr;
        }

        nullterm_array_iterator<T>& operator++()
        {
            if(*++_ptr == '\0')
                _ptr = 0;

            return *this;
        }

        nullterm_array_iterator<T> operator++(int)
        {
            if(*++_ptr == '\0')
                _ptr = 0;

            return *this;
        }

    private:
        const T* _ptr;
};

template <typename T>
void convertInt(T& n, const String& str, const char* typeto)
{
    bool ok = false;
    String::const_iterator r = getInt( str.begin(), str.end(), ok, n, DecimalFormat<Char>() );

    if (ok)
        _skipws(r, str.end());

    if( r != str.end() || ! ok )
        ConversionError::doThrow(typeto, "String", str.narrow().c_str());
}


template <typename T>
void convertInt(T& n, const std::string& str, const char* typeto)
{
    bool ok = false;
    std::string::const_iterator r = getInt( str.begin(), str.end(), ok, n );

    if (ok)
        _skipws(r, str.end());

    if( r != str.end() || ! ok )
        ConversionError::doThrow(typeto, "string", str.c_str());
}


template <typename T>
void convertInt(T& n, const char* str, const char* typeto)
{
    bool ok = false;
    nullterm_array_iterator<char> it(str);
    nullterm_array_iterator<char> end;
    it = getInt( it, end, ok, n );

    if (ok)
        _skipws(it, end);

    if( it != end || ! ok )
        ConversionError::doThrow(typeto, "char*");
}

template <typename T>
void convertFloat(T& n, const String& str, const char* typeto)
{
    bool ok = false;
    String::const_iterator r = getFloat(str.begin(), str.end(), ok, n, FloatFormat<Char>() );

    if (ok)
        _skipws(r, str.end());

    if(r != str.end() || ! ok)
        ConversionError::doThrow(typeto, "String", str.narrow().c_str());
}


template <typename T>
void convertFloat(T& n, const std::string& str, const char* typeto)
{
    bool ok = false;
    std::string::const_iterator r = getFloat(str.begin(), str.end(), ok, n);

    if (ok)
        _skipws(r, str.end());

    if(r != str.end() || ! ok)
        ConversionError::doThrow(typeto, "string", str.c_str());
}


template <typename T>
void convertFloat(T& n, const char* str, const char* typeto)
{
    bool ok = false;
    nullterm_array_iterator<char> it(str);
    nullterm_array_iterator<char> end;
    it = getFloat( it, end, ok, n );

    if (ok)
        _skipws(it, end);

    if( it != end || ! ok )
        ConversionError::doThrow(typeto, "char*", str);
}

//
// Conversions to cxxtools::String
//

void convert(String& s, const std::string& value)
{
    s = String::widen(value);
}

void convert(String& str, bool value)
{
    static const wchar_t* trueValue = L"true";
    static const wchar_t* falseValue = L"false";
    str = value ? trueValue : falseValue;
}

void convert(String& str, char value)
{
    str = String(1, Char(value));
}

void convert(String& str, wchar_t value)
{
    str = String(1, Char(value));
}

void convert(String& str, Char value)
{
    str = String(1, value);
}

void convert(String& str, unsigned char value)
{
    str.clear();
    putInt(std::back_inserter(str), value);
}


void convert(String& str, signed char value)
{
    str.clear();
    putInt(std::back_inserter(str), value);
}


void convert(String& str, short value)
{
    str.clear();
    putInt(std::back_inserter(str), value);
}


void convert(String& str, unsigned short value)
{
    str.clear();
    putInt(std::back_inserter(str), value);
}


void convert(String& str, int value)
{
    str.clear();
    putInt(std::back_inserter(str), value);
}


void convert(String& str, unsigned int value)
{
    str.clear();
    putInt(std::back_inserter(str), value);
}


void convert(String& str, long value)
{
    str.clear();
    putInt(std::back_inserter(str), value);
}


void convert(String& str, unsigned long value)
{
    str.clear();
    putInt(std::back_inserter(str), value);
}


void convert(String& str, long long value)
{
    str.clear();
    putInt(std::back_inserter(str), value);
}

void convert(String& str, unsigned long long value)
{
    str.clear();
    putInt(std::back_inserter(str), value);
}

void convert(String& str, float value)
{
    str.clear();
    putFloat(std::back_inserter(str), value);
}


void convert(String& str, double value)
{
    str.clear();
    putFloat(std::back_inserter(str), value);
}


void convert(String& str, long double value)
{
    str.clear();
    putFloat(std::back_inserter(str), value);
}

//
// Conversions from cxxtools::String
//

void convert(bool& n, const String& str)
{
    if (str == L"true" || str == L"1")
        n = true;
    else if (str == L"false" || str == L"0")
        n = false;
    else
        ConversionError::doThrow("bool", "String", str.narrow().c_str());
}

void convert(char& c, const String& str)
{
    if ( str.empty() )
        ConversionError::doThrow("char", "String");

    c = str[0].narrow();
}

void convert(wchar_t& c, const String& str)
{
    if ( str.empty() )
        ConversionError::doThrow("wchar_t", "String");

    c = str[0].toWchar();
}

void convert(Char& c, const String& str)
{
    if ( str.empty() )
        ConversionError::doThrow("char", "Char");

    c = str[0];
}

void convert(unsigned char& n, const String& str)
{
    convertInt(n, str, "unsigned char");
}


void convert(signed char& n, const String& str)
{
    convertInt(n, str, "signed char");
}


void convert(short& n, const String& str)
{
    convertInt(n, str, "short");
}


void convert(unsigned short& n, const String& str)
{
    convertInt(n, str, "unsigned short");
}


void convert(int& n, const String& str)
{
    convertInt(n, str, "int");
}


void convert(unsigned int& n, const String& str)
{
    convertInt(n, str, "unsigned int");
}


void convert(long& n, const String& str)
{
    convertInt(n, str, "long");
}


void convert(unsigned long& n, const String& str)
{
    convertInt(n, str, "unsigned long");
}


void convert(long long& n, const String& str)
{
    convertInt(n, str, "long long");
}

void convert(unsigned long long& n, const String& str)
{
    convertInt(n, str, "unsigned long long");
}

void convert(float& n, const String& str)
{
    convertFloat(n, str, "float");
}


void convert(double& n, const String& str)
{
    convertFloat(n, str, "double");
}


void convert(long double& n, const String& str)
{
    convertFloat(n, str, "long double");
}

//
// Conversions to std::string
//

void convert(std::string& s, const String& str)
{
    s = str.narrow();
}


void convert(std::string& str, bool value)
{
    static const char* trueValue = "true";
    static const char* falseValue = "false";
    str = value ? trueValue : falseValue;
}


void convert(std::string& str, char value)
{
    str.clear();
    str += value;
}


void convert(std::string& str, signed char value)
{
    str.clear();
    putInt(std::back_inserter(str), value);
}


void convert(std::string& str, unsigned char value)
{
    str.clear();
    putInt(std::back_inserter(str), value);
}


void convert(std::string& str, short value)
{
    str.clear();
    putInt(std::back_inserter(str), value);
}


void convert(std::string& str, unsigned short value)
{
    str.clear();
    putInt(std::back_inserter(str), value);
}


void convert(std::string& str, int value)
{
    str.clear();
    putInt(std::back_inserter(str), value);
}


void convert(std::string& str, unsigned int value)
{
    str.clear();
    putInt(std::back_inserter(str), value);
}


void convert(std::string& str, long value)
{
    str.clear();
    putInt(std::back_inserter(str), value);
}


void convert(std::string& str, unsigned long value)
{
    str.clear();
    putInt(std::back_inserter(str), value);
}


void convert(std::string& str, long long value)
{
    str.clear();
    putInt(std::back_inserter(str), value);
}

void convert(std::string& str, unsigned long long value)
{
    str.clear();
    putInt(std::back_inserter(str), value);
}

void convert(std::string& str, float value)
{
    str.clear();
    putFloat(std::back_inserter(str), value);
}


void convert(std::string& str, double value)
{
    str.clear();
    putFloat(std::back_inserter(str), value);
}


void convert(std::string& str, long double value)
{
    str.clear();
    putFloat(std::back_inserter(str), value);
}

//
// Conversions from std::string
//

void convert(bool& n, const std::string& str)
{
    if (str == "true" || str == "1")
        n = true;
    else if (str == "false" || str == "0")
        n = false;
    else
        ConversionError::doThrow("bool", "string", str.c_str());
}


void convert(char& c, const std::string& str)
{
    if ( str.empty() )
        ConversionError::doThrow("char", "string");

    int n = str[0];
    c = n;
}


void convert(signed char& n, const std::string& str)
{
    convertInt(n, str, "signed char");
}


void convert(unsigned char& n, const std::string& str)
{
    convertInt(n, str, "unsigned char");
}


void convert(short& n, const std::string& str)
{
    convertInt(n, str, "short");
}


void convert(unsigned short& n, const std::string& str)
{
    convertInt(n, str, "unsigned short");
}


void convert(int& n, const std::string& str)
{
    convertInt(n, str, "int");
}


void convert(unsigned int& n, const std::string& str)
{
    convertInt(n, str, "unsigned int");
}


void convert(long& n, const std::string& str)
{
    convertInt(n, str, "long");
}


void convert(unsigned long& n, const std::string& str)
{
    convertInt(n, str, "unsigned long");
}

void convert(long long& n, const std::string& str)
{
    convertInt(n, str, "long long");
}

void convert(unsigned long long& n, const std::string& str)
{
    convertInt(n, str, "unsigned long long");
}

void convert(float& n, const std::string& str)
{
    convertFloat(n, str, "float");
}


void convert(double& n, const std::string& str)
{
    convertFloat(n, str, "double");
}


void convert(long double& n, const std::string& str)
{
    convertFloat(n, str, "long double");
}

//
// Conversions from const char*
//

void convert(bool& n, const char* str)
{
    if (std::strcmp(str, "true") == 0 || std::strcmp(str, "1") == 0)
        n = true;
    else if (std::strcmp(str, "false") || std::strcmp(str, "0"))
        n = false;
    else
        ConversionError::doThrow("bool", "char*", str);
}


void convert(char& c, const char* str)
{
    if ( *str == '\0' )
        ConversionError::doThrow("char", "char*");

    c = str[0];
}


void convert(signed char& n, const char* str)
{
    convertInt(n, str, "signed char");
}


void convert(unsigned char& n, const char* str)
{
    convertInt(n, str, "unsigned char");
}


void convert(short& n, const char* str)
{
    convertInt(n, str, "short");
}


void convert(unsigned short& n, const char* str)
{
    convertInt(n, str, "unsigned short");
}


void convert(int& n, const char* str)
{
    convertInt(n, str, "int");
}


void convert(unsigned int& n, const char* str)
{
    convertInt(n, str, "unsigned int");
}


void convert(long& n, const char* str)
{
    convertInt(n, str, "long");
}


void convert(unsigned long& n, const char* str)
{
    convertInt(n, str, "unsigned long");
}

void convert(long long& n, const char* str)
{
    convertInt(n, str, "long long");
}

void convert(unsigned long long& n, const char* str)
{
    convertInt(n, str, "unsigned long long");
}

void convert(float& n, const char* str)
{
    convertFloat(n, str, "float");
}


void convert(double& n, const char* str)
{
    convertFloat(n, str, "double");
}


void convert(long double& n, const char* str)
{
    convertFloat(n, str, "long double");
}


}
