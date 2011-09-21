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
#include <cxxtools/typetraits.h>
#include <sstream>
#include <string>
#include <typeinfo>
#include <limits>
#include <iterator>
#include <cctype>
#include <cmath>

namespace cxxtools
{

//
// Conversions to cxxtools::String
//

inline void convert(String& s, const String& str)
{
    s = str;
}

CXXTOOLS_API void convert(String& s, const std::string& value);

CXXTOOLS_API void convert(String& s, bool value);

CXXTOOLS_API void convert(String& s, char value);
CXXTOOLS_API void convert(String& s, unsigned char value);
CXXTOOLS_API void convert(String& s, signed char value);

CXXTOOLS_API void convert(String& s, short value);
CXXTOOLS_API void convert(String& s, unsigned short value);
CXXTOOLS_API void convert(String& s, int value);
CXXTOOLS_API void convert(String& s, unsigned int value);
CXXTOOLS_API void convert(String& s, long value);
CXXTOOLS_API void convert(String& s, unsigned long value);

CXXTOOLS_API void convert(String& s, float value);
CXXTOOLS_API void convert(String& s, double value);
CXXTOOLS_API void convert(String& s, long double value);

template <typename T>
inline void convert(String& s, const T& value)
{
    OStringStream os;
    os << value;
    s = os.str();
}

//
// Conversions from cxxtools::String
//

CXXTOOLS_API void convert(bool& n, const String& str);

CXXTOOLS_API void convert(char& n, const String& str);
CXXTOOLS_API void convert(unsigned char& n, const String& str);
CXXTOOLS_API void convert(signed char& n, const String& str);

CXXTOOLS_API void convert(short& n, const String& str);
CXXTOOLS_API void convert(unsigned short& n, const String& str);
CXXTOOLS_API void convert(int& n, const String& str);
CXXTOOLS_API void convert(unsigned int& n, const String& str);
CXXTOOLS_API void convert(long& n, const String& str);
CXXTOOLS_API void convert(unsigned long& n, const String& str);

CXXTOOLS_API void convert(float& n, const String& str);
CXXTOOLS_API void convert(double& n, const String& str);
CXXTOOLS_API void convert(long double& n, const String& str);

template <typename T>
inline void convert(T& t, const String& str)
{
    IStringStream is(str);
    Char ch;
    is >> t;
    if (is.fail() || !(is >> ch).eof())
        ConversionError::doThrow(typeid(T).name(), "cxxtools::String");
}

//
// Conversions from const cxxtools::Char* (null-terminated)
//

CXXTOOLS_API void convert(int& n, const Char* str);

//
// Conversions to std::string
//

inline void convert(std::string& s, const std::string& str)
{
    s = str;
}

CXXTOOLS_API void convert(std::string& s, const String& str);

CXXTOOLS_API void convert(std::string& s, bool value);

CXXTOOLS_API void convert(std::string& s, char value);
CXXTOOLS_API void convert(std::string& s, signed char value);
CXXTOOLS_API void convert(std::string& s, unsigned char value);

CXXTOOLS_API void convert(std::string& s, short value);
CXXTOOLS_API void convert(std::string& s, unsigned short value);
CXXTOOLS_API void convert(std::string& s, int value);
CXXTOOLS_API void convert(std::string& s, unsigned int value);
CXXTOOLS_API void convert(std::string& s, long value);
CXXTOOLS_API void convert(std::string& s, unsigned long value);

CXXTOOLS_API void convert(std::string& s, float value);
CXXTOOLS_API void convert(std::string& s, double value);
CXXTOOLS_API void convert(std::string& s, long double value);

template <typename T>
inline void convert(std::string& s, const T& value)
{
    std::ostringstream os;
    os << value;
    s = os.str();
}

//
// Conversions from std::string
//

CXXTOOLS_API void convert(bool& n, const std::string& str);

CXXTOOLS_API void convert(char& n, const std::string& str);
CXXTOOLS_API void convert(signed char& n, const std::string& str);
CXXTOOLS_API void convert(unsigned char& n, const std::string& str);

CXXTOOLS_API void convert(short& n, const std::string& str);
CXXTOOLS_API void convert(unsigned short& n, const std::string& str);
CXXTOOLS_API void convert(int& n, const std::string& str);
CXXTOOLS_API void convert(unsigned int& n, const std::string& str);
CXXTOOLS_API void convert(long& n, const std::string& str);
CXXTOOLS_API void convert(unsigned long& n, const std::string& str);

CXXTOOLS_API void convert(float& n, const std::string& str);
CXXTOOLS_API void convert(double& n, const std::string& str);
CXXTOOLS_API void convert(long double& n, const std::string& str);

template <typename T>
inline void convert(T& t, const std::string& str)
{
    std::istringstream is(str);
    char ch;
    is >> t;
    if (is.fail() || !(is >> ch).eof())
        ConversionError::doThrow(typeid(T).name(), "std::string");
}

//
// Conversions from const char* (null-terminated)
//

CXXTOOLS_API void convert(int& n, const char* str);

//
// Generic stream-based conversions
//

template<typename T, typename S>
void convert(T& to, const S& from)
{
    StringStream ss;
    if( !(ss << from && ss >> to) )
        throw ConversionError("conversion between streamable types failed");
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

//
// number formatting
//

/** @brief Formats an integer in a given format format.
 */
template <typename OutIterT, typename T, typename FormatT>
inline OutIterT putInt(OutIterT it, T i, const FormatT& fmt);

/** @brief Formats an integer in a decimal format.
 */
template <typename OutIterT, typename T>
inline OutIterT putInt(OutIterT it, T i);

/** @brief Formats a floating point value in a given format.
 */
template <typename OutIterT, typename T, typename FormatT>
OutIterT putFloat(OutIterT it, T d, const FormatT& fmt, int precision, bool scientific);

/** @brief Formats a floating point value in default format.
 */
template <typename OutIterT, typename T>
OutIterT putFloat(OutIterT it, T d);

//
// number parsing
//

/** @brief Parses an integer value in a given format.
 */
template <typename InIterT, typename T, typename FormatT>
InIterT getInt(InIterT it, InIterT end, bool& ok, T& n, const FormatT& fmt);

/** @brief Parses an integer value in decimal format.
 */
template <typename InIterT, typename T>
InIterT getInt(InIterT it, InIterT end, bool& ok, T& n);

/** @brief Parses a floating point value in a given format.
 */
template <typename InIterT, typename T, typename FormatT>
InIterT getFloat(InIterT it, InIterT end, bool& ok, T& n, const FormatT& fmt);

/** @brief Parses a floating point value.
 */
template <typename InIterT, typename T>
InIterT getFloat(InIterT it, InIterT end, bool& ok, T& n);


template <typename CharType>
struct NumberFormat
{
    typedef CharType CharT;

    static CharT plus()
    { return '+'; }

    static CharT minus()
    { return '-'; }
};


template <typename CharType>
struct DecimalFormat : public NumberFormat<CharType>
{
    typedef CharType CharT;

    static const unsigned base = 10;
    
    static CharT toChar(unsigned char n)
    {
        return '0' + n; 
    }
    
    /** @brief Converts a character to a digit.
    
        Returns a number equal or less than the base on success or a number
        greater than base on failure.
    */
    static unsigned char toDigit(CharT ch)
    {
        int cc = ch - 48;
        // let negatives overrun
        return static_cast<unsigned>(cc);

    }
};


template <typename CharType>
struct OctalFormat : public NumberFormat<CharType>
{
    typedef CharType CharT;

    static const unsigned base = 8;
    
    static CharT toChar(unsigned char n)
    {
        return '0' + n; 
    }
    
    /** @brief Converts a character to a digit.
    
        Returns a number equal or less than the base on success or a number
        greater than base on failure.
        
    */
    static unsigned char toDigit(CharT ch)
    {
        int cc = ch - 48;
        // let negatives overrun
        return static_cast<unsigned>(cc);

    }
};


template <typename CharType>
struct HexFormat : public NumberFormat<CharType>
{
    typedef CharType CharT;

    static const unsigned base = 16;
    
    static CharT toChar(unsigned char n)
    {
        n &= 0x1F; // prevent overrun
        static const char* digtab = "0123456789abcdef";
        return digtab[n]; 
    }

    /** @brief Converts a character to a digit.
    
        Returns a number equal or less than the base on success or a number
        greater than base on failure.
    */
    static unsigned char toDigit(CharT ch)
    {
        static const unsigned char chartab[64] = 
        {
            0, 1, 2, 3, 4, 5, 6, 7, 8, 9,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
            0xFF,10,11,12,13,14,15,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
            0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
            0xFF,10,11,12,13,14,15,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
        };

        int cc = ch - 48;
        // let negatives overrun
        unsigned uc = static_cast<unsigned>(cc);

        if(uc > 64)
            return 0xFF;
        
        unsigned char idx = static_cast<unsigned char>(uc);
        return chartab[ idx ];
    }
};


template <typename CharType>
struct BinaryFormat : public NumberFormat<CharType>
{
    typedef CharType CharT;

    static const unsigned base = 2;
    
    static CharT toChar(unsigned char n)
    {
        return '0' + n; 
    }
    
    /** @brief Converts a character to a digit.
    
        Returns a number equal or less than the base on success or a number
        greater than base on failure.
        
    */
    static unsigned char toDigit(CharT ch)
    {
        int cc = ch - 48;
        // let negatives overrun
        return static_cast<unsigned>(cc);

    }
};


template <typename CharType>
struct FloatFormat : public DecimalFormat<CharType>
{
    typedef CharType CharT;

    static CharT point()
    { return '.'; }

    static CharT e()
    { return 'e'; }

    static CharT E()
    { return 'E'; }

    static const CharT* nan()
    { 
        static const CharT nanstr[] = { 'n', 'a', 'n', 0 };
        return nanstr; 
    }

    static const CharT* inf()
    { 
        static const CharT nanstr[] = { 'i', 'n', 'f', 0 };
        return nanstr; 
    }
};

//! @internal @brief Returns the absolute value of \a i
inline unsigned char formatAbs(char i, bool& isNeg)
{
    isNeg = i < 0;
    unsigned char u = isNeg ? -i : static_cast<unsigned char>(i);
    return u;
}

//! @internal @brief Returns the absolute value of \a i
inline unsigned char formatAbs(unsigned char i, bool& isNeg)
{
    isNeg = false;
    return i;
}

//! @internal @brief Returns the absolute value of \a i
inline unsigned short formatAbs(short i, bool& isNeg)
{
    isNeg = i < 0;
    unsigned short u = isNeg ? -i : static_cast<unsigned short>(i);
    return u;
}

//! @internal @brief Returns the absolute value of \a i
inline unsigned short formatAbs(unsigned short i, bool& isNeg)
{
    isNeg = false;
    return i;
}

//! @internal @brief Returns the absolute value of \a i
inline unsigned int formatAbs(int i, bool& isNeg)
{
    isNeg = i < 0;
    unsigned int u = isNeg ? -i : static_cast<unsigned int>(i);
    return u;
}

//! @internal @brief Returns the absolute value of \a i
inline unsigned int formatAbs(unsigned int i, bool& isNeg)
{
    isNeg = false;
    return i;
}

//! @internal @brief Returns the absolute value of \a i
inline unsigned long formatAbs(long i, bool& isNeg)
{
    isNeg = i < 0;
    unsigned long u = isNeg ? -i : static_cast<unsigned long>(i);
    return u;
}

//! @internal @brief Returns the absolute value of \a i
inline unsigned long formatAbs(unsigned long i, bool& isNeg)
{
    isNeg = false;
    return i;
}

//! @internal @brief Returns the absolute value of \a i
inline unsigned long long formatAbs(long long i, bool& isNeg)
{
    isNeg = i < 0;
    unsigned long long u = isNeg ? -i : static_cast<unsigned long long>(i);
    return u;
}

//! @internal @brief Returns the absolute value of \a i
inline unsigned long long formatAbs(unsigned long long i, bool& isNeg)
{
    isNeg = false;
    return i;
}

/** @brief Formats an integer in a given format.
 */
template <typename CharT, typename T, typename FormatT>
inline CharT* formatInt(CharT* buf, std::size_t buflen, T si, const FormatT& fmt)
{
    typedef typename IntTraits<T>::Unsigned UnsignedInt;

    CharT* end = buf + buflen;
    CharT* cur = end;

    const unsigned base = fmt.base;

    bool isNeg = false;
    UnsignedInt u = formatAbs(si, isNeg);

    do
    {
        T lsd = u % base;
        u /= base;
        --cur;
        *cur = fmt.toChar( unsigned(lsd) );
    } 
    while(u != 0 && cur != buf);
    
    if(cur == buf)
        return buf;
    
    if(isNeg)
    {
        --cur;
        *cur = fmt.minus();
    }

    return cur;
}

/** @brief Formats an integer in binary format.
 */
template <typename CharT, typename T>
inline CharT* formatInt(CharT* buf, std::size_t buflen, T i, const BinaryFormat<CharT>& fmt)
{
    CharT* end = buf + buflen;
    CharT* cur = end;
    T mask = 1;
    
    do
    {
        --cur;
        *cur = fmt.toChar( unsigned(i & mask));
        i = i >> 1;
    } 
    while(i != 0 && cur != buf);
    
    if(cur == buf)
        return buf;
    
    return cur;
}
   

template <typename OutIterT, typename T, typename FormatT>
inline OutIterT putInt(OutIterT it, T i, const FormatT& fmt)
{
    // large enough even for binary and a sign
    const std::size_t buflen = (sizeof(T) * 8) + 1;
    typename FormatT::CharT buf[buflen];
    typename FormatT::CharT* p = formatInt(buf, buflen, i, fmt);

    typename FormatT::CharT* end = buf + buflen;
    for(; p != end; ++p)
        *it++ = *p;

    return it;
}


template <typename OutIterT, typename T>
inline OutIterT putInt(OutIterT it, T i)
{
    DecimalFormat<char> fmt;
    return putInt(it, i, fmt);
}


template <typename CharT, typename T, typename FormatT>
inline std::size_t formatFloat(CharT* fraction, std::size_t fractSize, int& intpart, int& exp, T n,
                               const FormatT& fmt, int precision, bool scientific)
{
    intpart = 0;
    exp = 0;
    
    if(n == T(0.0) || n != n)
        return 0;

    const bool neg = n < 0;
    if(n < 0)
        n = -n;
    
    if( n == std::numeric_limits<T>::infinity() )
        return 0;
    
    exp = static_cast<int>( std::log10(n) );
    
    if(exp != 0)
        n /= std::pow(T(10.0), exp);

    if( precision >= 0 && std::size_t(precision) < fractSize )
    {
        if( ! scientific )
            precision += exp;
            
        T roundfact = std::pow(T(10.0), precision);
        n = (std::floor((n * roundfact) + T(0.5)) + T(0.1)) / roundfact;
    }

    intpart = static_cast<int>( std::floor(n) );
    n -= intpart;
    if(neg)
        intpart = -intpart;
    
    int digit = 0;
    T eps = std::numeric_limits<T>::epsilon();
    std::size_t places = 0;

    while(n > eps && places < fractSize)
    {
        eps *= 10.0;
        n *= 10.0;
        digit = static_cast<int>( std::floor(n) );
        n -= digit;

        CharT c = fmt.toChar(digit);
        *fraction++ = c;

        ++places;
    }

    return places;
}


template <typename OutIterT, typename T, typename FormatT>
inline OutIterT putFloat(OutIterT it, T d, const FormatT& fmt, int precision, bool scientific)
{
    typedef typename FormatT::CharT CharT;
    CharT zero = fmt.toChar(0);

    // 1. Test for not-a-number with d != d
    if( d != d ) 
    {
        for(const CharT* nanstr = fmt.nan(); *nanstr != 0; ++nanstr)
        {
            *it = *nanstr;
            ++it;
        }

        return it;
    }

    // 2. check sign
    if(d < 0.0)
    {
        *it = fmt.minus();
        ++it;
    }

    T num = std::fabs(d);

    // 3. Test for infinity
    if( num == std::numeric_limits<T>::infinity() ) 
    {
        for(const CharT* infstr = fmt.inf(); *infstr != 0; ++infstr)
        {
            *it = *infstr;
            ++it;
        }

        return it;
    }
    
    const int bufsize = std::numeric_limits<T>::digits10;
    CharT fract[bufsize];
    int i = 0;
    int e = 0;
    int fractSize = formatFloat(fract, bufsize, i, e, num, fmt, precision, scientific);

    // show only significant digits for default format
    precision = 1;
    if( e < fractSize )
        precision = fractSize - e;

    int n = 0;
    if(e >= 0)
    {
        *it++ = fmt.toChar(i);
        for(; n < e; ++n)
            *it++ = (n < fractSize) ? fract[n] : zero;

        *it++ = fmt.point();
    }
    else
    {
        *it++ = zero;
        *it++ = fmt.point();

        for( ;n > ++e && precision > 0; --precision)
            *it++ = zero;

        if(precision-- > 0)
            *it++ = fmt.toChar(i);
    }

    for(; precision > 0; ++n, --precision)
        *it++ = (n < fractSize) ?  fract[n] : zero; 

    return it;
}


template <typename OutIterT, typename T>
inline OutIterT putFloat(OutIterT it, T d)
{
    const int precision = std::numeric_limits<T>::digits10;
    FloatFormat<char> fmt;
    return putFloat(it, d, fmt, precision, false);
}


template <typename InIterT, typename FormatT>
InIterT getSign(InIterT it, InIterT end, bool& pos, const FormatT& fmt)
{
    pos = true;
    
    // strip leading whitespace, parse sign
    while (it != end && isspace(*it))
        ++it;

    if(*it == fmt.minus())
    {
        pos = false;
        ++it;
    }
    else if( *it == fmt.plus() )
    {
        ++it;
    }

    return it;
}


template <typename InIterT, typename T, typename FormatT>
InIterT getInt(InIterT it, InIterT end, bool& ok, T& n, const FormatT& fmt)
{
    typedef typename IntTraits<T>::Unsigned UnsignedInt;
    typedef typename IntTraits<T>::Signed SignedInt;

    n = 0;
    ok = false;
    UnsignedInt max = std::numeric_limits<T>::max();

    bool pos = false;
    it = getSign(it, end, pos, fmt);

    bool isNeg = ! pos;
    if( isNeg )
    {
        // return if minus sign was parsed for unsigned type
        if( isNeg != std::numeric_limits<T>::is_signed)
            return it;

        // abs(min) is max for negative signed types
        SignedInt smin = std::numeric_limits<T>::min();
        max = static_cast<UnsignedInt>(-smin);
    }

    // parse number
    UnsignedInt u = 0;
    const UnsignedInt base = fmt.base;
    unsigned char d = 0;
    while(it != end)
    {    
        d = fmt.toDigit(*it);
        
        if(d >= base)
            break;
        
        if ( u != 0u && base > (max/u) )
          return it;

        u *= base;

        if(d > max - u)
            return it;

        u += d;
        ++it;
    }

    if( isNeg ) 
        n = static_cast<T>(u * -1);
    else
        n = static_cast<T>(u);

    ok = true;
    return it;
}


template <typename InIterT, typename T>
InIterT getInt(InIterT it, InIterT end, bool& ok, T& n)
{
    return getInt(it, end, ok, n, DecimalFormat<char>() );
}


template <typename InIterT, typename T, typename FormatT>
InIterT getFloat(InIterT it, InIterT end, bool& ok, T& n, const FormatT& fmt)
{
    typedef typename FormatT::CharT CharT;
    CharT zero = fmt.toChar(0);
    n = 0.0;
    ok = false;

    bool pos = false;
    it = getSign(it, end, pos, fmt);
    
    // NaN, -inf, +inf
    bool done = false;
    while(it != end)
    {
        switch(*it)
        {
            case 'n':
            case 'N':
                if(++it == end)
                    return it;

                if(*it != 'a' && *it != 'A')
                    return it;

                if(++it == end)
                    return it;

                if(*it != 'n' && *it != 'N')
                    return it;

                n = std::numeric_limits<T>::quiet_NaN();
                ok = true;
                return ++it;
                break;

            case 'i':
            case 'I':
                if(++it == end)
                    return it;

                if(*it != 'n' && *it != 'N')
                    return it;

                if(++it == end)
                    return it;

                if(*it != 'f' && *it != 'F')
                    return it;

                if( ++it != end )
                {
                    if(*it != 'i' && *it != 'I')
                        return it;

                    if(++it == end)
                        return it;

                    if(*it != 'n' && *it != 'N')
                        return it;

                    if(++it == end)
                        return it;

                    if(*it != 'i' && *it != 'I')
                        return it;

                    if(++it == end)
                        return it;

                    if(*it != 't' && *it != 'T')
                        return it;

                    if(++it == end)
                        return it;

                    if(*it != 'y' && *it != 'Y')
                        return it;

                    ++it;
                }

                n = std::numeric_limits<T>::infinity();
                if(!pos)
                    n *= -1;

                ok = true;
                return it;
                break;

            default:
                done = true;
                break;
        }

        if(done)
            break;

        ++it;
    }

    // integral part
    for( ; it != end; ++it)
    {
        if( *it == fmt.point() )
        {
            ++it;
            break;
        }
        
        unsigned digit = fmt.toDigit(*it); 
        if(digit >= fmt.base)
            return it;
        
        n *= 10;
        n += digit; 
    }
    
    // it is ok, if fraction is missing
    if(it == end)
    {
        if( ! pos )
            n *= -1;

        ok = true;
        return it;
    }

    // fractional part, ignore 0 digits after dot
    unsigned short fractDigits = 0;
    size_t maxDigits = std::numeric_limits<unsigned short>::max() - std::numeric_limits<T>::digits10;
    while(it != end && *it == zero)
    {
        if( fractDigits > maxDigits )
            return it;

        ++fractDigits;
        ++it;
    }
 
    // fractional part, parse like integer, skip insignificant digits
    unsigned short significants = 0;
    T fraction = 0.0;
    for( ; it != end; ++it)
    {
        unsigned digit = fmt.toDigit(*it); 
        if(digit >= fmt.base)
            break;

        if( significants <= std::numeric_limits<T>::digits10 )
        {
            fraction *= 10;
            fraction += digit;

            ++fractDigits;
            ++significants;
        }
    }

    // fractional part, scale down
    T base = 10.0;
    fraction /= std::pow(base, T(fractDigits));
    n += fraction;

    // exponent [e|E][+|-][0-9]*
    if(it != end && (*it == fmt.e() || *it == fmt.E()) )
    {
        if(++it == end)
            return it;

        long exp = 0;
        it = getInt(it, end, ok, exp, fmt);
        if( ! ok )
            return it;
            
        n *= std::pow(base, T(exp));
    }

    if( ! pos )
        n *= -1;

    ok = true;
    return it;
}


template <typename InIterT, typename T>
InIterT getFloat(InIterT it, InIterT end, bool& ok, T& n)
{
    return getFloat( it, end, ok, n, FloatFormat<char>() );
}

} // namespace cxxtools

#endif
