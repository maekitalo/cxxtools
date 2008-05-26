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

#include <cxxtools/config.h>

#if defined(CXXTOOLS_ATOMICITY_SUN)
    #include <cxxtools/atomicity.sun.h>

#elif defined(CXXTOOLS_ATOMICITY_WINDOWS)
    #include <cxxtools/atomicity.windows.h>

#elif defined(CXXTOOLS_ATOMICITY_GCC_ARM)
    #include <cxxtools/atomicity.gcc.arm.h>

#elif defined(CXXTOOLS_ATOMICITY_GCC_MIPS)
    #include <cxxtools/atomicity.gcc.mips.h>

#elif defined(CXXTOOLS_ATOMICITY_GCC_MIPS)
    #include <cxxtools/atomicity.gcc.ppc.h>

#elif defined(CXXTOOLS_ATOMICITY_GCC_SPARC)
    #include <cxxtools/atomicity.gcc.sparc.h>

#elif defined(CXXTOOLS_ATOMICITY_GCC_X86_64)
    #include <cxxtools/atomicity.gcc.x86_64.h>

#elif defined(CXXTOOLS_ATOMICITY_GCC_X86)
    #include <cxxtools/atomicity.gcc.x86.h>

#elif defined(CXXTOOLS_ATOMICITY_GCC_PPC)
    #include <cxxtools/atomicity.gcc.ppc.h>

#else
    #include <cxxtools/atomicity.pthread.h>

#endif


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
