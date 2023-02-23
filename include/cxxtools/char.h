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

#include <cxxtools/config.h>
#include <string>
#include <cstring>
#include <ios>
#include <limits>
#include <stdint.h>

namespace cxxtools
{

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
            typedef char32_t value_type;

            //! Constructs a character with a value of 0.
            Char() = default;

            //! Constructs a character using the given value as base for the character value.
            Char(value_type ch)
            : _value(ch)
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
            char narrow(char def = '?') const;

            wchar_t toWchar() const
            { return wchar_t(value()); }

            static Char null()
            {
                return Char(0);
            }

            /**
             * @brief Returns the internal value (unsigned 32 bits) of this character.
             * @return The 32-bit-value of this character.
             */
            value_type value() const
            { return _value; }

            /**
             * @brief This conversion operator converts the internal value of this character to unsigned 32 bits.
             *
             * As the internal value also is an unsigned 32-bit value, the internal value of this character
             * ist returned.
             *
             * @return The character converted to unsigned 32-bit.
             */
            operator value_type() const
            { return _value; }

            //! @brief Returns $true$ if the a and b are the same character; $false$ otherwise.
            //! @return $true$ if the a and b are the same character; $false$ otherwise.
            friend bool operator==(const Char& a, const Char& b)
            { return a.value() == b.value(); }
            friend bool operator==(const Char& a, wchar_t b)
            { return a.value() == static_cast<value_type>(b); }
            friend bool operator==(wchar_t a, const Char& b)
            { return static_cast<value_type>(a) == b.value(); }
            friend bool operator==(const Char& a, char b)
            { return a.value() == static_cast<value_type>(b); }
            friend bool operator==(char a, const Char& b)
            { return static_cast<value_type>(a) == b.value(); }

            //! @brief Returns $true$ if the a and b are not the same character; $false$ otherwise.
            //! @return $true$ if the a and b are not the same character; $false$ otherwise.
            friend bool operator!=(const Char& a, const Char& b)
            { return a.value() != b.value(); }
            friend bool operator!=(const Char& a, wchar_t b)
            { return a.value() != static_cast<value_type>(b); }
            friend bool operator!=(wchar_t a, const Char& b)
            { return static_cast<value_type>(a) != b.value(); }
            friend bool operator!=(const Char& a, char b)
            { return a.value() != static_cast<value_type>(b); }
            friend bool operator!=(char a, const Char& b)
            { return static_cast<value_type>(a) != b.value(); }

            //! @brief Returns $true$ if the numeric value of a is less than the numeric value of b; $false$ otherwise.
            //! @return $true$ if the numeric value of a is less than the numeric value of b; $false$ otherwise.
            friend bool operator<(const Char& a, const Char& b)
            { return a.value() < b.value(); }
            friend bool operator<(const Char& a, wchar_t b)
            { return a.value() < static_cast<value_type>(b); }
            friend bool operator<(wchar_t a, const Char& b)
            { return static_cast<value_type>(a) < b.value(); }
            friend bool operator<(const Char& a, char b)
            { return a.value() < static_cast<value_type>(b); }
            friend bool operator<(char a, const Char& b)
            { return static_cast<value_type>(a) < b.value(); }

            //! @brief Returns $true$ if the numeric value of a is greater than the numeric value of b; $false$ otherwise.
            //! @return $true$ if the numeric value of a is greater than the numeric value of b; $false$ otherwise.
            friend bool operator>(const Char& a, const Char& b)
            { return a.value() > b.value(); }
            friend bool operator>(const Char& a, wchar_t b)
            { return a.value() > static_cast<value_type>(b); }
            friend bool operator>(wchar_t a, const Char& b)
            { return static_cast<value_type>(a) > b.value(); }
            friend bool operator>(const Char& a, char b)
            { return a.value() > static_cast<value_type>(b); }
            friend bool operator>(char a, const Char& b)
            { return static_cast<value_type>(a) > b.value(); }

