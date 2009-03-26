/*
 * Copyright (C) 2005-2007 Marc Boris Duerner
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
#ifndef CXXTOOLS_CHAR_H
#define CXXTOOLS_CHAR_H

#include <cxxtools/api.h>
#include <cxxtools/types.h>
#include <string>
#include <cstring>

namespace cxxtools {

    /**
     * @brief A lightweight Character class (32 bits).
     *
     * Unicode characters are 32-bit entities. This class represents such an entity. It is lightweight, so it
     * can be used everywhere. Most compilers treat it like an unsigned int of 32 bits.
     *
     * This class provides methods for testing/classification, converting to and from other formats, comparing
     * and case-converting. To convert a character or number to a instance of this class use one of the
     * constructors provided. To check the type of the character use one of the method starting with "is", like
     * isLetter() or isDigit(). To compare lower- or upper-case use isUpper() and isLower(). To compare two
     * characters the corresponding operators are overloaded accordingly. Addition and substraction is supported
     * as well. Comparison of numeric values (>, <, ==) is supported when using these operators.
     *
     * The classification methods operate on the full range of Unicode characters. All methods return $true$
     * if the character is a certain type of character. These methods are are wrappers around category() which
     * return the Unicode-defined category of each character.
     *
     * Comparison is critical in Unicode as it covers the characters of the entire world where characters which
     * look the same may be different in the thinking of numeric values (aka positions in the Unicode table)
     * Comparing characters will compare based purely on the numeric Unicode value (code point) of the characters.
     * Upper- and lower-casing using upper() and lower() will only work if the character has a well-defined
     * upper/lower-case equivalent.
     *
     * @see Category
     */
    class Char
    {
        public:
            //! Constructs a character with a value of 0.
            Char()
            : _value(0)
            {}

            //! Constructs a character using the given char as base for the character value.
            Char(char ch)
            : _value( (uint32_t)((unsigned char)ch) )
            {}

            //! Constructs a character using the given 8-bit char as base for the character value.
            Char(signed char ch)
            : _value( (uint32_t)((unsigned char)ch) )
            {}

            //! Constructs a character using the given char as base for the character value.
            Char(unsigned char ch)
            : _value( (uint32_t)(ch) )
            {}

            //! Constructs a character using the given 16-bit integer as base for the character value.
            Char(short val)
            : _value( (uint32_t)((unsigned short)val) )
            {}

            //! Constructs a character using the given 32-bit integer as base for the character value.
            Char(const int& val)
            : _value( (uint32_t)(val) )
            {}

            //! Constructs a character using the given 32-bit integer as base for the character value.
            Char(const unsigned int& val)
            : _value(val)
            {}

            //! Constructs a character using the given long as base for the character value.
            Char(const long value)
            : _value( (uint32_t)((unsigned long)value) )
            {}

            //! Constructs a character using the given long as base for the character value.
            Char(const unsigned long val)
            : _value(val)
            {}

            /**
             * @brief Narrows this character into an 8-bit char if possible.
             *
             * If the character can not be converted into an 8-bit char because its value is
             * greater than 255, the defaultCharacter which is passed to this method is returned.
             *
             * If this character is equal or lower than 255 the character is cast to char.
             *
             * @param def The default character which is returned if this character can not be narrowed
             * @return An 8-bit char which is a narrowed representation of this character object or
             * the default character if this character object's value is out of range (>255).
             */
            char narrow(char def) const;

            static Char null()
            {
                return Char(0);
            }

            /**
             * @brief Assigns the given uint32_t as new value for this character.
             * @param value The new 32-bit value for this character.
             * @return A reference to this object to allow concatination of operations.
             */
            Char& operator=(uint32_t value)
            { _value = value; return *this; }

            Char& operator=(const Char& ch)
            { _value = ch._value; return *this; }

            /**
             * @brief Returns the internal value (unsigned 32 bits) of this character.
             * @return The 32-bit-value of this character.
             */
            uint32_t value() const
            { return _value; }

            /**
             * @brief This conversion operator converts the internal value of this character to unsigned 32 bits.
             *
             * As the internal value also is an unsigned 32-bit value, the internal value of this character
             * ist returned.
             *
             * @return The character converted to unsigned 32-bit.
             */
            operator int() const
            { return _value; }

            /**
             * @brief Substracts the numeric value of this character and the numeric value of the given character and
             * stores the result in this chracater class.
             *
             * @param value This character's numeric value is subtracted from this' character numeric value.
             * @return A reference to this character class.
             */
            Char& operator-=(const Char& value)
            {
                this->_value -= value._value;
                return *this;
            }

            /**
             * @brief Sums the numeric value of this character and the numeric value of the given character and
             * stores the result in this chracater class.
             *
             * @param value This character's numeric value is added to this' character numeric value.
             * @return A reference to this character class.
             */
            Char& operator+=(const Char& value)
            {
                this->_value += value._value;
                return *this;
            }

            Char& operator>>=(const Char& value)
            {
                this->_value >>= value._value;
                return *this;
            }

            Char& operator<<=(const Char& value)
            {
                this->_value <<= value._value;
                return *this;
            }

            //! @brief Returns $true$ if the a and b are the same character; $false$ otherwise.
            //! @return $true$ if the a and b are the same character; $false$ otherwise.
            friend bool operator==(const Char& a, const Char& b)
            { return a.value() == b.value(); }

            //! @brief Returns $true$ if the a and b are the same character; $false$ otherwise.
            //! @return $true$ if the a and b are the same character; $false$ otherwise.
            friend bool operator==(const Char& a, char b)
            { return a.value() == (unsigned char)b; }

            //! @brief Returns $true$ if the a and b are the same character; $false$ otherwise.
            //! @return $true$ if the a and b are the same character; $false$ otherwise.
            friend bool operator==(const Char& a, int b)
            { return a.value() == (unsigned int)b; }

            //! @brief Returns $true$ if the a and b are not the same character; $false$ otherwise.
            //! @return $true$ if the a and b are not the same character; $false$ otherwise.
            friend bool operator!=(const Char& a, const Char& b)
            { return a.value() != b.value(); }

            //! @brief Returns $true$ if the a and b are the same character; $false$ otherwise.
            //! @return $true$ if the a and b are the same character; $false$ otherwise.
            friend bool operator!=(const Char& a, char b)
            { return a.value() != (unsigned char)b; }

            //! @brief Returns $true$ if the a and b are the same character; $false$ otherwise.
            //! @return $true$ if the a and b are the same character; $false$ otherwise.
            friend bool operator!=(const Char& a, wchar_t b)
            { return a.value() != (unsigned int)b; }

            //! @brief Returns $true$ if the a and b are the same character; $false$ otherwise.
            //! @return $true$ if the a and b are the same character; $false$ otherwise.
            friend bool operator!=(const Char& a, int b)
            { return a.value() != (unsigned int)b; }

            //! @brief Returns $true$ if the numeric value of a is less than the numeric value of b; $false$ otherwise.
            //! @return $true$ if the numeric value of a is less than the numeric value of b; $false$ otherwise.
            friend bool operator<(const Char& a, const Char& b)
            { return a.value() < b.value(); }

            //! @brief Returns $true$ if the numeric value of a is greater than the numeric value of b; $false$ otherwise.
            //! @return $true$ if the numeric value of a is greater than the numeric value of b; $false$ otherwise.
            friend bool operator>(const Char& a, const Char& b)
            { return a.value() > b.value(); }

            //! @brief Returns $true$ if the numeric value of a is equal or less than the numeric value of b; $false$ otherwise.
            //! @return $true$ if the numeric value of a is equal or less than the numeric value of b; $false$ otherwise.
            friend  bool operator<=(const Char& a, const Char& b)
            { return a.value() <= b.value(); }

            friend  bool operator<=(const Char& a, int b)
            { return a.value() <= static_cast<uint32_t>(b); }

            //! @brief Returns $true$ if the numeric value of a is equals or greater than the numeric value of b; $false$ otherwise.
            //! @return $true$ if the numeric value of a is equals or greater than the numeric value of b; $false$ otherwise.
            friend  bool operator>=(const Char& a, const Char& b)
            { return a.value() >= b.value(); }

            friend  bool operator>=(const Char& a, int b)
            { return a.value() >= static_cast<uint32_t>(b); }

            //! @brief Sums the numeric value of a and the numeric value of b and returns the sum.
            //! @return The sum of the numeric values of a and b.
            friend Char operator+(const Char& a, const Char& b)
            { return a.value() + b.value(); }

            //! @brief Sums the numeric value of a and the numeric value of b and returns the sum.
            //! @return The sum of the numeric values of a and b.
            friend Char operator+(const Char& a, char ch)
            { return a.value() + ch; }

            friend Char operator+(const Char& a, int ch)
            { return a.value() + ch; }

            //! @brief Subtracts the numeric value of b from the numeric value of b and returns the result.
            //! @return The substraction of the numeric values of b from a.
            friend Char operator-(const Char& a, const Char& b)
            { return a.value() - b.value(); }

            //! @brief Subtracts the numeric value of b from the numeric value of b and returns the result.
            //! @return The substraction of the numeric values of b from a.
            friend Char operator-(const Char& a, char ch)
            { return a.value() - ch; }

            //! @brief Does an OR-combination of the numeric value of a and b and returns the result.
            //! @return The OR-combination of the numeric values of a and b.
            friend Char operator|(const Char& a, const Char& b)
            { return a.value() | b.value(); }

            //! @brief Does an AND-combination of the numeric value of a and b and returns the result.
            //! @return The AND-combination of the numeric values of a and b.
            friend Char operator&(const Char& a, const Char& b)
            { return a.value() & b.value(); }

            friend Char operator&(const Char& a, int b)
            { return a.value() & b; }

        private:
            cxxtools::uint32_t _value;
    };

    struct MBState
    {
        MBState()
        : n(0)
        {}

        int n;
        union {
            cxxtools::uint32_t wchars[4];
            char mbytes[16];
        } value;
    };

} // namespace cxxtools


