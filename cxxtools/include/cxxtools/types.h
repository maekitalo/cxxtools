/***************************************************************************
 *   Copyright (C) 2004 Marc Boris Duerner                                 *
 *   Copyright (C) 2005 Aloysius Indrayanto                                *
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
