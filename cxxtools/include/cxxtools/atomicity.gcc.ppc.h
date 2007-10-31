/***************************************************************************
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
#ifndef CXXTOOLS_ATOMICINT_GCC_PPC_H
#define CXXTOOLS_ATOMICINT_GCC_PPC_H

#include <csignal>

namespace cxxtools {

typedef std::sig_atomic_t atomic_t;


inline atomic_t atomicIncrement(volatile atomic_t& val)
{
    atomic_t result = 0, tmp;
    asm volatile ("\n1:\n\t"
                  "lwarx %0, 0, %2\n\t"
                  "addi %1, %0, 1\n\t"
                  "stwcx. %1, 0, %2\n\t"
                  "bne- 1b"
                  : "=&b" (result), "=&b" (tmp): "r" (&val): "cc", "memory");
    return result + 1;
}


inline atomic_t atomicDecrement(volatile atomic_t& val)
{
    atomic_t result = 0, tmp;
    asm volatile ("\n1:\n\t"
                  "lwarx %0, 0, %2\n\t"
                  "addi %1, %0, -1\n\t"
                  "stwcx. %1, 0, %2\n\t"
                  "bne- 1b"
                  : "=&b" (result), "=&b" (tmp): "r" (&val): "cc", "memory");

    return result - 1;
}


inline atomic_t atomicExchangeAdd(volatile atomic_t& val, atomic_t add)
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
                  "bne 1b"
                  : "=&r" (result), "=&r" (tmp)
                  : "r" (&val), "r" (add) : "cc", "memory");
    return result;
}


inline atomic_t atomicCompareExchange(volatile atomic_t& val, atomic_t exch, atomic_t comp)
{
    /* ALTERNATIVE
    volatile register atomic_t ret = 0;

    asm volatile (
        "sync                    \n\t"
        "1:    lwarx   %0,  0, %1\n\t"
        "      subf.   %0, %2, %0\n\t"
        "      bne     2f        \n\t"
        "      stwcx.  %3,  0, %1\n\t"
        "      bne-    1b        \n\t"
        "2:    isync                 "
        : "=&r"(ret)
        :   "b"(&val), "r"(oldval), "r"(newval)
        : "cr0", "memory"
    );

    return ret == 0;
    */

    atomic_t tmp = 0;
    asm volatile ("\n1:\n\t"
                  "lwarx %0, 0, %1\n\t"
                  "cmpw %0, %2\n\t"
                  "bne- 2f\n\t"
                  "stwcx. %3, 0, %1\n\t"
                  "bne- 1b\n"
                  "2:"
                  : "=&r" (tmp)
                  : "b" (&val), "r" (comp), "r" (exch): "cc", "memory");

    return tmp;
}


inline void* atomicCompareExchange(volatile void*& ptr, void* exch, void* comp)
{
    void* tmp = 0;
    asm volatile ("\n1:\n\t"
                  "lwarx %0, 0, %1\n\t"
                  "cmpw %0, %2\n\t"
                  "bne- 2f\n\t"
                  "stwcx. %3, 0, %1\n\t"
                  "bne- 1b\n"
                  "2:"
                  : "=&r" (tmp)
                  : "b" (&ptr), "r" (comp), "r" (exch): "cc", "memory");

    return tmp;
}


inline atomic_t atomicExchange(volatile atomic_t& val, atomic_t exch)
{
    volatile register atomic_t ret = 0;

    asm volatile (
        "0:    lwarx  %0, 0, %1\n\t"
        "      stwcx. %2, 0, %1\n\t"
        "      bne-   0b       \n\t"
        "      isync               "
        : "=&r"(ret)
        :   "r"(&val),  "r"(exch)
        : "cr0","memory", "r0"
    );

    return ret;

/* ALTERNATIVE missing isync might be wrong
    volatile atomic_t tmp = 0;
    asm volatile ("\n1:\n\t"
                  "lwarx %0, 0, %2\n\t"
                  "stwcx. %3, 0, %2\n\t"
                  "bne 1b"
                  : "=r" (tmp) : "0" (tmp), "b" (&val), "r" (exch): "cc", "memory");
    return tmp;
*/
}


inline void* atomicExchange(volatile void*& dest, void* exch)
{
    void* ret = 0;

    asm volatile (
        "0:    lwarx  %0, 0, %1\n\t"
        "      stwcx. %2, 0, %1\n\t"
        "      bne-   0b       \n\t"
        "      isync               "
        : "=&r"(ret)
        :   "r"(&dest),  "r"(exch)
        : "cr0","memory", "r0"
    );

    return ret;
}

} // namespace cxxtools

#endif
