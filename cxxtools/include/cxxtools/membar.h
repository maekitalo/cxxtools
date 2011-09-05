/*
 * Copyright (C) 2011 by Julian Wiesener
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
#ifndef CXXTOOLS_MEMBAR_H
#define CXXTOOLS_MEMBAR_H

#include <cxxtools/config.h>

#if defined(CXXTOOLS_ATOMICITY_SUN)
    #include <cxxtools/membar.sun.h>

#elif defined(CXXTOOLS_ATOMICITY_WINDOWS)
    #include <cxxtools/membar.windows.h>

#elif defined(CXXTOOLS_ATOMICITY_GCC_ARM)
    #include <cxxtools/membar.gcc.arm.h>

#elif defined(CXXTOOLS_ATOMICITY_GCC_MIPS)
    #include <cxxtools/membar.gcc.mips.h>

#elif defined(CXXTOOLS_ATOMICITY_GCC_AVR32)
    #include <cxxtools/membar.gcc.nonsmp.h>

#elif defined(CXXTOOLS_ATOMICITY_GCC_SPARC32)
    #include <cxxtools/membar.gcc.sparc32.h>

#elif defined(CXXTOOLS_ATOMICITY_GCC_SPARC)
    #include <cxxtools/membar.gcc.sparc64.h>

#elif defined(CXXTOOLS_ATOMICITY_GCC_X86_64)
    #include <cxxtools/membar.gcc.x86.h>

#elif defined(CXXTOOLS_ATOMICITY_GCC_X86)
    #include <cxxtools/membar.gcc.x86.h>

#elif defined(CXXTOOLS_ATOMICITY_GCC_PPC)
    #include <cxxtools/membar.gcc.ppc.h>

#elif defined(_WIN32) || defined(WIN32) || defined(_WIN32_WCE)
    #include <cxxtools/membar.windows.h>

#elif defined(__sun)
    #include <cxxtools/membar.sun.h>

#elif defined(__GNUC__) || defined(__xlC__) || \
      defined(__SUNPRO_CC) || defined(__SUNPRO_C)

    #if defined (i386) || defined(__i386) || defined (__i386__) || \
        defined(_X86_) || defined(sun386) || defined (_M_IX86) || \
	defined(__x86_64__) || defined(__amd64__)
        #include <cxxtools/membar.gcc.x86.h>

    #elif defined (ARM) || defined(__arm__) || defined(_M_ARM) || defined(_M_ARMT)
        #include <cxxtools/membar.gcc.arm.h>

    #elif defined (AVR) || defined(__AVR__)
        #include <cxxtools/membar.gcc.nonsmp.h>

    #elif defined( _M_PPC  ) || defined( PPC         ) || \
          defined( ppc     ) || defined( __powerpc__ ) || \
          defined( __ppc__ )
        #include <cxxtools/membar.gcc.ppc.h>

    #elif defined(__mips__) || defined(MIPSEB) || defined(_MIPSEB) || \
          defined(MIPSEL) || defined(_MIPSEL)
        #include <cxxtools/membar.gcc.mips.h>

    #elif defined(__sparcv9)
        #include <cxxtools/membar.gcc.sparc64.h>

    #elif defined(__sparc__) || defined(sparc) || defined(__sparc) || \
          defined(__sparcv8)
        #include <cxxtools/membar.gcc.sparc32.h>
    
    #else
	#include <cxxtools/membar.gcc.h>

    #endif

#else
    #error "unknown architecture"

#endif

#endif // CXXTOOLS_MEMBAR_H
