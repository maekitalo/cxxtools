/*
 * Copyright (C) 2006 by Marc Boris Duerner
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

#include <cxxtools/atomicity.gcc.avr32.h>

namespace cxxtools {

atomic_t atomicGet(volatile atomic_t& val)
{
    asm volatile ("" : : : "memory");
    return val;
}


void atomicSet(volatile atomic_t& val, atomic_t n)
{
    val = n;
    asm volatile("sync 0" : : : "memory")
}


atomic_t atomicIncrement(volatile atomic_t& val)
{
    volatile uint8_t tmp;
    asm volatile(
        "in %0, __SREG__"           "\n\t"
        "cli"                       "\n\t"
        "ld __tmp_reg__, %a1"       "\n\t"
        "inc __tmp_reg__"           "\n\t"
        "st %a1, __tmp_reg__"       "\n\t"
        "out __SREG__, %0"          "\n\t"
        : "=&r" (tmp)
        : "e" (&val)
        : "memory"
    );

    return tmp-1;
}


atomic_t atomicDecrement(volatile atomic_t& val)
{
    volatile uint8_t tmp;
    asm volatile(
        "in %0, __SREG__"           "\n\t"
        "cli"                       "\n\t"
        "ld __tmp_reg__, %a1"       "\n\t"
        "dec __tmp_reg__"           "\n\t"
        "st %a1, __tmp_reg__"       "\n\t"
        "out __SREG__, %0"          "\n\t"
        : "=&r" (tmp)
        : "e" (&val)
        : "memory"
    );

    return tmp+1;
}


atomic_t atomicExchangeAdd(volatile atomic_t& val, atomic_t add)
{
    atomic_t result;
    asm volatile("1:     ssrf    5\n"
                 "       ld.w    %0, %1\n"
                 "       add     %0, %3\n"
                 "       stcond  %2, %0\n"
                 "       brne    1b"
                 : "=&r"(result), "=o"(val)
                 : "m"(val), "r"(add)
                 : "cc", "memory");
    return result;
}


atomic_t atomicExchange(volatile atomic_t& val, atomic_t new_val)
{
    atomic_t ret;
    asm volatile("xchg %[ret], %[val], %[new_val]"
                 : [ret] "=&r"(ret), "=m"(val)
                 : "m"(val), [val] "r"(&val), [new_val] "r"(new_val)
                 : "memory");
    return ret;
}


void* atomicExchange(void* volatile& val, void* new_val)
{
    atomic_t ret;
    asm volatile("xchg %[ret], %[val], %[new_val]"
                 : [ret] "=&r"(ret), "=m"(val)
                 : "m"(val), [val] "r"(&val), [new_val] "r"(new_val)
                 : "memory");
    return ret;
}


atomic_t atomicCompareExchange(volatile atomic_t& dest, atomic_t exch, atomic_t comp)
{
    volatile atomic_t ret;
    asm volatile(
            "1:     ssrf    5\n"
            "       ld.w    %[ret], %[dest]\n"
            "       cp.w    %[ret], %[comp]\n"
            "       brne    2f\n"
            "       stcond  %[dest], %[exch]\n"
            "       brne    1b\n"
            "2:\n"
            : [ret] "=&r"(ret), [dest] "=m"(dest)
            : "m"(&dest), [comp] "ir"(comp), [exch] "r"(exch)
            : "memory", "cc");
    return ret;
}


void* atomicCompareExchange(void* volatile& dest, void* exch, void* comp)
{
    volatile atomic_t ret;
    asm volatile(
            "1:     ssrf    5\n"
            "       ld.w    %[ret], %[dest]\n"
            "       cp.w    %[ret], %[comp]\n"
            "       brne    2f\n"
            "       stcond  %[dest], %[exch]\n"
            "       brne    1b\n"
            "2:\n"
            : [ret] "=&r"(ret), [dest] "=m"(dest)
            : "m"(&dest), [comp] "ir"(comp), [exch] "r"(exch)
            : "memory", "cc");
    return ret;
}

} // namespace cxxtools
