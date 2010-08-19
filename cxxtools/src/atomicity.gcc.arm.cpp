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

#include <cxxtools/atomicity.gcc.arm.h>
#include <csignal>
#include <cstdio>

namespace cxxtools {

atomic_t atomicGet(volatile atomic_t& val)
{
    asm volatile ("" : : : "memory");
    return val;
}


void atomicSet(volatile atomic_t& val, atomic_t n)
{
    val = n;
    asm volatile ("" : : : "memory");
}


atomic_t atomicIncrement(volatile atomic_t& dest)
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


atomic_t atomicDecrement(volatile atomic_t& dest)
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


atomic_t atomicCompareExchange(volatile atomic_t& dest, atomic_t exch, atomic_t comp)
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


void* atomicCompareExchange(void* volatile& dest, void* exch, void* comp)
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
    return (void*)a;
}


atomic_t atomicExchange(volatile atomic_t& dest, atomic_t exch)
{
       int a;

       asm volatile (  "swp %0, %2, [%1]"
                               : "=&r" (a)
                               : "r" (&dest), "r" (exch));

       return a;
}

void* atomicExchange(void* volatile& dest, void* exch)
{
       void* a;

       asm volatile ( "swp %0, %2, [%1]"
                      : "=&r" (a)
                      : "r" (&dest), "r" (exch));

       return a;
}


atomic_t atomicExchangeAdd(volatile atomic_t& dest, atomic_t add)
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
