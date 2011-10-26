/*
 * Copyright (C) 2004-2007 Marc Boris Duerner
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

#ifndef CXXTOOLS_STRING_H
#define CXXTOOLS_STRING_H

#include <cxxtools/api.h>
#include <cxxtools/char.h>

#include <string>
#include <iterator>
#include <stdexcept>

#include <cxxtools/config.h>

namespace std {

/** @brief Unicode capable strings
    @ingroup Unicode
*/
template <>
class CXXTOOLS_API basic_string< cxxtools::Char > {
    public:
        typedef cxxtools::Char value_type;
        typedef size_t size_type;
        typedef char_traits< cxxtools::Char > traits_type;
        typedef std::allocator<cxxtools::Char> allocator_type;
        typedef allocator_type::difference_type difference_type;
        typedef allocator_type::reference reference;
        typedef allocator_type::const_reference const_reference;
        typedef allocator_type::pointer pointer;
        typedef allocator_type::const_pointer const_pointer;
        typedef value_type* iterator;
        typedef const value_type* const_iterator;

#if defined(HAVE_REVERSE_ITERATOR)
        typedef std::reverse_iterator<iterator> reverse_iterator;
        typedef const std::reverse_iterator<const_iterator> const_reverse_iterator;
#       define HAVE_STRING_REVERSE_ITERATOR
#elif defined(HAVE_REVERSE_ITERATOR_4)
        typedef std::reverse_iterator<iterator, difference_type, value_type, pointer, reference> reverse_iterator;
        typedef std::reverse_iterator<const_iterator, difference_type, value_type, pointer, reference> const_reverse_iterator;
#       define HAVE_STRING_REVERSE_ITERATOR
#endif

        static const size_type npos = static_cast<size_type>(-1);

    public:
        explicit basic_string( const allocator_type& a = allocator_type());

        basic_string(const cxxtools::Char* str, const allocator_type& a = allocator_type());

        basic_string(const wchar_t* str, const allocator_type& a = allocator_type());

        basic_string(const wchar_t* str, size_type n, const allocator_type& a = allocator_type());

        basic_string(const std::string& str, const allocator_type& a = allocator_type());

        basic_string(const char* str, const allocator_type& a = allocator_type());

        basic_string(const char* str, size_type n, const allocator_type& a = allocator_type());

        basic_string(const cxxtools::Char* str, size_type n, const allocator_type& a = allocator_type());

        basic_string(size_type n, cxxtools::Char c, const allocator_type& a = allocator_type());

        basic_string(const basic_string& str);

        basic_string(const basic_string& str, const allocator_type& a);

        basic_string(const basic_string& str, size_type pos, const allocator_type& a = allocator_type());

        basic_string(const basic_string& str, size_type pos, size_type n, const allocator_type& a = allocator_type());

        basic_string(const cxxtools::Char* begin, const cxxtools::Char* end, const allocator_type& a = allocator_type());

        template <typename InputIterator>
        basic_string(InputIterator begin, InputIterator end, const allocator_type& a = allocator_type());

        ~basic_string();

    public:
        iterator begin()
        { return privdata_rw(); }

        iterator end()
        { return privdata_rw() + length(); }

        const_iterator begin() const
        { return privdata_ro(); }

        const_iterator end() const
        { return privdata_ro() + length(); }

#ifdef HAVE_STRING_REVERSE_ITERATOR
        reverse_iterator rbegin()
        { return reverse_iterator( this->end() ); }

        reverse_iterator rend()
        { return reverse_iterator( this->begin() ); }

        const_reverse_iterator rbegin() const
        { return const_reverse_iterator( this->end() ); }

        const_reverse_iterator rend()   const
        { return const_reverse_iterator( this->begin() ); }
#endif

        reference operator[](size_type n)
        { return privdata_rw()[n]; }

        const_reference operator[](size_type n) const
        { return privdata_ro()[n]; }

        reference at(size_type n)
        { return privdata_rw()[n]; }

        const_reference at(size_type n) const
        { return privdata_ro()[n]; }