            //! @brief Returns $true$ if the numeric value of a is equal or less than the numeric value of b; $false$ otherwise.
            //! @return $true$ if the numeric value of a is equal or less than the numeric value of b; $false$ otherwise.
            friend  bool operator<=(const Char& a, const Char& b)
            { return a.value() <= b.value(); }
            friend  bool operator<=(const Char& a, wchar_t b)
            { return a.value() <= static_cast<value_type>(b); }
            friend  bool operator<=(wchar_t a, const Char& b)
            { return static_cast<value_type>(a) <= b.value(); }
            friend  bool operator<=(const Char& a, char b)
            { return a.value() <= static_cast<value_type>(b); }
            friend  bool operator<=(char a, const Char& b)
            { return static_cast<value_type>(a) <= b.value(); }

            //! @brief Returns $true$ if the numeric value of a is equals or greater than the numeric value of b; $false$ otherwise.
            //! @return $true$ if the numeric value of a is equals or greater than the numeric value of b; $false$ otherwise.
            friend  bool operator>=(const Char& a, const Char& b)
            { return a.value() >= b.value(); }
            friend  bool operator>=(const Char& a, wchar_t b)
            { return a.value() >= static_cast<value_type>(b); }
            friend  bool operator>=(wchar_t a, const Char& b)
            { return static_cast<value_type>(a) >= b.value(); }
            friend  bool operator>=(const Char& a, char b)
            { return a.value() >= static_cast<value_type>(b); }
            friend  bool operator>=(char a, const Char& b)
            { return static_cast<value_type>(a) >= b.value(); }

        private:
            value_type _value;
    };

    /// @cond internal
    struct MBState
    {
        MBState()
        : n(0)
        {}

        uint8_t n;
        union {
            Char::value_type wchars[4];
            char mbytes[16];
        } value;
    };
    /// @endcond internal

    std::ostream& operator<< (std::ostream& out, Char ch);

} // namespace cxxtools


namespace std {

    /// @cond internal
    template<>
    struct char_traits<cxxtools::Char>
    {
        typedef cxxtools::Char char_type;
        typedef int64_t int_type;
        typedef std::streamoff off_type;
        typedef std::streampos pos_type;
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
    /// @endcond internal

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
        return static_cast<cxxtools::Char*>(std::memmove(static_cast<void*>(s1), static_cast<const void*>(s2), n * sizeof(cxxtools::Char)));
    }


