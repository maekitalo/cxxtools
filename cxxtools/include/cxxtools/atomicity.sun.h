/***************************************************************************
 *   Copyright (C) 2006 by PTV AG                                          *
 *   Copyright (C) 2006 by Marc Boris Duerner                              *
 *   Copyright (C) 2006 by Aloysius Indrayanto                             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this program; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef CXXTOOLS_ATOMICITY_SUN_H
#define CXXTOOLS_ATOMICITY_SUN_H

#include <sys/types.h>
#include <sys/atomic.h>

namespace cxxtools {

typedef long atomic_t;


inline atomic_t atomicIncrement(volatile atomic_t& value)
{
    volatile ulong_t* uvalue = reinterpret_cast<volatile ulong_t*>(&value);

    return atomic_inc_ulong_nv( uvalue );
}


inline atomic_t atomicDecrement(volatile atomic_t& value)
{
    volatile ulong_t* uvalue = reinterpret_cast<volatile ulong_t*>(&value);

    return atomic_dec_ulong_nv( uvalue);
}


inline atomic_t atomicExchangeAdd(volatile atomic_t& value, atomic_t n)
{
    volatile ulong_t* uvalue = reinterpret_cast<volatile ulong_t*>(&value);
    volatile ulong_t& un = reinterpret_cast<volatile ulong_t&>(n);

    volatile atomic_t result = atomic_add_long_nv(uvalue, un);
    return result - n;
}


inline atomic_t atomicExchange(volatile atomic_t& value, atomic_t newval)
{
    volatile ulong_t* uvalue = reinterpret_cast<volatile ulong_t*>(&value);
    volatile ulong_t& unewval = reinterpret_cast<volatile ulong_t&>(newval);

    return atomic_swap_ulong(uvalue, unewval);
}


inline void* atomicExchange(void* volatile& ptr, void* new_val)
{
    return atomic_swap_ptr(&ptr, new_val);
}


inline atomic_t atomicCompareExchange(volatile atomic_t& value, atomic_t ex, atomic_t cmp)
{
    volatile ulong_t* uvalue = reinterpret_cast<volatile ulong_t*>(&value);
    volatile ulong_t& uex = reinterpret_cast<volatile ulong_t&>(ex);
    volatile ulong_t& ucmp = reinterpret_cast<volatile ulong_t&>(cmp);

    return atomic_cas_ulong(uvalue, ucmp, uex);
}


inline void* atomicCompareExchange(void* volatile& ptr, void* ex, void* cmp)
{
    return atomic_cas_ptr(&ptr, cmp, ex);
}

} // namespace cxxtools

#endif