    public:
        void push_back(cxxtools::Char ch)
        { this->append(1, ch); }

        void resize( size_t n, cxxtools::Char ch = value_type() );

        void reserve(size_t n = 0);

        void swap(basic_string& str);

        allocator_type get_allocator() const
        { return _d; }

        size_type copy(cxxtools::Char* a, size_type n, size_type pos = 0) const;

        basic_string substr(size_type pos, size_type n) const
        { return basic_string(*this, pos, n); }

        basic_string substr(size_type pos = 0) const
        { return basic_string(*this, pos); }

    public:
        size_type length() const
        { return isShortString() ? shortStringLength() : longStringLength(); }

        size_type size() const
        { return length(); }

        bool empty() const
        { return length() == 0; }

        size_type max_size() const
        { return ( size_type(-1) / sizeof(cxxtools::Char) ) - 1; }

        size_type capacity() const
        { return isShortString() ? shortStringCapacity() : longStringCapacity(); }

        const cxxtools::Char* data() const
        { return privdata_ro(); }

        const cxxtools::Char* c_str() const
        { return privdata_ro(); }

        basic_string& assign(const basic_string& str);

        basic_string& assign(const basic_string& str, size_type pos, size_type n);

        basic_string& assign(const string& str);

        basic_string& assign(const string& str, size_type pos, size_type n);

        basic_string& assign(const wchar_t* str);

        basic_string& assign(const wchar_t* str, size_type n);

        basic_string& assign(const cxxtools::Char* str)
        { return assign(str, traits_type::length(str)); }

        basic_string& assign(const cxxtools::Char* str, size_type length);

        basic_string& assign(const char* str);

        basic_string& assign(const char* str, size_type length);

        basic_string& assign(size_type n, cxxtools::Char c);

        template <typename InputIterator>
        basic_string& assign(InputIterator begin, InputIterator end);

        basic_string& append(const cxxtools::Char* str)
        { return append( str, traits_type::length(str) ); }

        basic_string& append(const cxxtools::Char* str, size_type n);

        basic_string& append(size_type n, cxxtools::Char ch);

        basic_string& append(const basic_string& str);

        basic_string& append(const basic_string& str, size_type pos, size_type n);

        template <typename InputIterator>
        basic_string& append(InputIterator begin, InputIterator end);

        basic_string& append(const cxxtools::Char* begin, const cxxtools::Char* end);

        basic_string& insert(size_type pos, const cxxtools::Char* str)
        { return this->insert( pos, str, traits_type::length(str) ); }

        basic_string& insert(size_type pos, const cxxtools::Char* str, size_type n);

        basic_string& insert(size_type pos, size_type n, cxxtools::Char ch);

        basic_string& insert(size_type pos, const basic_string& str)
        { return insert(pos, str.privdata_ro(), str.length()); }

        basic_string& insert(size_type pos, const basic_string& str, size_type pos2, size_type n)
        { return insert(pos, str.privdata_ro() + pos2, n > str.length() ? str.length() : n); }

        basic_string& insert(iterator p, cxxtools::Char ch)
        { return insert(p - begin(), 1, ch); }

        basic_string& insert(iterator p, size_type n, cxxtools::Char ch)
        { return insert(p - begin(), n, ch); }

        // unimplemented
        //template <typename InputIterator>
        //basic_string& insert(iterator p, InputIterator first, InputIterator last);

        void clear()
        { setLength(0); }

        basic_string& erase(size_type pos = 0, size_type n = npos);

        iterator erase(iterator pos);

        iterator erase(iterator first, iterator last);

        basic_string& replace(size_type pos, size_type n, const cxxtools::Char* str);

        basic_string& replace(size_type pos, size_type n, const cxxtools::Char* str, size_type n2);

        basic_string& replace(size_type pos, size_type n, size_type n2, cxxtools::Char ch);

        basic_string& replace(size_type pos, size_type n, const basic_string& str);

        basic_string& replace(size_type pos, size_type n, const basic_string& str, size_type pos2, size_type n2);

        basic_string& replace(iterator i1, iterator i2, const cxxtools::Char* str);