namespace std {

    /// @cond INTERNAL
    template<>
    struct char_traits<cxxtools::Char>
    {
        typedef cxxtools::Char char_type;
        typedef cxxtools::uint32_t int_type;
        typedef streamoff off_type;
        typedef streampos pos_type;
        typedef cxxtools::MBState state_type;

        inline static void assign(char_type& c1, const char_type& c2);

        inline static bool eq(const char_type& c1, const char_type& c2);

        inline static bool lt(const char_type& c1, const char_type& c2);

        inline static int compare(const char_type* c1, const char_type* c2, size_t n);

        inline static size_t length(const char_type* s);

        inline static const char_type* find(const char_type* s, size_t n, const char_type& a);

        inline static char_type* move(char_type* s1, const char_type* s2, int_type n);

        inline static char_type* copy(char_type* s1, const char_type* s2, size_t n);

        inline static char_type* assign(char_type* s, size_t n, char_type a);

        inline static char_type to_char_type(const int_type& c);

        inline static int_type to_int_type(const char_type& c);

        inline static bool eq_int_type(const int_type& c1, const int_type& c2);

        inline static int_type eof();

        inline static int_type not_eof(const int_type& c);
    };


    inline void char_traits<cxxtools::Char>::assign(char_type& c1, const char_type& c2)
    {
        c1 = c2;
    }


