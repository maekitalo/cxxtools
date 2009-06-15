/*
 * Copyright (C) 2004-2007 Marc Boris Duerner
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
#include <algorithm>

namespace cxxtools {

inline StringData::StringData( const allocator_type& a )
: _str(0), _length(0), _capacity(0), _allocator(a), _n(1)
{
    _str = _allocator.allocate(1, _str);
    _str[0] = cxxtools::Char::null();
}


inline StringData::StringData(const cxxtools::Char* s, size_type length, const allocator_type& a)
: _str(0), _length(0), _capacity(0), _allocator(a), _n(1)
{
    _str = _allocator.allocate(length + 1, _str);
    _capacity = length;

    traits_type::copy(_str, s, length);
    _str[length] = cxxtools::Char::null();
    _length = length;
}


inline StringData::StringData(const wchar_t* wstr, size_type length, const allocator_type& a)
: _str(0), _length(0), _capacity(0), _allocator(a), _n(1)
{
    _str = _allocator.allocate(length + 1, _str);
    _str[length] = cxxtools::Char::null();
    _capacity = length;
    _length = length;

    Char* str = _str;
    for(size_t n = 0; n < length; ++n)
    {
        *str = *wstr;
        ++wstr;
        ++str;
    }
}


inline StringData::StringData(size_type length, cxxtools::Char ch)
: _str(0), _length(0), _capacity(0), _allocator(), _n(1)
{
    _str = _allocator.allocate(length + 1, _str);
    _capacity = length;

    traits_type::assign(_str, length, ch);
    _str[length] = cxxtools::Char::null();
    _length = length;
}


inline StringData::~StringData()
{
    _allocator.deallocate(_str, _capacity + 1 );
}


inline StringData::atomic_type StringData::refs() const
{
    return atomicGet(_n);
}


inline StringData::atomic_type StringData::ref()
{
    return atomicIncrement(_n);
}


inline StringData::atomic_type StringData::unref()
{
    return atomicDecrement(_n);
}


inline cxxtools::Char* StringData::str()
{
    return _str;
}


inline cxxtools::Char* StringData::end()
{
    return _str + _length;
}


inline StringData::size_type StringData::length() const
{
    return _length;
}


inline StringData::size_type StringData::capacity() const
{
    return _capacity;
}


inline void StringData::clear()
{
    _str[0] = cxxtools::Char::null();
    _length = 0;
}


inline void StringData::assign(const cxxtools::Char* s, size_type length)
{
    this->allocate(length);

    traits_type::copy(_str, s, length);

    _str[length] = cxxtools::Char::null();
    _length = length;
}


inline void StringData::assign(size_type length, cxxtools::Char ch)
{
    this->allocate(length);

    traits_type::assign(_str, length, ch);

    _str[length] = cxxtools::Char::null();
    _length = length;
}


inline void StringData::append(size_type n, cxxtools::Char ch)
{
    this->reserve( _length + n + 1);

    traits_type::assign( _str + _length, n, ch);

    _length += n;
    _str[_length] = cxxtools::Char::null();
}


inline void StringData::append(const cxxtools::Char* str, size_type n)
{
    this->reserve( _length + n + 1);

    traits_type::copy( _str + _length, str, n);

    _length += n;
    _str[_length] = cxxtools::Char::null();
}


inline void StringData::insert(size_type pos, const cxxtools::Char* str, size_type n)
{
    this->reserve( _length + n );

    traits_type::move(_str + pos + n,
                      _str + pos,
                      static_cast<std::char_traits<cxxtools::Char>::int_type>(_length - pos));

    traits_type::copy(_str + pos, str, n);

    _length += n;
    _str[_length] = cxxtools::Char::null();
}


inline void StringData::insert(size_type pos, size_type n, cxxtools::Char ch)
{
    this->reserve( _length + n );

    traits_type::move(_str + pos + n,
                      _str + pos,
                      static_cast<std::char_traits<cxxtools::Char>::int_type>(_length - pos));

    traits_type::assign(_str + pos, n, ch);

    _length += n;
    _str[_length] = cxxtools::Char::null();
}


// better make size_t pos based
inline StringData::value_type* StringData::erase(value_type* pos, size_type n)
{
    const size_type rpos = pos - _str;

    traits_type::move(pos,
                      pos + n,
                      static_cast<std::char_traits<cxxtools::Char>::int_type>(_length - n - rpos));

    _length -= n;
    _str[_length] = cxxtools::Char::null();

    return _str + rpos;
}


inline void StringData::replace(size_type pos, size_type n, const cxxtools::Char* str, size_type n2)
{
    this->reserve( _length - n + n2);

    if( n2 > n ) {
        traits_type::move(_str + pos + n2,
                          _str + pos + n,
                          static_cast<std::char_traits<cxxtools::Char>::int_type>(_length - pos - n));
        //cerr << "moved a: " << (_length - pos - n) << endl;
    }
    else if(n2 < n) {
        traits_type::move(_str + pos + n2,
                          _str + pos + n,
                          static_cast<std::char_traits<cxxtools::Char>::int_type>(_length - pos - n));
        //cerr << "moved b: " << (_length - pos - n2) << endl;
    }

    traits_type::copy(_str + pos, str, n2);

    _length += (n2 - n);
    _str[_length] = cxxtools::Char::null();
}


inline void StringData::replace(size_type pos, size_type n, size_type n2, cxxtools::Char ch)
{
    this->reserve( _length - n + n2);

    if( n2 > n ) {
        traits_type::move(_str + pos + n2,
                          _str + pos + n,
                          static_cast<std::char_traits<cxxtools::Char>::int_type>(_length - pos - n));
        //cerr << "moved a: " << (_length - pos - n) << endl;
    }
    else if(n2 < n) {
        traits_type::move(_str + pos + n2,
                          _str + pos + n,
                          static_cast<std::char_traits<cxxtools::Char>::int_type>(_length - pos - n));
        //cerr << "moved b: " << (_length - pos - n2) << endl;
    }

    traits_type::assign(_str+ pos, n2, ch);

    _length += (n2 - n);
    _str[_length] = cxxtools::Char::null();
}


inline void StringData::reserve(size_type n)
{
    // only resize if necessary
    if(n <= _capacity)
        return;

    n += 16;

    // make room for new string AND NULL TERMINATION CHAR
    cxxtools::Char* newStr = _allocator.allocate(n + 1, _str);

    // backup old string
    traits_type::copy(newStr, _str, _length);

    // dealloc old string
    if(_str) {
        _allocator.deallocate(_str, _capacity + 1 );
    }

    _str = newStr;
    _capacity = n;
}


inline void StringData::allocate(size_type n)
{
    if(n <= _capacity)
        return;

    if(_str) {
        _allocator.deallocate(_str, _capacity + 1 );
    }

    _str = _allocator.allocate(n + 1, _str);
    _capacity = n;
}

} // namespace cxxtools

