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

namespace std {

inline basic_string<cxxtools::Char>::basic_string()
: _data( &cxxtools::StringData::emptyInstance() )
{
    _data->ref();
}


inline basic_string<cxxtools::Char>::basic_string(const allocator_type& a)
: _data( new cxxtools::StringData(a) )
{
}


inline basic_string<cxxtools::Char>::basic_string(const cxxtools::Char* str, const allocator_type& a)
: _data( new cxxtools::StringData( str, char_traits<cxxtools::Char>::length(str), a ) )
{
}


inline basic_string<cxxtools::Char>::basic_string(const wchar_t* str, const allocator_type& a)
: _data( new cxxtools::StringData( str, char_traits<wchar_t>::length(str), a ) )
{
}


inline basic_string<cxxtools::Char>::basic_string(const wchar_t* wstr, size_type length, const allocator_type& a)
: _data( new cxxtools::StringData( wstr, length, a ) )
{
}


inline basic_string<cxxtools::Char>::basic_string(const cxxtools::Char* str, size_type n, const allocator_type& a)
: _data( new cxxtools::StringData( str, n, a ) )
{
}


inline basic_string<cxxtools::Char>::basic_string(size_type n, cxxtools::Char c)
: _data( new cxxtools::StringData( n, c ) )
{
}


inline basic_string<cxxtools::Char>::basic_string(const basic_string& str, size_type pos)
: _data( new cxxtools::StringData( str._data->str() + pos, str._data->length() - pos ) )
{
}


inline basic_string<cxxtools::Char>::basic_string(const basic_string& str, size_type pos, size_type n)
: _data( new cxxtools::StringData( str._data->str() + pos, n ) )
{
}


inline basic_string<cxxtools::Char>::basic_string(const basic_string& str, size_type pos, size_type n, const allocator_type& a)
: _data( new cxxtools::StringData( str._data->str() + pos, n, a ) )
{
}

inline basic_string<cxxtools::Char>::basic_string(const cxxtools::Char* begin, const cxxtools::Char* end)
: _data( new cxxtools::StringData( begin, end - begin ) )
{
}


inline basic_string<cxxtools::Char>::const_iterator basic_string<cxxtools::Char>::begin() const
{
    return _data->str();
}


inline basic_string<cxxtools::Char>::const_iterator basic_string<cxxtools::Char>::end() const
{
    return _data->end();
}


inline basic_string<cxxtools::Char>::size_type basic_string<cxxtools::Char>::length() const
{
    return _data->length();
}


inline basic_string<cxxtools::Char>::size_type basic_string<cxxtools::Char>::size() const
{
    return _data->length();
}


inline bool basic_string<cxxtools::Char>::empty() const
{
    return _data->length() == 0;
}


inline basic_string<cxxtools::Char>::size_type basic_string<cxxtools::Char>::max_size() const
{
    return ( size_type(-1) / sizeof(cxxtools::Char) ) - 1;
}


inline basic_string<cxxtools::Char>::size_type basic_string<cxxtools::Char>::capacity() const
{
    return _data->capacity();
}


inline const cxxtools::Char* basic_string<cxxtools::Char>::c_str() const
{
    return _data->str();
}


inline basic_string<cxxtools::Char>& basic_string<cxxtools::Char>::assign(const basic_string<cxxtools::Char>& str, size_type pos, size_type n)
{
    this->assign( str._data->str() + pos, n );   
    return *this;
}


inline basic_string<cxxtools::Char>& basic_string<cxxtools::Char>::append(const basic_string& str)
{
    return this->append( str._data->str(), str._data->length() );
}


inline basic_string<cxxtools::Char>& basic_string<cxxtools::Char>::append(const basic_string& str, size_type pos, size_type n)
{
    return this->append( str._data->str() + pos, n );
}


inline basic_string<cxxtools::Char>& basic_string<cxxtools::Char>::append(const cxxtools::Char* begin, const cxxtools::Char* end)
{
    return this->append( begin, end-begin );
}


inline basic_string<cxxtools::Char>& basic_string<cxxtools::Char>::insert(size_type pos, const cxxtools::Char* str)
{
    return this->insert( pos, str, traits_type::length(str) );
}


inline basic_string<cxxtools::Char>& basic_string<cxxtools::Char>::insert(iterator p, cxxtools::Char ch)
{
    return this->insert(p, 1, ch);
}

inline
basic_string<cxxtools::Char>::size_type
basic_string<cxxtools::Char>::find(const cxxtools::Char* str, size_type pos) const
{
    return this->find( str, pos, traits_type::length(str) );
}


inline
basic_string<cxxtools::Char>::size_type
basic_string<cxxtools::Char>::rfind(const cxxtools::Char* str, size_type pos) const
{
    return this->rfind( str, pos, traits_type::length(str) );
}


}

