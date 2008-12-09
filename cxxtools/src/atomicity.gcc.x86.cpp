/***************************************************************************
 *   Copyright (C) 2006 by Marc Boris Dürner                               *
 *   Copyright (C) 2006 by Aloysius Indrayanto                             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
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

#include <cxxtools/atomicity.gcc.x86.h>
#include <csignal>


namespace cxxtools {


atomic_t atomicGet(volatile atomic_t& val)
{
    asm volatile ("lock; addl $0,0(%%esp)" : : : "memory");
    return val;
}


void atomicSet(volatile atomic_t& val, atomic_t n)
{
    val = n;
    asm volatile ("lock; addl $0,0(%%esp)" : : : "memory");
}


atomic_t atomicIncrement(volatile atomic_t& val)
{
        atomic_t tmp;

        asm volatile ("lock; xaddl %0, %1"
                      : "=r" (tmp), "=m" (val)
                      : "0" (1), "m" (val));

        return tmp+1;
}

atomic_t atomicDecrement(volatile atomic_t& val)
{
        volatile register atomic_t tmp;

        asm volatile ("lock; xaddl %0, %1"
                      : "=r" (tmp), "=m" (val)
                      : "0" (-1), "m" (val));

        return tmp-1;
}


atomic_t atomicExchangeAdd(volatile atomic_t& val, atomic_t add)
{
        volatile register atomic_t ret;

        asm volatile ("lock; xaddl %0, %1"
                      : "=r" (ret), "=m" (val)
                      : "0" (add), "m" (val));

        return ret;
}


atomic_t atomicExchange(volatile atomic_t& val, atomic_t new_val)
{
        volatile register atomic_t ret;

        // using cmpxchg and a loop here on purpose
        asm volatile ("1:; lock; cmpxchgl %2, %0; jne 1b"
                      : "=m" (val), "=a" (ret)
                      : "r" (new_val), "m" (val), "a" (val));

        return ret;
}


void* atomicExchange(void* volatile& val, void* new_val)
{
        void* ret;

        asm volatile ("1:; lock; cmpxchgl %2, %0; jne 1b"
                      : "=m" (val), "=a" (ret)
                      : "r" (new_val), "m" (val), "a" (val));

        return ret;
}


atomic_t atomicCompareExchange(volatile atomic_t& dest, atomic_t exch, atomic_t comp)
{
        volatile register atomic_t old;

        asm volatile ("lock; cmpxchgl %2, %0"
                      : "=m" (dest), "=a" (old)
                      : "r" (exch), "m" (dest), "a" (comp));
        return old;
}


void* atomicCompareExchange(void* volatile& dest, void* exch, void* comp)
{
        void* old;

        asm volatile ("lock; cmpxchgl %2, %0"
                      : "=m" (dest), "=a" (old)
                      : "r" (exch), "m" (dest), "a" (comp));
        return old;
}


} // namespace cxxtools
