/***************************************************************************
 *   Copyright (C) 2006-2007 by Marc Boris Duerner                         *
 *   Copyright (C) 2006-2007 by Aloysius Indrayanto                        *
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
#ifndef CXXTOOLS_ATOMICITY_H

// use Win32 Interlocked-functions
#if defined(_MSC_VER)

    #define CXXTOOLS_ATOMICITY_H "atomicity.windows.h"

// use AT&T-style inline asm
#elif defined(__GNUC__) || defined(__xlC__)

    #if defined( _i386_     ) || defined( __i386__ ) || \
        defined( __x86_64__ ) || defined( _M_IX86  )

        #define CXXTOOLS_ATOMICITY_H "atomicity.gcc.x86.h"

    #elif defined( __arm__ )

        #define CXXTOOLS_ATOMICITY_H "atomicity.gcc.arm.h"

    #elif defined( _M_PPC  ) || defined( PPC         ) || \
          defined( ppc     ) || defined( __powerpc__ ) || \
          defined( __ppc__ )

        #define CXXTOOLS_ATOMICITY_H "atomicity.gcc.ppc.h"

    #elif defined( __mips__ )

        #define CXXTOOLS_ATOMICITY_H "atomicity.gcc.mips.h"

    #else

        #define CXXTOOLS_ATOMICITY_H "atomicity.generic.h"

    #endif

// fallback to normal integer operations
#else

    #define CXXTOOLS_ATOMICITY_H "atomicity.generic.h"

#endif

#include CXXTOOLS_ATOMICITY_H

namespace cxxtools {

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
void* atomicCompareExchange(volatile void*& ptr, void* exch, void* comp);

/** @brief Performs an atomic exchange operation

    Sets \a val to \a exch and returns the initial value of \a val.
*/
atomic_t atomicExchange(volatile atomic_t& val, atomic_t exch);

/** @brief Performs an atomic exchange operation

    Sets \a dest to \a exch and returns the initial value of \a dest.
*/
void* atomicExchange(volatile void*& dest, void* exch);

}

#endif