        basic_string& replace(iterator i1, iterator i2, const cxxtools::Char* str, size_type n);

        basic_string& replace(iterator i1, iterator i2, size_type n, cxxtools::Char ch);

        basic_string& replace(iterator i1, iterator i2, const basic_string& str);

        //template<InputIterator>
        //basic_string& replace(iterator i1, iterator i2, InputIterator j1, InputIterator j2);

        int compare(const basic_string& str) const;

        int compare(const cxxtools::Char* str) const;

        int compare(const cxxtools::Char* str, size_type n) const;

        int compare(const wchar_t* str) const;

        int compare(const wchar_t* str, size_type n) const;

        int compare(const std::string& str) const
        { return compare(str.data(), str.length()); }

        int compare(const char* str) const;

        int compare(const char* str, size_type n) const;

        int compare(size_type pos, size_type n, const basic_string& str) const;

        int compare(size_type pos, size_type n, const basic_string& str, size_type pos2, size_type n2) const;

        int compare(size_type pos, size_type n, const cxxtools::Char* str) const;

        int compare(size_type pos, size_type n, const cxxtools::Char* str, size_type n2) const;

        size_type find(const basic_string& str, size_type pos = 0) const;

        size_type find(const cxxtools::Char* str, size_type pos, size_type n) const;

        size_type find(const cxxtools::Char* str, size_type pos = 0) const;
//
        size_type find(cxxtools::Char ch, size_type pos = 0) const;

        size_type rfind(const basic_string& str, size_type pos = npos) const;

        size_type rfind(const cxxtools::Char* str, size_type pos, size_type n) const;

        size_type rfind(const cxxtools::Char* str, size_type pos = npos) const;

        size_type rfind(cxxtools::Char ch, size_type pos = npos) const;

        size_type find_first_of(const basic_string& str, size_type pos = 0) const
        { return this->find_first_of( str.data(), pos, str.size() ); }

        size_type find_first_of(const cxxtools::Char* s, size_type pos, size_type n) const;

        size_type find_first_of(const cxxtools::Char* str, size_type pos = 0) const
        { return this->find_first_of( str, pos, traits_type::length(str) ); }

        size_type find_first_of(const cxxtools::Char ch, size_type pos = 0) const
        { return this->find(ch, pos); }

        size_type find_last_of(const basic_string& str, size_type pos = npos) const
        { return this->find_last_of( str.data(), pos, str.size() ); }

        size_type find_last_of(const cxxtools::Char* s, size_type pos, size_type n) const;

        size_type find_last_of(const cxxtools::Char* str, size_type pos = npos) const
        { return this->find_last_of( str, pos, traits_type::length(str) ); }

        size_type find_last_of(const cxxtools::Char ch, size_type pos = npos) const
        { return this->rfind(ch, pos); }

        size_type find_first_not_of(const basic_string& str, size_type pos = 0) const
        { return this->find_first_not_of( str.data(), pos, str.size() ); }

        size_type find_first_not_of(const cxxtools::Char* s, size_type pos, size_type n) const;

        size_type find_first_not_of(const cxxtools::Char* str, size_type pos = 0) const
        {
            // requires_string(str);
            return this->find_first_not_of( str, pos, traits_type::length(str) );
        }

        size_type find_first_not_of(const cxxtools::Char ch, size_type pos = 0) const;

        size_type find_last_not_of(const basic_string& str, size_type pos = npos) const
        { return this->find_last_not_of( str.data(), pos, str.size() ); }

        size_type find_last_not_of(const cxxtools::Char* tok, size_type pos, size_type n) const;

        size_type find_last_not_of(const cxxtools::Char* str, size_type pos = npos) const
        {
            //requires_string(s);
            return this->find_last_not_of( str, pos, traits_type::length(str) );
        }

        // untested
        size_type find_last_not_of(cxxtools::Char ch, size_type pos = npos) const;

    public:
        std::string narrow(char dfault = '?') const;

        static basic_string widen(const char* str);

        static basic_string widen(const std::string& str);

