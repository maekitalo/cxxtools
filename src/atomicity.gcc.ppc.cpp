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

#include <cxxtools/atomicity.gcc.ppc.h>
#include <csignal>

namespace cxxtools {


atomic_t atomicGet(volatile atomic_t& val)
{
    asm volatile ("sync" : : : "memory");
    return val;
}


void atomicSet(volatile atomic_t& val, atomic_t n)
{
    val = n;
    asm volatile ("sync" : : : "memory");
}


atomic_t atomicIncrement(volatile atomic_t& val)
{
    atomic_t result = 0, tmp;
    asm volatile ("\n1:\n\t"
                  "lwarx %0, 0, %2\n\t"
                  "addi %1, %0, 1\n\t"
                  "stwcx. %1, 0, %2\n\t"
                  "bne- 1b\n"
                  "isync\n"
                  : "=&b" (result), "=&b" (tmp): "r" (&val): "cc", "memory");
    return result + 1;
}


atomic_t atomicDecrement(volatile atomic_t& val)
{
    atomic_t result = 0, tmp;
    asm volatile ("\n1:\n\t"
                  "lwarx %0, 0, %2\n\t"
                  "addi %1, %0, -1\n\t"
                  "stwcx. %1, 0, %2\n\t"
                  "bne- 1b\n"
                  "isync\n"
                  : "=&b" (result), "=&b" (tmp): "r" (&val): "cc", "memory");

    return result - 1;
}


atomic_t atomicExchangeAdd(volatile atomic_t& val, atomic_t add)
{
/* ALTERNATIVE
    volatile register atomic_t ret  = 0;
    volatile register atomic_t zero = 0;

    asm volatile (
        "0:    lwarx  %0, %3, %1\n\t"
        "      add    %0, %2, %0\n\t"
        "      stwcx. %0, %3, %1\n\t"
        "      bne-   0b        \n\t"
        "      isync                "
        : "=&r"(ret)
        :   "r"(&val), "r"(n), "r"(zero)
        : "cr0", "memory", "r0"
    );
*/
    atomic_t result, tmp;
    asm volatile ("\n1:\n\t"
                  "lwarx %0, 0, %2\n\t"
                  "add %1, %0, %3\n\t"
                  "stwcx. %1, 0, %2\n\t"
                  "bne 1b\n"
                  "isync\n"
                  : "=&r" (result), "=&r" (tmp)
                  : "r" (&val), "r" (add) : "cc", "memory");
    return result;
}


atomic_t atomicCompareExchange(volatile atomic_t& val, atomic_t exch, atomic_t comp)
{
/* ALTERNATIVE:
    atomicCompareExchange( long *dest, long xchg, long compare)
    long ret = 0;
    long scratch;
    __asm__ __volatile__(
        "0:    lwarx %0,0,%2\n"
        "      xor. %1,%4,%0\n"
        "      bne 1f\n"
        "      stwcx. %3,0,%2\n"
        "      bne- 0b\n"
        "      isync\n"
        "1:    "
        : "=&r"(ret), "=&r"(scratch)
        : "r"(dest), "r"(xchg), "r"(compare)
        : "cr0","memory","r0");
    return ret;
*/

    atomic_t tmp = 0;
    asm volatile ("\n1:\n\t"
                  "lwarx %0, 0, %1\n\t"
                  "cmpw %0, %2\n\t"
                  "bne- 2f\n\t"
                  "stwcx. %3, 0, %1\n\t"
                  "bne- 1b\n"
                  "2:\n"
                  "isync\n"
                  : "=&r" (tmp)
                  : "b" (&val), "r" (comp), "r" (exch): "cc", "memory");

    return tmp;
}


void* atomicCompareExchange(void* volatile& ptr, void* exch, void* comp)
{
/* ALTERNATIVE:
    atomicCompareExchange( void **dest, void* xchg, void* compare)
    long ret = 0;
    long scratch;
    __asm__ __volatile__(
        "0:    lwarx %0,0,%2\n"
        "      xor. %1,%4,%0\n"
        "      bne 1f\n"
        "      stwcx. %3,0,%2\n"
        "      bne- 0b\n"
        "      isync\n"
        "1:    "
        : "=&r"(ret), "=&r"(scratch)
        : "r"(dest), "r"(xchg), "r"(compare)
        : "cr0","memory");
    return (void*)ret;
*/
    void* tmp = 0;
    asm volatile ("\n1:\n\t"
                  "lwarx %0, 0, %1\n\t"
                  "cmpw %0, %2\n\t"
                  "bne- 2f\n\t"
                  "stwcx. %3, 0, %1\n\t"
                  "bne- 1b\n"
                  "2:"
                  "isync\n"
                  : "=&r" (tmp)
                  : "b" (&ptr), "r" (comp), "r" (exch): "cc", "memory");

    return tmp;
}


atomic_t atomicExchange(volatile atomic_t& val, atomic_t exch)
{
    volatile atomic_t ret = 0;

/*
    asm volatile (
        "0:    lwarx  %0, 0, %1\n\t"
        "      stwcx. %2, 0, %1\n\t"
        "      bne-   0b       \n\t"
        "      isync               "
        : "=&r"(ret)
        :   "r"(&val),  "r"(exch)
        : "cr0","memory", "r0"
    );
*/

    asm volatile (
        "\n1:\n\t"
        "lwarx  %0, 0, %2\n\t"
        "stwcx. %3, 0, %2\n\t"
        "bne    1b\n"
        "isync\n"
        : "=r" (ret) : "0" (ret), "b" (&val), "r" (exch): "cc", "memory");

    return ret;
}


void* atomicExchange(void* volatile& dest, void* exch)
{
    void* ret = 0;

    asm volatile (
        "0:    lwarx  %0, 0, %1\n\t"
        "      stwcx. %2, 0, %1\n\t"
        "      bne-   0b       \n\t"
        "      isync"
        : "=&r"(ret)
        :   "r"(&dest),  "r"(exch)
        : "cr0","memory", "r0"
    );

    return ret;
}

} // namespace cxxtools
