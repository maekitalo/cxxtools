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

#include <cxxtools/string.h>
#include <cxxtools/utf8codec.h>
#include <iostream>
#include <algorithm>

namespace std
{

const basic_string<cxxtools::Char>::size_type basic_string<cxxtools::Char>::npos;

#if __cplusplus >= 201103L
basic_string<cxxtools::Char>::basic_string(basic_string&& str)
    : _data(str._data)
{
    if (!isShortString())
    {
        str._data.u.shortdata[0] = 0;
        str._data.u.shortdata[_shortStringSize - 1] = _shortStringSize - 1;
    }
}

basic_string<cxxtools::Char>& basic_string<cxxtools::Char>::operator=(basic_string<cxxtools::Char>&& str)
{
    if (!isShortString())
    {
        _data.deallocate(longStringData(), longStringCapacity() + 1);
    }

    _data = str._data;

    if (!isShortString())
    {
        str._data.u.shortdata[0] = 0;
        str._data.u.shortdata[_shortStringSize - 1] = _shortStringSize - 1;
    }

    return *this;
}

#endif

void basic_string<cxxtools::Char>::resize(size_t n, cxxtools::Char ch)
{
    size_type size = this->size();
    if(size < n) {
        this->append(n - size, ch);
    }
    else if(n < size) {
        this->erase(n);
    }
}


void basic_string<cxxtools::Char>::reserve(size_t n)
{
    if (capacity() < n)
    {
        // since capacity is always at least shortStringCapacity, we need to use long string
        // to ensure the requested capacity if the current is not enough
        cxxtools::Char* p = reinterpret_cast<cxxtools::Char*>(_data.allocate(n + 1));
        size_type l = length();
        const cxxtools::Char* oldData = privdata_ro();
        traits_type::copy(p, oldData, l);

        if (isShortString())
            markLongString();
        else
            _data.deallocate(longStringData(), longStringCapacity() + 1);

        _data.u.ptr._begin = p;
        _data.u.ptr._end = p + l;
        _data.u.ptr._capacity = p + n;
        *_data.u.ptr._end = cxxtools::Char::null();
    }
}


void basic_string<cxxtools::Char>::privreserve(size_t n)
{
    if (capacity() < n)
    {
        size_type nn = 16;
        while (nn < n)
            nn += (nn >> 1);
        reserve(nn);
    }
}


void basic_string<cxxtools::Char>::swap(basic_string& str)
{
    if (isShortString())
    {
        if (str.isShortString())
        {
            for (unsigned nn = 0; nn < _shortStringSize; ++nn)
                std::swap(shortStringData()[nn], str.shortStringData()[nn]);
        }
        else
        {
            Ptr p = str._data.u.ptr;
            for (unsigned nn = 0; nn < _shortStringSize; ++nn)
                str.shortStringData()[nn] = shortStringData()[nn];
            markLongString();
            _data.u.ptr = p;
        }
    }
    else
    {
        if (str.isShortString())
        {
            Ptr p = _data.u.ptr;
            for (unsigned nn = 0; nn < _shortStringSize; ++nn)
                shortStringData()[nn] = str.shortStringData()[nn];
            str.markLongString();
            str._data.u.ptr = p;
        }
        else
        {
            std::swap(_data.u.ptr, str._data.u.ptr);
        }
    }
}



basic_string<cxxtools::Char>::size_type
basic_string<cxxtools::Char>::copy(cxxtools::Char* a, size_type n, size_type pos) const
{
    if( pos > this->size() ) {
        throw out_of_range("basic_string::copy");
    }

    if(n > this->size() - pos) {
        n = this->size() - pos;
    }

    traits_type::copy(a, privdata_ro() + pos, n);

    return n;
}


basic_string<cxxtools::Char>& basic_string<cxxtools::Char>::assign(const basic_string<cxxtools::Char>& str)
{
    // self-assignment check
    if (this == &str)
    {
        return *this;
    }

    privreserve(str.size());
    cxxtools::Char* p = privdata_rw();
    size_type l = str.length();
    traits_type::copy(p, str.data(), l);
    setLength(l);

    return *this;
}


basic_string<cxxtools::Char>& basic_string<cxxtools::Char>::assign(const std::string& str)
{
    size_type len = str.length();
    privreserve(len);

    cxxtools::Char* p = privdata_rw();
    for (size_type n = 0; n < len; ++n)
        p[n] = cxxtools::Char( str[n] );

    setLength(len);

    return *this;
}


basic_string<cxxtools::Char>& basic_string<cxxtools::Char>::assign(const std::string& str, size_type pos, size_type len)
{
    privreserve(len);

    cxxtools::Char* p = privdata_rw();
    for (size_type n = 0; n < len; ++n)
        p[n] = cxxtools::Char( str[pos + n] );

    setLength(len);

    return *this;
}


basic_string<cxxtools::Char>& basic_string<cxxtools::Char>::assign(const wchar_t* str)
{
    size_type length = 0;
    while (str[length])
        ++length;
    assign(str, length);

    return *this;
}


basic_string<cxxtools::Char>& basic_string<cxxtools::Char>::assign(const wchar_t* str, size_type length)
{
    privreserve(length);
    cxxtools::Char* d = privdata_rw();
    for (unsigned n = 0; n < length; ++n)
    {
        d[n] = str[n];
    }

    setLength(length);

    return *this;
}


basic_string<cxxtools::Char>& basic_string<cxxtools::Char>::assign(const char* str)
{
    size_type length = 0;
    while (str[length])
        ++length;
    assign(str, length);

    return *this;
}


basic_string<cxxtools::Char>& basic_string<cxxtools::Char>::assign(const char* str, size_type length)
{
    privreserve(length);
    cxxtools::Char* d = privdata_rw();
    for (unsigned n = 0; n < length; ++n)
    {
        d[n] = cxxtools::Char(str[n]);
    }

    setLength(length);

    return *this;
}


basic_string<cxxtools::Char>& basic_string<cxxtools::Char>::assign(const cxxtools::Char* str, size_type length)
{
    // self-assignment check
    if (str != privdata_ro())
    {
        privreserve(length);
        traits_type::copy(privdata_rw(), str, length);
    }

    setLength(length);

    return *this;
}


basic_string<cxxtools::Char>& basic_string<cxxtools::Char>::assign(size_type n, cxxtools::Char ch)
{
    privreserve(n);

    cxxtools::Char* p = privdata_rw();
    for (size_type nn = 0; nn < n; ++nn)
        p[nn] = ch;

    setLength(n);

    return *this;
}


basic_string<cxxtools::Char>& basic_string<cxxtools::Char>::append(const cxxtools::Char* str, size_type n)
{
    size_type l = length();
    privreserve(l + n);
    traits_type::copy(privdata_rw() + l, str, n);
    setLength(l + n);

    return *this;
}


basic_string<cxxtools::Char>& basic_string<cxxtools::Char>::append(size_type n, cxxtools::Char ch)
{
    size_type l = length();
    privreserve(l + n);
    cxxtools::Char* p = privdata_rw();
    for (size_type nn = 0; nn < n; ++nn)
        p[l + nn] = ch;
    setLength(l + n);

    return *this;
}


basic_string<cxxtools::Char>& basic_string<cxxtools::Char>::insert(size_type pos, const cxxtools::Char* str, size_type n)
{
    size_type l = length();
    privreserve(l + n);
    cxxtools::Char* p = privdata_rw();
    traits_type::move(p + pos + n, p + pos, l - pos);
    traits_type::copy(p + pos, str, n);
    setLength(l + n);

    return *this;
}


basic_string<cxxtools::Char>& basic_string<cxxtools::Char>::insert(size_type pos, size_type n, cxxtools::Char ch)
{
    size_type l = length();
    privreserve(l + n);
    cxxtools::Char* p = privdata_rw();
    traits_type::move(p + pos + n, p + pos, l - pos);
    for (size_type nn = 0; nn < n; ++nn)
        p[pos + nn] = ch;
    setLength(l + n);

    return *this;
}


basic_string<cxxtools::Char>& basic_string<cxxtools::Char>::erase(size_type pos, size_type n)
{
    cxxtools::Char* p = privdata_rw();
    size_type l = length();
    if (n == npos || pos + n > l)
        n = l - pos;
    traits_type::move(p + pos, p + pos + n, l - pos - n);
    setLength(l - n);

    return *this;
}


basic_string<cxxtools::Char>& basic_string<cxxtools::Char>::replace(size_type pos, size_type n, const cxxtools::Char* str, size_type n2)
{
    cxxtools::Char* p;
    if (n != n2)
    {
        size_type l = length();
        privreserve(l - n + n2);
        p = privdata_rw();
        traits_type::move(p + pos + n2, p + pos + n, l - pos - n);
        setLength(l - n + n2);
    }
    else
    {
        p = privdata_rw();
    }

    traits_type::copy(p + pos, str, n2);
    return *this;
}


basic_string<cxxtools::Char>& basic_string<cxxtools::Char>::replace(size_type pos, size_type n, size_type n2, cxxtools::Char ch)
{
    cxxtools::Char* p;
    if (n != n2)
    {
        size_type l = length();
        privreserve(l - n + n2);
        p = privdata_rw();
        traits_type::move(p + pos + n2, p + pos + n, l - pos - n);
        setLength(l - n + n2);
    }
    else
    {
        p = privdata_rw();
    }

    for (size_type nn = 0; nn < n2; ++nn)
        p[pos + nn] = ch;

    return *this;
}


int basic_string<cxxtools::Char>::compare(const basic_string& str) const
{
    const size_type size = this->size();
    const size_type osize = str.size();
    size_type n = min(size , osize);

    const int result = traits_type::compare(privdata_ro(), str.privdata_ro(), n);

    // unlike real life, size only matters when the quality is equal
    if (result == 0) {
        return static_cast<int>(size - osize);
    }

    return result;
}


int basic_string<cxxtools::Char>::compare(const char* str) const
{
    size_type size = length();
    size_type n;
    const cxxtools::Char* p = privdata_ro();
    for (n = 0; n < size && str[n]; ++n)
    {
        cxxtools::Char ch(str[n]);
        if (p[n] != ch)
            return p[n] > ch ? 1 : -1;
    }

    return n < size ? 1 : str[n] ? -1 : 0;
}


int basic_string<cxxtools::Char>::compare(const char* str, size_type len) const
{
    size_type size = length();
    size_type n;
    const cxxtools::Char* p = privdata_ro();
    for (n = 0; n < size && n < len; ++n)
    {
        cxxtools::Char ch(str[n]);
        if (p[n] != ch)
            return p[n] > ch ? 1 : -1;
    }

    return n < size ? 1 : n < len ? -1 : 0;
}


int basic_string<cxxtools::Char>::compare(const cxxtools::Char* str, size_type osize) const
{
    const size_type size = this->size();
    size_type n = min(size , osize);

    const int result = traits_type::compare(privdata_ro(), str, n);

    // unlike real life, size only matters when the quality is equal
    if (result == 0) {
        return size > osize ? 1 : size < osize ? -1 : 0;
    }

    return result;
}


int basic_string<cxxtools::Char>::compare(const wchar_t* str) const
{
    const cxxtools::Char* self = privdata_ro();
    size_type size = this->size();
    size_type n;
    for (n = 0; n < size && str[n] != L'\0'; ++n)
    {
        if ( self[n] != str[n] )
            return traits_type::lt(self[n], cxxtools::Char(str[n])) ? -1 : 1;
    }

    return n < size        ? 1
         : str[n] != L'\0' ? -1
         : 0;
}


int basic_string<cxxtools::Char>::compare(const wchar_t* str, size_type n) const
{
    const cxxtools::Char* self = privdata_ro();
    size_type nn;
    size_type size = this->size();
    for (nn = 0; nn < size && nn < n; ++nn)
    {
        if (self[nn] != str[nn])
            return traits_type::lt(self[nn], cxxtools::Char(str[nn])) ? -1 : 1;
    }

    return size < n ? -1 : 0;
}


int basic_string<cxxtools::Char>::compare(size_type pos, size_type n, const cxxtools::Char* str, size_type n2) const
{
    const size_type size = n;
    const size_type osize = n2;
    size_type len = min(size , osize);

    const int result = traits_type::compare(privdata_ro() + pos,
                                            str,
                                            len);

    // unlike real life, size only matters when the quality is equal
    if (result == 0) {
        return size < osize ? -1 : size > osize ? 1 : 0;
    }

    return result;
}


basic_string<cxxtools::Char>::size_type
basic_string<cxxtools::Char>::find(const cxxtools::Char* token, size_type pos, size_type n) const
{
    const size_type size = this->size();
    const cxxtools::Char* str = privdata_ro();

    for( ; pos + n <= size; ++pos) {
        if( 0 == traits_type::compare( str + pos, token, n ) ) {
            return pos;
        }
    }

    return npos;
}


basic_string<cxxtools::Char>::size_type
basic_string<cxxtools::Char>::find(cxxtools::Char ch, size_type pos) const
{
    const size_type size = this->size();
    if(pos > size) {
        return npos;
    }

    const cxxtools::Char* str = privdata_ro();
    const size_type n = size - pos;

    const cxxtools::Char* found = traits_type::find(str + pos, n, ch);
    if(found) {
        return found - str;
    }

    return npos;
}


basic_string<cxxtools::Char>::size_type
basic_string<cxxtools::Char>::rfind(const cxxtools::Char* token, size_type pos, size_type n) const
{
    // FIXME: check length
    const size_type size = this->size();

    if (n > size) {
        return npos;
    }

    pos = min(size_type(size - n), pos);
    const cxxtools::Char* str = privdata_ro();
    do {
        if (traits_type::compare(str + pos, token, n) == 0)
        return pos;
    }
    while (pos-- > 0);

    return npos;
}


basic_string<cxxtools::Char>::size_type
basic_string<cxxtools::Char>::rfind(cxxtools::Char ch, size_type pos) const
{
    const cxxtools::Char* str = privdata_ro();
    size_type size = this->size();

    if(size == 0)
        return npos;

    if(--size > pos)
        size = pos;

    for(++size; size-- > 0; ) {
        if( traits_type::eq(str[size], ch) )
            return size;
    }

    return npos;
}

basic_string<cxxtools::Char>::size_type
basic_string<cxxtools::Char>::find_first_of(const cxxtools::Char* s, size_type pos, size_type n) const
{
    // check length os s against n
    const cxxtools::Char* str = privdata_ro();
    const size_type size = this->size();

    for (; n && pos < size; ++pos) {
        if( traits_type::find(s, n, str[pos]) )
            return pos;
    }

    return npos;
}

basic_string<cxxtools::Char>::size_type
basic_string<cxxtools::Char>::find_last_of(const cxxtools::Char* s, size_type pos, size_type n) const
{
    // check length os s against n
    size_type size = this->size();
    const cxxtools::Char* str = privdata_ro();

    if (size == 0 ||  n == 0) {
        return npos;
    }

    if (--size > pos) {
        size = pos;
    }

    do {
        if( traits_type::find(s, n, str[size]) )
            return size;
    }
    while (size-- != 0);


    return npos;
}

basic_string<cxxtools::Char>::size_type
basic_string<cxxtools::Char>::find_first_not_of(const cxxtools::Char* tok, size_type pos, size_type n) const
{
    const cxxtools::Char* str = privdata_ro();

    for (; pos < this->size(); ++pos) {
        if ( !traits_type::find(tok, n, str[pos]) )
            return pos;
    }
    return npos;
}

basic_string<cxxtools::Char>::size_type
basic_string<cxxtools::Char>::find_first_not_of(const cxxtools::Char ch, size_type pos) const
{
    const cxxtools::Char* str = privdata_ro();

    for (; pos < this->size(); ++pos) {
        if ( !traits_type::eq(str[pos], ch) ) {
            return pos;
        }
    }

    return npos;
}


basic_string<cxxtools::Char>::size_type
basic_string<cxxtools::Char>::find_last_not_of(const cxxtools::Char* tok, size_type pos, size_type n) const
{
    size_type size = this->size();
    const cxxtools::Char* str = privdata_ro();

    if(size) {
        if (--size > pos)
            size = pos;

        do {
            if ( !traits_type::find(tok, n, str[size]) ) {
                return size;
            }
        }
        while(size--);
    }

    return npos;
}

basic_string<cxxtools::Char>::size_type
basic_string<cxxtools::Char>::find_last_not_of(cxxtools::Char ch, size_type pos) const
{
    size_type size = this->size();
    const cxxtools::Char* str = privdata_ro();

    if (size) {
        if (--size > pos)
            size = pos;
        do {
            if( !traits_type::eq(str[size], ch) ) {
                return size;
            }
        } while (size--);
    }

    return npos;
}


std::string basic_string<cxxtools::Char>::narrow(char dfault) const
{
    std::string ret;
    size_type len = this->length();
    const cxxtools::Char* s = privdata_ro();

    ret.reserve(len);

    for (size_type n = 0; n < len; ++n)
        ret.append( 1, s[n].narrow(dfault) );

    return ret;
}


basic_string<cxxtools::Char> basic_string<cxxtools::Char>::widen(const char* str)
{
    std::basic_string<cxxtools::Char> ret;

    size_type len = std::char_traits<char>::length(str);
    ret.privreserve(len);

    for (size_type n = 0; n < len; ++n)
        ret += cxxtools::Char( str[n] );

    return ret;
}


basic_string<cxxtools::Char> basic_string<cxxtools::Char>::widen(const std::string& str)
{
    std::basic_string<cxxtools::Char> ret;

    size_type len = str.length();
    ret.privreserve(len);

    for (size_type n = 0; n < len; ++n)
        ret += cxxtools::Char( str[n] );

    return ret;
}


ostream& operator<< (ostream& out, const basic_string<cxxtools::Char>& str)
{
    cxxtools::Utf8Codec codec;
    char to[64];
    cxxtools::MBState state;

    cxxtools::Utf8Codec::result r;
    const cxxtools::Char* from = str.data();
    cxxtools::String::size_type size = str.size();

    do{
        const cxxtools::Char* from_next;

        char* to_next = to;
        r = codec.out(state, from, from + size, from_next, to, to + sizeof(to), to_next);

        if (r == cxxtools::Utf8Codec::error)
        {
            out.setstate(std::ios::failbit);
            break;
        }

        out.write(to, to_next - to);

        size -= (from_next - from);
        from = from_next;

    } while (out.good() && r == cxxtools::Utf8Codec::partial);

    return out;
}


}

