/*
 * Copyright (C) 2004 Marc Boris Duerner
 * Copyright (C) 2005 Aloysius Indrayanto
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
#ifndef cxxtools_Types_h
#define cxxtools_Types_h

#include <climits>
#include <cstddef>

namespace cxxtools {

    /// \exclude
    typedef unsigned char  uchar;
    /// \exclude
    typedef unsigned short ushort;
    /// \exclude
    typedef unsigned int   uint;
    /// \exclude
    typedef unsigned long  ulong;

    /// \exclude
    using std::size_t;
    /// \exclude
    typedef std::ptrdiff_t ssize_t;

    /// \exclude
    typedef signed char int8_t;
    /// \exclude
    typedef unsigned char uint8_t;

#if USHRT_MAX == 0xffff
    /// \exclude
    typedef short int16_t;
    /// \exclude
    typedef unsigned short uint16_t;
#elif UINT_MAX == 0xffff
    /// \exclude
    typedef int int16_t;
    /// \exclude
    typedef unsigned int uint16_t;
#elif ULONG_MAX == 0xffff
    /// \exclude
    typedef long int16_t;
    /// \exclude
    typedef unsigned long uint16_t;
#endif

#if USHRT_MAX == 0xffffffffUL
    /// \exclude
    typedef short int32_t;
    /// \exclude
    typedef unsigned short uint32_t;
#elif UINT_MAX == 0xffffffffUL
    /// \exclude
    typedef int int32_t;
    /// \exclude
    typedef unsigned int uint32_t;
#elif ULONG_MAX == 0xffffffffUL
    /// \exclude
    typedef long int32_t;
    /// \exclude
    typedef unsigned long uint32_t;
#endif

#if UINT_MAX == 18446744073709551615ULL
    #define CXXTOOLS_WITH_INT64 1
    /// \exclude
    typedef int int64_t;
    /// \exclude
    typedef unsigned int uint64_t;
#elif ULONG_MAX == 18446744073709551615ULL
    #define CXXTOOLS_WITH_INT64 1
    /// \exclude
    typedef long int64_t;
    /// \exclude
    typedef unsigned long uint64_t;
#elif ULLONG_MAX == 18446744073709551615ULL
    #define CXXTOOLS_WITH_INT64 1
    /// \exclude
    typedef long long int64_t;
    /// \exclude
    typedef unsigned long long uint64_t;
#elif defined(__GNUC__) || defined(__MWERKS_SYMBIAN__)
    #define CXXTOOLS_WITH_INT64 1
    /// \exclude
    typedef long long int64_t;
    /// \exclude
    typedef unsigned long long uint64_t;
#endif

} // namespace cxxtools

#endif