    inline bool char_traits<cxxtools::Char>::eq(const char_type& c1, const char_type& c2)
    {
        return c1 == c2;
    }


    inline bool char_traits<cxxtools::Char>::lt(const char_type& c1, const char_type& c2)
    {
        return c1 < c2;
    }


    inline int char_traits<cxxtools::Char>::compare(const char_type* s1, const char_type* s2, size_t n)
    {
        while(n-- > 0)
        {
            if( !eq(*s1, *s2) )
                return lt(*s1, *s2) ? -1 : +1;

            ++s1;
            ++s2;
        }

        return 0;
    }


    inline size_t char_traits<cxxtools::Char>::length(const char_type* s)
    {
        static const cxxtools::Char term(0);
        std::size_t n = 0;
        while( !eq(s[n], term) )
            ++n;

        return n;
    }


    inline const char_traits<cxxtools::Char>::char_type*
    char_traits<cxxtools::Char>::find(const char_type* s, size_t n, const char_type& a)
    {
        while(n-- > 0) {
            if (*s == a)
                return s;
            ++s;
        }

        return 0;
    }


    inline char_traits<cxxtools::Char>::char_type*
    char_traits<cxxtools::Char>::move(char_type* s1, const char_type* s2, int_type n)
    {
        return (cxxtools::Char*)std::memmove(s1, s2, n * sizeof(cxxtools::Char));
    }


    inline char_traits<cxxtools::Char>::char_type*
    char_traits<cxxtools::Char>::copy(char_type* s1, const char_type* s2, size_t n)
    {
        return (cxxtools::Char*)std::memcpy(s1, s2, n * sizeof(cxxtools::Char));
    }


    inline char_traits<cxxtools::Char>::char_type*
    char_traits<cxxtools::Char>::assign(char_type* s, size_t n, char_type a)
    {
        while(n-- > 0) {
            *(s++) = a;
        }

        return s;
    }


    inline char_traits<cxxtools::Char>::char_type
    char_traits<cxxtools::Char>::to_char_type(const int_type& c)
    {
        return char_type(c);
    }


    inline char_traits<cxxtools::Char>::int_type
    char_traits<cxxtools::Char>::to_int_type(const char_type& c)
    {
        return c.value();
    }


    inline bool char_traits<cxxtools::Char>::eq_int_type(const int_type& c1, const int_type& c2)
    {
        return c1 == c2;
    }


    inline char_traits<cxxtools::Char>::int_type char_traits<cxxtools::Char>::eof()
    {
        return static_cast<char_traits<cxxtools::Char>::int_type>( cxxtools::uint32_t(-1) );
    }


    inline char_traits<cxxtools::Char>::int_type char_traits<cxxtools::Char>::not_eof(const int_type& c)
    {
        return eq_int_type(c, eof()) ? 0 : c;
    }

} // namespace std

namespace cxxtools {

    inline char Char::narrow(char def) const
    {
        if( _value == std::char_traits<Char>::eof() )
        {
            return std::char_traits<char>::eof();
        }

        if( _value <= 0xff )
        {
            return (char)_value;
        }

        return def;
    }
} // namespace cxxtools

#endif
