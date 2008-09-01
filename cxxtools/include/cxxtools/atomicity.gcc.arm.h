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
#ifndef CXXTOOLS_ATOMICINT_GCC_ARM_H
#define CXXTOOLS_ATOMICINT_GCC_ARM_H

#include <csignal>
#include <cstdio>

namespace cxxtools {

typedef std::sig_atomic_t atomic_t;


inline atomic_t atomicGet(volatile atomic_t& val)
{
    asm volatile ("" : : : "memory");
    return val;
}


inline void atomicSet(volatile atomic_t& val, atomic_t n)
{
    val = n;
    asm volatile ("" : : : "memory");
}


inline atomic_t atomicIncrement(volatile atomic_t& dest)
{
       int a, b, c;

       asm volatile (  "0:\n\t"
                               "ldr %0, [%3]\n\t"
                               "add %1, %0, %4\n\t"
                               "swp %2, %1, [%3]\n\t"
                               "cmp %0, %2\n\t"
                               "swpne %1, %2, [%3]\n\t"
                               "bne 0b"
                               : "=&r" (a), "=&r" (b), "=&r" (c)
                               : "r" (&dest), "r" (1)
                               : "cc", "memory");

       return b;
}


inline atomic_t atomicDecrement(volatile atomic_t& dest)
{
       int a, b, c;

       asm volatile (  "0:\n\t"
                               "ldr %0, [%3]\n\t"
                               "add %1, %0, %4\n\t"
                               "swp %2, %1, [%3]\n\t"
                               "cmp %0, %2\n\t"
                               "swpne %1, %2, [%3]\n\t"
                               "bne 0b"
                               : "=&r" (a), "=&r" (b), "=&r" (c)
                               : "r" (&dest), "r" (-1)
                               : "cc", "memory");

       return b;
}


inline atomic_t atomicCompareExchange(volatile atomic_t& dest, atomic_t exch, atomic_t comp)
{
    int a, b;

    asm volatile ( "0:\n\t"
                    "ldr %1, [%2]\n\t"
                    "cmp %1, %4\n\t"
                    "mov %0, %1\n\t"
                    "bne 1f\n\t"
                    "swp %0, %3, [%2]\n\t"
                    "cmp %0, %1\n\t"
                    "swpne %3, %0, [%2]\n\t"
                    "bne 0b\n\t"
                    "1:"
                    : "=&r" (a), "=&r" (b)
                    : "r" (&dest), "r" (exch), "r" (comp)
                    : "cc", "memory");

    return a;
}


inline void* atomicCompareExchange(void* volatile& dest, void* exch, void* comp)
{
    volatile void* a;
    volatile void* b;

    asm volatile ( "0:\n\t"
                   "ldr %1, [%2]\n\t"
                   "cmp %1, %4\n\t"
                   "mov %0, %1\n\t"
                   "bne 1f\n\t"
                   "swpeq %0, %3, [%2]\n\t"
                   "cmp %0, %1\n\t"
                   "swpne %3, %0, [%2]\n\t"
                   "bne 0b\n\t"
                   "1:"
                   : "=&r" (a), "=&r" (b)
                   : "r" (&dest), "r" (exch), "r" (comp)
                   : "cc", "memory");
    return a;
}


inline atomic_t atomicExchange(volatile atomic_t& dest, atomic_t exch)
{
       int a;

       asm volatile (  "swp %0, %2, [%1]"
                               : "=&r" (a)
                               : "r" (&dest), "r" (exch));

       return a;
}

template <typename T>
T* atomicExchange(T* volatile& dest, T* exch)
{
       T* a;

       asm volatile ( "swp %0, %2, [%1]"
                      : "=&r" (a)
                      : "r" (&dest), "r" (exch));

       return a;
}


inline atomic_t atomicExchangeAdd(volatile atomic_t& dest, atomic_t add)
{
       int a, b, c;

       asm volatile (  "0:\n\t"
                       "ldr %0, [%3]\n\t"
                       "add %1, %0, %4\n\t"
                       "swp %2, %1, [%3]\n\t"
                       "cmp %0, %2\n\t"
                       "swpne %1, %2, [%3]\n\t"
                       "bne 0b"
                       : "=&r" (a), "=&r" (b), "=&r" (c)
                       : "r" (&dest), "r" (add)
                       : "cc", "memory");
       return a;
}

} // namespace cxxtools

#endif
