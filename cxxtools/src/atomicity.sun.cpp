/*
 * Copyright (C) 2006 by PTV AG
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

#include <cxxtools/atomicity.sun.h>
#include <sys/types.h>
#include <sys/atomic.h>

namespace cxxtools {

typedef long atomic_t;


atomic_t atomicIncrement(volatile atomic_t& value)
{
    volatile ulong_t* uvalue = reinterpret_cast<volatile ulong_t*>(&value);

    return atomic_inc_ulong_nv( uvalue );
}


atomic_t atomicDecrement(volatile atomic_t& value)
{
    volatile ulong_t* uvalue = reinterpret_cast<volatile ulong_t*>(&value);

    return atomic_dec_ulong_nv( uvalue);
}


atomic_t atomicExchangeAdd(volatile atomic_t& value, atomic_t n)
{
    volatile ulong_t* uvalue = reinterpret_cast<volatile ulong_t*>(&value);
    volatile ulong_t& un = reinterpret_cast<volatile ulong_t&>(n);

    volatile atomic_t result = atomic_add_long_nv(uvalue, un);
    return result - n;
}


atomic_t atomicExchange(volatile atomic_t& value, atomic_t newval)
{
    volatile ulong_t* uvalue = reinterpret_cast<volatile ulong_t*>(&value);
    volatile ulong_t& unewval = reinterpret_cast<volatile ulong_t&>(newval);

    return atomic_swap_ulong(uvalue, unewval);
}


void* atomicExchange(void* volatile& ptr, void* new_val)
{
    return atomic_swap_ptr(&ptr, new_val);
}


atomic_t atomicCompareExchange(volatile atomic_t& value, atomic_t ex, atomic_t cmp)
{
    volatile ulong_t* uvalue = reinterpret_cast<volatile ulong_t*>(&value);
    volatile ulong_t& uex = reinterpret_cast<volatile ulong_t&>(ex);
    volatile ulong_t& ucmp = reinterpret_cast<volatile ulong_t&>(cmp);

    return atomic_cas_ulong(uvalue, ucmp, uex);
}


void* atomicCompareExchange(void* volatile& ptr, void* ex, void* cmp)
{
    return atomic_cas_ptr(&ptr, cmp, ex);
}

} // namespace cxxtools