        template <typename OutIterT>
        OutIterT toUtf16(OutIterT to) const;

        template <typename InIterT>
        static basic_string fromUtf16(InIterT from, InIterT fromEnd);

    public:
        basic_string& operator=(const basic_string& str)
        { return this->assign(str); }

        basic_string& operator=(const string& str)
        { return this->assign(str); }

        basic_string& operator=(const char* str)
        { return this->assign(str); }

        basic_string& operator=(const cxxtools::Char* str)
        { return this->assign(str); }

        basic_string& operator=(cxxtools::Char c)
        { return this->assign(1, c); }

        basic_string& operator+=(const basic_string& str)
        { return this->append(str); }

        basic_string& operator+=(const cxxtools::Char* str)
        { return this->append(str); }

        basic_string& operator+=(cxxtools::Char c)
        { return this->append(1, c); }

    private:
        struct Ptr
        {
            cxxtools::Char* _begin;
            cxxtools::Char* _end;
            cxxtools::Char* _capacity;
        };

        static const unsigned _minN = (sizeof(Ptr) / sizeof(cxxtools::uint32_t)) + 1;
        static const unsigned _N = _minN < 7 ? 7 : _minN;

        struct Data : public allocator_type
        {
            Data(const allocator_type& a)
            : allocator_type(a)
            {
                _u._s[0] = 0;
                _u._s[_N - 1] = _N - 1;
            }

            union
            {
                Ptr _p;
                cxxtools::uint32_t _s[_N];
            } _u;

        } _d;

    private:
        const cxxtools::Char* privdata_ro() const
        { return isShortString() ? shortStringData() : longStringData(); }
        cxxtools::Char* privdata_rw()
        { return isShortString() ? shortStringData() : longStringData(); }

        bool isShortString() const                    { return shortStringMagic() != 0xffff; }
        void markLongString()                         { shortStringMagic() = 0xffff; }
        const cxxtools::Char* shortStringData() const { return reinterpret_cast<const cxxtools::Char*>(&_d._u._s[0]); }
        cxxtools::Char* shortStringData()             { return reinterpret_cast<cxxtools::Char*>(&_d._u._s[0]); }
        cxxtools::uint32_t  shortStringMagic() const  { return _d._u._s[_N - 1]; }
        cxxtools::uint32_t& shortStringMagic()        { return _d._u._s[_N - 1]; }
        size_type shortStringLength() const           { return _N - 1 - shortStringMagic(); }
        size_type shortStringCapacity() const         { return _N - 1; }
        void setShortStringLength(size_type n)        { shortStringData()[n] = cxxtools::Char(0); shortStringMagic() = _N - n - 1; }
        void shortStringAssign(const cxxtools::Char* str, size_type n)
        {
            traits_type::copy(shortStringData(), str, n);
            shortStringData()[n] = cxxtools::Char(0);
            shortStringMagic() = _N - n - 1;
        }
        void shortStringAssign(const wchar_t* str, size_type n)
        {
            for (size_type nn = 0; nn < n; ++nn)
                shortStringData()[nn] = str[nn];
            shortStringData()[n] = cxxtools::Char(0);
            shortStringMagic() = _N - n - 1;
        }

