/*
 * Copyright (C) 2006 by Marc Boris Duerner
 * Copyright (C) 2006 by Aloysius Indrayanto
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

#include <cxxtools/atomicity.windows.h>
#define _WINSOCKAPI_   /* Prevent inclusion of winsock.h in windows.h */
#include <windows.h>

namespace cxxtools {

typedef LONG atomic_t;


atomic_t atomicGet(volatile atomic_t& val)
{
#if ! defined(_WIN32_WCE) && (_MSC_VER >= 1400) && ! defined(__GNUC__)
    MemoryBarrier();
#endif

    return val;
}


void atomicSet(volatile atomic_t& val, atomic_t n)
{
    val = n;

#if ! defined(_WIN32_WCE) && (_MSC_VER >= 1400) && ! defined(__GNUC__)
    MemoryBarrier();
#endif
}


atomic_t atomicIncrement(volatile atomic_t& value)
{
    return InterlockedIncrement( const_cast<atomic_t*>(&value) );
}


atomic_t atomicDecrement(volatile atomic_t& value)
{
    return InterlockedDecrement( const_cast<atomic_t*>(&value) );
}


atomic_t atomicExchangeAdd(volatile atomic_t& value, atomic_t n)
{
    return InterlockedExchangeAdd(const_cast<atomic_t*>(&value), n);
}


atomic_t atomicExchange(volatile atomic_t& value, atomic_t new_val)
{
    return InterlockedExchange(const_cast<atomic_t*>(&value), new_val);
}


void* atomicExchange(void* volatile& ptr, void* new_val)
{
    return InterlockedExchangePointer( const_cast<void**>(&ptr), new_val );
}


atomic_t atomicCompareExchange(volatile atomic_t& value, atomic_t ex, atomic_t cmp)
{
    return InterlockedCompareExchange(const_cast<atomic_t*>(&value), ex, cmp);
}


void* atomicCompareExchange(void* volatile& ptr, void* ex, void* cmp)
{
    return InterlockedCompareExchangePointer(&ptr, ex, cmp);
}

} // namespace cxxtools
