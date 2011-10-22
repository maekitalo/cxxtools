/*
 * Copyright (C) 2005 by Dr. Marc Boris Duerner
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
#include "cxxtools/atomicity.h"

#if defined(CXXTOOLS_ATOMICITY_GCC_ARM)
    #include "atomicity.gcc.arm.cpp"

#elif defined(CXXTOOLS_ATOMICITY_GCC_MIPS)
    #include "atomicity.gcc.mips.cpp"

#elif defined(CXXTOOLS_ATOMICITY_GCC_SPARC32)
    #include "atomicity.gcc.sparc32.cpp"

#elif defined(CXXTOOLS_ATOMICITY_GCC_SPARC64)
    #include "atomicity.gcc.sparc64.cpp"

#elif defined(CXXTOOLS_ATOMICITY_GCC_X86_64)
    #include "atomicity.gcc.x86_64.cpp"

#elif defined(CXXTOOLS_ATOMICITY_GCC_X86)
    #include "atomicity.gcc.x86.cpp"

#elif defined(CXXTOOLS_ATOMICITY_GCC_AVR32)
    #include "atomicity.gcc.avr32.cpp"

#elif defined(CXXTOOLS_ATOMICITY_GCC_PPC)
    #include "atomicity.gcc.ppc.cpp"

#elif defined(CXXTOOLS_ATOMICITY_WINDOWS)
    #include "atomicity.windows.cpp"

#elif defined(CXXTOOLS_ATOMICITY_SUN)
    #include "atomicity.sun.cpp"

#elif defined(CXXTOOLS_ATOMICITY_SYMBIAN)
    #include "atomicity.cw.x86.cpp"

#elif defined(CXXTOOLS_ATOMICITY_PTHREAD)
    #include "atomicity.pthread.cpp"
#else
    #include "atomicity.generic.cpp"
#endif