        const cxxtools::Char* longStringData() const    { return _d._u._p._begin; }
        cxxtools::Char* longStringData()                { return _d._u._p._begin; }
        size_type longStringLength() const              { return _d._u._p._end - _d._u._p._begin; }
        size_type longStringCapacity() const            { return _d._u._p._capacity - _d._u._p._begin; }
        void setLength(size_type n)
        {
            if (isShortString())
                setShortStringLength(n);
            else
            {
                _d._u._p._end = _d._u._p._begin + n;
                _d._u._p._begin[n] = cxxtools::Char::null();
            }
        }
    };

    inline basic_string<cxxtools::Char> operator+(const basic_string<cxxtools::Char>& a, const basic_string<cxxtools::Char>& b)
    { basic_string<cxxtools::Char> temp; temp += a; temp += b; return temp; }

    inline basic_string<cxxtools::Char> operator+(const basic_string<cxxtools::Char>& a, const cxxtools::Char* b)
    { basic_string<cxxtools::Char> temp; temp += a; temp += b; return temp; }

    inline basic_string<cxxtools::Char> operator+(const cxxtools::Char* a, const basic_string<cxxtools::Char>& b)
    { basic_string<cxxtools::Char> temp; temp += a; temp += b; return temp; }

    inline basic_string<cxxtools::Char> operator+(const basic_string<cxxtools::Char>& a, cxxtools::Char b)
    { basic_string<cxxtools::Char> temp; temp += a; temp += b; return temp; }

    inline basic_string<cxxtools::Char> operator+(cxxtools::Char a, const basic_string<cxxtools::Char>& b)
    { basic_string<cxxtools::Char> temp; temp += a; temp += b; return temp; }

    // operator ==
    inline bool operator==(const basic_string<cxxtools::Char>& a, const basic_string<cxxtools::Char>& b)
    { return a.compare(b) == 0; }

    inline bool operator==(const cxxtools::Char* a, const basic_string<cxxtools::Char>& b)
    { return b.compare(a) == 0; }

    inline bool operator==(const basic_string<cxxtools::Char>& a, const cxxtools::Char* b)
    { return a.compare(b) == 0; }

    inline bool operator==(const basic_string<cxxtools::Char>& a, const wchar_t* b)
    { return a.compare(b) == 0; }

    inline bool operator==(const wchar_t* b, const basic_string<cxxtools::Char>& a)
    { return a.compare(b) == 0; }

    inline bool operator==(const basic_string<cxxtools::Char>& a, const char* b)
    { return a.compare(b) == 0; }

    inline bool operator==(const char* b, const basic_string<cxxtools::Char>& a)
    { return a.compare(b) == 0; }

    inline bool operator==(const basic_string<cxxtools::Char>& a, const std::string& b)
    { return a.compare(b) == 0; }

    inline bool operator==(const std::string& b, const basic_string<cxxtools::Char>& a)
    { return a.compare(b) == 0; }

    // operator !=
    inline bool operator!=(const basic_string<cxxtools::Char>& a, const basic_string<cxxtools::Char>& b)
    { return a.compare(b) != 0; }

    inline bool operator!=(const cxxtools::Char* a, const basic_string<cxxtools::Char>& b)
    { return b.compare(a) != 0; }

    inline bool operator!=(const basic_string<cxxtools::Char>& a, const cxxtools::Char* b)
    { return a.compare(b) != 0; }

    inline bool operator!=(const basic_string<cxxtools::Char>& a, const wchar_t* b)
    { return a.compare(b) != 0; }

    // operator <
    inline bool operator<(const basic_string<cxxtools::Char>& a, const basic_string<cxxtools::Char>& b)
    { return a.compare(b) < 0; }

    inline bool operator<(const cxxtools::Char* a, const basic_string<cxxtools::Char>& b)
    { return b.compare(a) > 0; }

    inline bool operator<(const basic_string<cxxtools::Char>& a, const cxxtools::Char* b)
    { return a.compare(b) < 0; }

    inline bool operator<(const basic_string<cxxtools::Char>& a, const wchar_t* b)
    { return a.compare(b) < 0; }

    // operator >
    inline bool operator>(const basic_string<cxxtools::Char>& a, const basic_string<cxxtools::Char>& b)
    { return a.compare(b) > 0; }

    inline bool operator>(const cxxtools::Char* a, const basic_string<cxxtools::Char>& b)
    { return b.compare(a) < 0; }

    inline bool operator>(const basic_string<cxxtools::Char>& a, const cxxtools::Char* b)
    { return a.compare(b) > 0; }

    inline bool operator>(const basic_string<cxxtools::Char>& a, const wchar_t* b)
    { return a.compare(b) > 0; }

} // namespace std


namespace cxxtools {

    /** @brief Unicode capable strings
        @ingroup Unicode
    */
    typedef std::basic_string<cxxtools::Char> String;

}

// Include the implementation header
#include <cxxtools/string.tpp>

#endif
