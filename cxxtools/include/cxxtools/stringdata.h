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
#ifndef CXXTOOLS_STRINGDATA_H
#define CXXTOOLS_STRINGDATA_H

#include <cxxtools/api.h>
#include <cxxtools/atomicity.h>
#include <cxxtools/char.h>
#include <string>
#include <iterator>
#include <algorithm>

namespace std {

    template<>
    class allocator<cxxtools::Char>
    {
        public:
            typedef size_t          size_type;
            typedef ptrdiff_t       difference_type;
            typedef cxxtools::Char*       pointer;
            typedef const cxxtools::Char* const_pointer;
            typedef cxxtools::Char&       reference;
            typedef const cxxtools::Char& const_reference;
            typedef cxxtools::Char        value_type;

            template<typename U>
            struct rebind
            { typedef allocator<U> other; };

            allocator()
            { }

            template<typename U>
            allocator(const allocator<U>&)
            { }

            ~allocator()
            { }

            pointer address(reference x) const
            { return &x; }

            const_pointer address(const_reference x) const
            { return &x; }

            pointer allocate(size_type n, const void* = 0)
            {
                return static_cast<value_type*>(::operator new(n * sizeof(value_type)));
            }

            void deallocate(pointer p, size_type)
            { ::operator delete(p); }

            size_type max_size() const throw()
            { return size_t(-1) / sizeof(value_type); }

            void construct(pointer p, const value_type& val)
            { ::new(p) value_type(val); }

            void destroy(pointer p)
            { p->~value_type(); }
    };

} // namespace std

namespace cxxtools {

/** @internal
*/
class StringData {
    public:
        typedef atomic_t atomic_type;

        typedef size_t size_type;
        typedef cxxtools::Char value_type;
        typedef std::char_traits<cxxtools::Char> traits_type;
        typedef std::allocator<cxxtools::Char> allocator_type;

    public:
        StringData(const allocator_type& a = allocator_type());

        StringData(const cxxtools::Char* s, size_type length, const allocator_type& a = allocator_type());

        StringData(const wchar_t* wstr, size_type length, const allocator_type& a);

        StringData(size_type length, cxxtools::Char ch);

        ~StringData();

        allocator_type get_allocator() const
        { return _allocator; }

        atomic_type refs() const;

        atomic_type ref();

        atomic_type unref();

        void setInitial()
        { atomicSet(_n, atomic_type(1)); }

        /** @brief Check if in busy state

            Busy mode means that a modifying action that reuires more than one
            calls by the caller was started. This is the case if a non-const
            iterator was aquired or the index operator is used. Busy mode will
            be ended if the client calls a modifying action which would also
            invalidate an iterator even if the class was not shared.
        */
        bool busy() const
        { return refs() == atomic_type(-1); }

        /** @brief Enter busy state

            For example a mutating iterator is aquired, so we can not allow any
            data sharing until the caller calls a modifying method that would
            normally invalidate any iterator as well.
        */
        void setBusy()
        { atomicSet(_n, atomic_type(-1)); }

        bool shared() const
        {
            atomic_t n = refs();
            return (n > 1) && ( n != atomic_type(-1) );
        }

        cxxtools::Char* str();

        cxxtools::Char* end();

        size_type length() const;

        size_type capacity() const;

        void clear();

        void assign(const cxxtools::Char* s, size_type length);

        void assign(size_type length, cxxtools::Char ch);

        void append(size_type n, cxxtools::Char ch);

        void append(const cxxtools::Char* str, size_type n);

        void insert(size_type pos, const cxxtools::Char* str, size_type n);

        void insert(size_type pos, size_type n, cxxtools::Char ch);

        value_type* erase(value_type* pos, size_type n);

        void replace(size_type pos, size_type n, const cxxtools::Char* str, size_type n2);

        void replace(size_type pos, size_type n, size_type n2, cxxtools::Char ch);

        void reserve(size_type length);

        static StringData& emptyInstance()
        {
            static StringData empty;
            return empty;
        }

    protected:
        void allocate(size_type length);


    public:
        // the allocated size is the capacity plus 1
        cxxtools::Char* _str;
        size_type _length;
        size_type _capacity;
        allocator_type _allocator;
        volatile mutable atomic_type _n;
};

//! @internal
static struct StringStaticInitializer
{
    StringStaticInitializer()
    {
        StringData::emptyInstance();
    }
} pt_string_static_initializer;

} // namespace cxxtools

// Include the implementation header
#include <cxxtools/stringdata.tpp>

#endif
