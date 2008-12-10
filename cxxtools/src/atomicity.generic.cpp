/*
 * Copyright (C) 2006-2007 by Marc Boris Duerner
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

#include <cxxtools/atomicity.generic.h>
#include <csignal>

namespace cxxtools {


atomic_t atomicGet(volatile atomic_t& val)
{
    return val;
}


void atomicSet(volatile atomic_t& val, atomic_t n)
{
    val = n;
}


atomic_t atomicIncrement(volatile atomic_t& dest)
{
    return dest++;
}


atomic_t atomicDecrement(volatile atomic_t& dest)
{
    return dest--;
}


atomic_t atomicCompareExchange(volatile atomic_t& dest, atomic_t exch, atomic_t comp)
{
    atomic_t tmp = dest;

    if(dest== comp)
        dest = exch;

    return tmp;
}


void* atomicCompareExchange(void* volatile& dest, void* exch, void* comp)
{
    volatile void* tmp = dest;

    if(dest== comp)
        dest = exch;

    return tmp;
}


atomic_t atomicExchange(volatile atomic_t& dest, atomic_t exch)
{
    atomic_t tmp = dest;
    dest = exch;
    return tmp;
}


void* atomicExchange(void* volatile& dest, void* exch)
{
    volatile void* tmp = dest;
    dest = exch;
    return tmp;
}


atomic_t atomicExchangeAdd(volatile atomic_t& dest, atomic_t add)
{
    atomic_t tmp = dest;
    dest += add;
    return tmp;
}

} // namespace cxxtools
