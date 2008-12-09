/***************************************************************************
 *   Copyright (C) 2006 by Marc Boris Duerner                              *
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

#include <cxxtools/atomicity.gcc.sparc.h>
#include <csignal>

namespace cxxtools {


atomic_t atomicGet(volatile atomic_t& val)
{
    asm volatile ("membar	#LoadLoad | #LoadStore | #StoreStore | #StoreLoad" : : : "memory");
    return val;
}


void atomicSet(volatile atomic_t& val, atomic_t n)
{
    val = n;
    asm volatile ("membar	#LoadLoad | #LoadStore | #StoreStore | #StoreLoad" : : : "memory");
}


atomic_t atomicIncrement(volatile atomic_t& val)
{
    register volatile atomic_t* dest asm("g1") = &val;
    register atomic_t tmp asm("o4");
    register atomic_t ret asm("o5");

    asm volatile(
            "1:     ld      [%%g1], %%o4\n\t"
            "       add     %%o4, 1, %%o5\n\t"
            /*      cas     [%%g1], %%o4, %%o5 */
            "       .word   0xdbe0500c\n\t"
            "       cmp     %%o4, %%o5\n\t"
            "       bne     1b\n\t"
            "        add    %%o5, 1, %%o5"
            : "=&r" (tmp), "=&r" (ret)
            : "r" (dest)
            : "memory", "cc");

    return ret;
}


atomic_t atomicDecrement(volatile atomic_t& val)
{
    register volatile atomic_t* dest asm("g1") = &val;
    register atomic_t tmp asm("o4");
    register atomic_t ret asm("o5");

    asm volatile(
            "1:     ld      [%%g1], %%o4\n\t"
            "       sub     %%o4, 1, %%o5\n\t"
            /*      cas     [%%g1], %%o4, %%o5 */
            "       .word   0xdbe0500c\n\t"
            "       cmp     %%o4, %%o5\n\t"
            "       bne     1b\n\t"
            "        sub    %%o5, 1, %%o5"
            : "=&r" (tmp), "=&r" (ret)
            : "r" (dest)
            : "memory", "cc");

        return ret;
}


atomic_t atomicExchangeAdd(volatile atomic_t& val, atomic_t add)
{
       register volatile atomic_t* dest asm("g1") = &val;
       register atomic_t tmp asm("o4");
       register atomic_t ret asm("o5");

       asm volatile(
               "1:     ld      [%%g1], %%o4\n\t"
               "       add     %%o4, %3, %%o5\n\t"
               /*      cas     [%%g1], %%o4, %%o5 */
               "       .word   0xdbe0500c\n\t"
               "       cmp     %%o4, %%o5\n\t"
               "       bne     1b\n\t"
               "        add    %%o5, %3, %%o5"
               : "=&r" (tmp), "=&r" (ret)
               : "r" (dest), "r" (add)
               : "memory", "cc");

        return ret;
}


atomic_t atomicCompareExchange(volatile atomic_t& val, atomic_t exch, atomic_t comp)
{
    register volatile atomic_t* dest asm("g1") = &val;
    register atomic_t _comp asm("o4") = comp;
    register atomic_t _exch asm("o5") = exch;

    asm volatile(
            /* cas [%%g1], %%o4, %%o5 */
            ".word 0xdbe0500c"
            : "=r" (_exch)
            : "0" (_exch), "r" (dest), "r" (_comp)
            : "memory");

    return exch;
}


void* atomicCompareExchange(void* volatile& ptr, void* exch, void* comp)
{
    register void* volatile* dest asm("g1") = &ptr;
    register void* _comp asm("o4") = comp;
    register void* _exch asm("o5") = exch;

    asm volatile(
#if defined(__sparcv9)
        /* casx [%%g1], %%o4, %%o5 */
        ".word 0xdbf0500c"
#else
        /* cas [%%g1], %%o4, %%o5 */
        ".word 0xdbe0500c"
#endif
        : "=r" (_exch)
        : "0" (_exch), "r" (dest), "r" (_comp)
        : "memory");

    return exch;
}


atomic_t atomicExchange(volatile atomic_t& val, atomic_t exch)
{
    register volatile atomic_t* dest asm("g1") = &val;
    register atomic_t tmp asm("o4");
    register atomic_t ret asm("o5");

    asm volatile(
            "1:     ld      [%%g1], %%o4\n\t"
            "       mov     %3, %%o5\n\t"
            /*      cas     [%%g1], %%o4, %%o5 */
            "       .word   0xdbe0500c\n\t"
            "       cmp     %%o4, %%o5\n\t"
            "       bne     1b\n\t"
            "        nop"
            : "=&r" (tmp), "=&r" (ret)
            : "r" (dest), "r" (exch)
            : "memory", "cc");

    return ret;
}


void* atomicExchange(void* volatile& ptr, void* exch)
{
       register void* volatile* dest asm("g1") = &ptr;
       register void* tmp asm("o4");
       register void* ret asm("o5");

       asm volatile(
#if defined(__sparcv9)
               "1:     ldx     [%%g1], %%o4\n\t"
#else
               "1:     ld      [%%g1], %%o4\n\t"
#endif
               "       mov     %3, %%o5\n\t"
#if defined(__sparcv9)
               /*      casx    [%%g1], %%o4, %%o5 */
               "       .word   0xdbf0500c\n\t"
#else
               /*      cas     [%%g1], %%o4, %%o5 */
               "       .word   0xdbe0500c\n\t"
#endif
               "       cmp     %%o4, %%o5\n\t"
               "       bne     1b\n\t"
               "        nop"
               : "=&r" (tmp), "=&r" (ret)
               : "r" (dest), "r" (exch)
               : "memory", "cc");

        return ret;
}

} // namespace cxxtools
