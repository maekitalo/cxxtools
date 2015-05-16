/*
 * Copyright (C) 2013 Tommi Maekitalo
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

#include "dateutils.h"
#include <stdexcept>
#include <cctype>

namespace cxxtools
{
  void skipNonDigit(std::string::const_iterator& b, std::string::const_iterator e)
  {
    while (b != e && !std::isdigit(*b))
      ++b;
  }

  unsigned getUnsigned(std::string::const_iterator& b, std::string::const_iterator e, unsigned digits)
  {
    if (b == e || !std::isdigit(*b))
      throw std::runtime_error("invalid date format");

    unsigned ret = 0;
    for (unsigned d = 0; d < digits && b < e && std::isdigit(*b); ++d, ++b)
      ret = ret * 10 + (*b - '0');

    return ret;
  }

  unsigned getInt(std::string::const_iterator& b, std::string::const_iterator e, unsigned digits)
  {
    int sgn = 1;

    if (b != e)
    {
      if (*b == '-')
      {
        sgn = -1;
        ++b;
      }
      else if (*b == '+')
        ++b;
    }

    if (b == e || !std::isdigit(*b))
      throw std::runtime_error("invalid date format");

    int ret = 0;
    for (unsigned d = 0; d < digits && b < e && std::isdigit(*b); ++d, ++b)
      ret = ret * 10 + (*b - '0');

    return ret * sgn;
  }

  unsigned getMicroseconds(std::string::const_iterator& b, std::string::const_iterator e)
  {
    unsigned m = 0;
    unsigned d = 100000;

    for (unsigned c = 0; c < 6; ++c)
    {
      if (b == e || !std::isdigit(*b))
        return m;

      m += (*b++ - '0') * d;
      d /= 10;
    }

    return m;
  }

  void appendDn(std::string& s, unsigned short n, unsigned v)
  {
      s.resize(s.size() + n);
      for (unsigned short i = 0; i < n; ++i)
      {
        s[s.size() - i - 1] = '0' + v % 10;
        v /= 10;
      }
  }
}
