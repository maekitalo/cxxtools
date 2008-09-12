/*
 * Copyright (C) 2007 Tommi Maekitalo
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
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
 * This program is distributed in the hope that it will be useful, but
 * is provided AS IS, WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, and
 * NON-INFRINGEMENT.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 *
 */

#ifndef CXXTOOLS_BYTEORDER_H
#define CXXTOOLS_BYTEORDER_H

#include <algorithm>
#include <endian.h>

#if __BYTE_ORDER == __LITTLE_ENDIAN
  #define CXXTOOLS_LITTLE_ENDIAN
#elif __BYTE_ORDER == __BIG_ENDIAN
  #define CXXTOOLS_BIG_ENDIAN
#endif

namespace cxxtools
{
  template <typename T, unsigned sizeofT>
  class Reverser;

  template <typename T>
  class Reverser<T, 2>
  {
    public:
      T operator() (T value)
      {
        return value >> 8 & 0xff
             | value << 8 & 0xff00;
      }
  };

  template <typename T>
  class Reverser<T, 4>
  {
    public:
      T operator() (T value)
      {
        return value >> 24 & 0xff
             | value >>  8 & 0xff00
             | value <<  8 & 0xff0000
             | value << 24 & 0xff000000;
      }
  };

  template <typename T>
  class Reverser<T, 8>
  {
    public:
      T operator() (T value)
      {
        return value >> 56 & 0xffll
             | value >> 40 & 0xff00ll
             | value >> 24 & 0xff0000ll
             | value >>  8 & 0xff000000ll
             | value <<  8 & 0xff00000000ll
             | value << 24 & 0xff0000000000ll
             | value << 40 & 0xff000000000000ll
             | value << 56 & 0xff00000000000000ll;
      }
  };

  template <typename T>
  T reverse(T value)
  {
    return Reverser<T, sizeof(T)>() (value);
  }

  /// Returns true, if machine is big-endian (high byte first).
  /// e.g. PowerPC
  inline bool isBigEndian()
  {
    const int i = 1;
    return *reinterpret_cast<const int8_t*>(&i) == 0;
  }

  /// Returns true, if machine is little-endian (low byte first).
  /// e.g. x86
  inline bool isLittleEndian()
  {
    const int i = 1;
    return *reinterpret_cast<const int8_t*>(&i) == 1;
  }

  /// Converts a native value in little endian.
  template <typename T>
  T hostToLe(T value)
  {
#if defined(CXXTOOLS_BIG_ENDIAN)
    return reverse(value);
#elif defined(CXXTOOLS_LITTLE_ENDIAN)
    return value;
#else
    return isBigEndian() ? reverse(value) : value;
#endif
  }

  /// Converts a little endian value to native.
  template <typename T>
  T leToHost(T value)
  {
#if defined(CXXTOOLS_BIG_ENDIAN)
    return reverse(value);
#elif defined(CXXTOOLS_LITTLE_ENDIAN)
    return value;
#else
    return isBigEndian() ? reverse(value) : value;
#endif
  }

  /// Converts a native value in big endian.
  template <typename T>
  T hostToBe(T value)
  {
#if defined(CXXTOOLS_BIG_ENDIAN)
    return value;
#elif defined(CXXTOOLS_LITTLE_ENDIAN)
    return reverse(value);
#else
    return isBigEndian() ? value : reverse(value);
#endif
  }

  /// Converts a big endian value to native.
  template <typename T>
  T beToHost(T value)
  {
#if defined(CXXTOOLS_BIG_ENDIAN)
    return value;
#elif defined(CXXTOOLS_LITTLE_ENDIAN)
    return reverse(value);
#else
    return isBigEndian() ? value : reverse(value);
#endif
  }

}

#endif // CXXTOOLS_BYTEORDER_H