    inline char_traits<cxxtools::Char>::char_type*
    char_traits<cxxtools::Char>::copy(char_type* s1, const char_type* s2, size_t n)
    {
        return static_cast<cxxtools::Char*>(std::memcpy(static_cast<void*>(s1), static_cast<const void*>(s2), n * sizeof(cxxtools::Char)));
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
        return std::numeric_limits<int_type>::max();
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

#ifdef CXXTOOLS_WITH_STD_LOCALE

#include <locale>

namespace std {

#if (defined _MSC_VER || defined __QNX__ || defined __xlC__)

    /** @brief Ctype localization facet
        @ingroup Unicode
    */
    template <>
    class ctype< cxxtools::Char > : public ctype_base {

#else
    /** @brief Ctype localization facet
        @ingroup Unicode
    */
    template <>
    class ctype<cxxtools::Char> : public ctype_base, public locale::facet {

#endif

    public:
        typedef ctype_base::mask mask;

        static locale::id id;
        virtual locale::id& __get_id (void) const { return id; }

    public:
        explicit ctype(size_t refs = 0);

        virtual ~ctype();

        bool is(mask m, cxxtools::Char c) const
        { return this->do_is(m, c); }

        const cxxtools::Char* is(const cxxtools::Char *lo, const cxxtools::Char *hi, mask *vec) const
        { return this->do_is(lo, hi, vec); }

        const cxxtools::Char* scan_is(mask m, const cxxtools::Char* lo, const cxxtools::Char* hi) const
        { return this->do_scan_is(m, lo, hi); }

        const cxxtools::Char* scan_not(mask m, const cxxtools::Char* lo, const cxxtools::Char* hi) const
        { return this->do_scan_not(m, lo, hi); }

        cxxtools::Char toupper(cxxtools::Char c) const
        { return this->do_toupper(c); }

        const cxxtools::Char* toupper(cxxtools::Char *lo, const cxxtools::Char* hi) const
        { return this->do_toupper(lo, hi); }

        cxxtools::Char tolower(cxxtools::Char c) const
        { return this->do_tolower(c); }

        const cxxtools::Char* tolower(cxxtools::Char* lo, const cxxtools::Char* hi) const
        { return this->do_tolower(lo, hi); }

        cxxtools::Char widen(char c) const
        { return this->do_widen(c); }

        const char* widen(const char* lo, const char* hi, cxxtools::Char* to) const
        { return this->do_widen(lo, hi, to); }

        char narrow(cxxtools::Char c, char dfault) const
        { return this->do_narrow(c, dfault); }

        const cxxtools::Char* narrow(const cxxtools::Char* lo, const cxxtools::Char* hi,
                            char dfault, char *to) const
        { return this->do_narrow(lo, hi, dfault, to); }

    protected:
        virtual bool do_is(mask m, cxxtools::Char c) const;

        virtual const cxxtools::Char* do_is(const cxxtools::Char* lo, const cxxtools::Char* hi,
                                    mask* vec) const;

        virtual const cxxtools::Char* do_scan_is(mask m, const cxxtools::Char* lo,
                                            const cxxtools::Char* hi) const;

        virtual const cxxtools::Char* do_scan_not(mask m, const cxxtools::Char* lo,
                                            const cxxtools::Char* hi) const;

        virtual cxxtools::Char do_toupper(cxxtools::Char) const;

        virtual const cxxtools::Char* do_toupper(cxxtools::Char* lo, const cxxtools::Char* hi) const;

        virtual cxxtools::Char do_tolower(cxxtools::Char) const;

        virtual const cxxtools::Char* do_tolower(cxxtools::Char* lo, const cxxtools::Char* hi) const;

        virtual cxxtools::Char do_widen(char) const;

        virtual const char* do_widen(const char* lo, const char* hi,
                                    cxxtools::Char* dest) const;

        virtual char do_narrow(cxxtools::Char, char dfault) const;

        virtual const cxxtools::Char* do_narrow(const cxxtools::Char* lo, const cxxtools::Char* hi,
                                        char dfault, char* dest) const;
};

} // namespace std

#else

namespace std {

class ctype_base
{
    public:
        enum {
            alpha  = 1 << 5,
            cntrl  = 1 << 2,
            digit  = 1 << 6,
            lower  = 1 << 4,
            print  = 1 << 1,
            punct  = 1 << 7,
            space  = 1 << 0,
            upper  = 1 << 3,
            xdigit = 1 << 8,
            alnum  = alpha | digit,
            graph  = alnum | punct
        };

        typedef unsigned short mask;

        ctype_base(size_t _refs = 0)
        { }
};

}

#endif

namespace cxxtools {

std::ctype_base::mask ctypeMask(const Char& ch);

inline int isalpha(const Char& ch)
{
    return ctypeMask(ch) & std::ctype_base::alpha;
}

inline int isalnum(const Char& ch)
{
    return ctypeMask(ch) & std::ctype_base::alnum;
}

inline int ispunct(const Char& ch)
{
    return ctypeMask(ch) & std::ctype_base::punct;
}

inline int iscntrl(const Char& ch)
{
    return ctypeMask(ch) & std::ctype_base::cntrl;
}

inline int isdigit(const Char& ch)
{
    return ctypeMask(ch) & std::ctype_base::digit;
}

inline int isxdigit(const Char& ch)
{
    return ctypeMask(ch) & std::ctype_base::xdigit;
}

inline int isgraph(const Char& ch)
{
    return ctypeMask(ch) & std::ctype_base::graph;
}

inline int islower(const Char& ch)
{
    return ctypeMask(ch) & std::ctype_base::lower;
}

inline int isupper(const Char& ch)
{
    return ctypeMask(ch) & std::ctype_base::upper;
}

inline int isprint(const Char& ch)
{
    return ctypeMask(ch) & std::ctype_base::print;
}

inline int isspace(const Char& ch)
{
    return ctypeMask(ch) & std::ctype_base::space;
}

Char tolower(const Char& ch);

Char toupper(const Char& ch);

} // namespace cxxtools

#ifdef CXXTOOLS_WITH_STD_LOCALE
#include <cxxtools/facets.h>
#endif

#endif
