/***************************************************************************
 *   Copyright (C) 2006 by Marc Boris Dürner                               *
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
#ifndef CXXTOOLS_ATOMICINT_GCC_X86_H
#define CXXTOOLS_ATOMICINT_GCC_X86_H

#include <csignal>


namespace cxxtools {

typedef std::sig_atomic_t atomic_t;


inline atomic_t atomicIncrement(volatile atomic_t& val)
{
        atomic_t tmp;

        asm volatile ("lock; xaddl %0, %1"
                      : "=r" (tmp), "=m" (val)
                      : "0" (1), "m" (val));

        return tmp+1;
}

inline atomic_t atomicDecrement(volatile atomic_t& val)
{
        volatile register atomic_t tmp;

        asm volatile ("lock; xaddl %0, %1"
                      : "=r" (tmp), "=m" (val)
                      : "0" (-1), "m" (val));

        return tmp-1;
}


inline atomic_t atomicExchangeAdd(volatile atomic_t& val, atomic_t add)
{
        volatile register atomic_t ret;

        asm volatile ("lock; xaddl %0, %1"
                      : "=r" (ret), "=m" (val)
                      : "0" (add), "m" (val));

        return ret;
}


inline atomic_t atomicExchange(volatile atomic_t& val, atomic_t new_val)
{
        volatile register atomic_t ret;

        // using cmpxchg and a loop here on purpose
        asm volatile ("1:; lock; cmpxchgl %2, %0; jne 1b"
                      : "=m" (val), "=a" (ret)
                      : "r" (new_val), "m" (val), "a" (val));

        return ret;
}


inline void* atomicExchange(void* volatile& val, void* new_val)
{
        void* ret;

        asm volatile ("1:; lock; cmpxchgl %2, %0; jne 1b"
                      : "=m" (val), "=a" (ret)
                      : "r" (new_val), "m" (val), "a" (val));

        return ret;
}


inline atomic_t atomicCompareExchange(volatile atomic_t& dest, atomic_t exch, atomic_t comp)
{
        volatile register atomic_t old;

        asm volatile ("lock; cmpxchgl %2, %0"
                      : "=m" (dest), "=a" (old)
                      : "r" (exch), "m" (dest), "a" (comp));
        return old;
}


inline void* atomicCompareExchange(void* volatile& dest, void* exch, void* comp)
{
        void* old;

        asm volatile ("lock; cmpxchgl %2, %0"
                      : "=m" (dest), "=a" (old)
                      : "r" (exch), "m" (dest), "a" (comp));
        return old;
}


} // namespace cxxtools

#endif
