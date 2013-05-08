/*
 * Copyright (C) 2006-2007 by Marc Boris Duerner
 * Copyright (C) 2006-2007 by Aloysius Indrayanto
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
#ifndef CXXTOOLS_ATOMICITY_H
#define CXXTOOLS_ATOMICITY_H

#include <cxxtools/config.h>

#if defined(CXXTOOLS_ATOMICITY_SUN)
    #include <cxxtools/atomicity.sun.h>

#elif defined(CXXTOOLS_ATOMICITY_WINDOWS)
    #include <cxxtools/atomicity.windows.h>

#elif defined(CXXTOOLS_ATOMICITY_GCC_ARM)
    #include <cxxtools/atomicity.gcc.arm.h>

#elif defined(CXXTOOLS_ATOMICITY_GCC_MIPS)
    #include <cxxtools/atomicity.gcc.mips.h>

#elif defined(CXXTOOLS_ATOMICITY_GCC_AVR32)
    #include <cxxtools/atomicity.gcc.avr32.h>

#elif defined(CXXTOOLS_ATOMICITY_GCC_SPARC32)
    #include <cxxtools/atomicity.gcc.sparc32.h>

#elif defined(CXXTOOLS_ATOMICITY_GCC_SPARC64)
    #include <cxxtools/atomicity.gcc.sparc64.h>

#elif defined(CXXTOOLS_ATOMICITY_GCC_X86_64)
    #include <cxxtools/atomicity.gcc.x86_64.h>

#elif defined(CXXTOOLS_ATOMICITY_GCC_X86)
    #include <cxxtools/atomicity.gcc.x86.h>

#elif defined(CXXTOOLS_ATOMICITY_GCC_PPC)
    #include <cxxtools/atomicity.gcc.ppc.h>

#elif defined(CXXTOOLS_ATOMICITY_PTHREAD)
    #include <cxxtools/atomicity.pthread.h>

#elif defined(_WIN32) || defined(WIN32) || defined(_WIN32_WCE)
    #define CXXTOOLS_ATOMICITY_WINDOWS
    #include <cxxtools/atomicity.windows.h>

#elif defined(__sun)
    #define CXXTOOLS_ATOMICITY_SUN
    #include <cxxtools/atomicity.sun.h>

#elif defined(__GNUC__) || defined(__xlC__) || \
      defined(__SUNPRO_CC) || defined(__SUNPRO_C)

    #if defined (i386) || defined(__i386) || defined (__i386__) || \
        defined(_X86_) || defined(sun386) || defined (_M_IX86)
        #define CXXTOOLS_ATOMICITY_GCC_X86
        #include <cxxtools/atomicity.gcc.x86.h>

    #elif defined(__x86_64__) || defined(__amd64__)
        #define CXXTOOLS_ATOMICITY_GCC_X86_64
        #include <cxxtools/atomicity.gcc.x86_64.h>

    #elif defined (ARM) || defined(__arm__) || defined(_M_ARM) || defined(_M_ARMT)
        #define CXXTOOLS_ATOMICITY_GCC_ARM
        #include <cxxtools/atomicity.gcc.arm.h>

    #elif defined (AVR) || defined(__AVR__)
        #define CXXTOOLS_ATOMICITY_GCC_AVR32
        #include <cxxtools/atomicity.avr32.h>

    #elif defined( _M_PPC  ) || defined( PPC         ) || \
          defined( ppc     ) || defined( __powerpc__ ) || \
          defined( __ppc__ )
        #define CXXTOOLS_ATOMICITY_GCC_PPC
        #include <cxxtools/atomicity.gcc.ppc.h>

    #elif defined(__mips__) || defined(MIPSEB) || defined(_MIPSEB) || \
          defined(MIPSEL) || defined(_MIPSEL)
        #define CXXTOOLS_ATOMICITY_GCC_MIPS
        #include <cxxtools/atomicity.gcc.mips.h>

    #elif defined(__sparcv9)
        #define CXXTOOLS_ATOMICITY_GCC_SPARC64
        #include <cxxtools/atomicity.gcc.sparc64.h>

    #elif defined(__sparc__) || defined(sparc) || defined(__sparc) || \
          defined(__sparcv8)
        #define CXXTOOLS_ATOMICITY_GCC_SPARC32
        #include <cxxtools/atomicity.gcc.sparc32.h>

    #else
        #include <cxxtools/atomicity.generic.h>

    #endif

#else
    #include <cxxtools/atomicity.generic.h>

#endif


namespace cxxtools {

/** @brief Atomically get a value

    Returns the value after employing a memory fence.
*/
atomic_t atomicGet(volatile atomic_t& val);

/** @brief Atomically set a value

    Sets the value and employs a memory fence.
*/
void atomicSet(volatile atomic_t& val, atomic_t n);

/** @brief Increases a value by one as an atomic operation

    Returns the resulting incremented value.
*/
atomic_t atomicIncrement(volatile atomic_t& val);

/** @brief Decreases a value by one as an atomic operation

    Returns the resulting decremented value.
*/
atomic_t atomicDecrement(volatile atomic_t& val);

/** @brief Performs atomic addition of two values

    Returns the initial value of the addend.
*/
atomic_t atomicExchangeAdd(volatile atomic_t& val, atomic_t add);

/** @brief Performs an atomic compare-and-exchange operation

    If \a val is equal to \a comp, \a val is replaced by \a exch. The initial
    value of of \a val is returned.
*/
atomic_t atomicCompareExchange(volatile atomic_t& val, atomic_t exch, atomic_t comp);

/** @brief Performs an atomic compare-and-exchange operation

    If \a ptr is equal to \a comp, \a ptr is replaced by \a exch. The initial
    value of \a ptr is returned.
*/
void* atomicCompareExchange(void* volatile& ptr, void* exch, void* comp);

/** @brief Performs an atomic exchange operation

    Sets \a val to \a exch and returns the initial value of \a val.
*/
atomic_t atomicExchange(volatile atomic_t& val, atomic_t exch);

/** @brief Performs an atomic exchange operation

    Sets \a dest to \a exch and returns the initial value of \a dest.
*/
void* atomicExchange(void* volatile& dest, void* exch);

}

#endif
