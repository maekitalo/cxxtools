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
#include <string.h>

namespace cxxtools
{
  const char* weekdaynames[7] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };

  const char* monthnames[12] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };

  void skipNonDigit(std::string::const_iterator& b, std::string::const_iterator e)
  {
    while (b != e && !std::isdigit(*b))
      ++b;
  }

  void skipWord(std::string::const_iterator& b, std::string::const_iterator e)
  {
    while (b != e && std::isalnum(*b))
      ++b;
  }

  unsigned getUnsigned(std::string::const_iterator& b, std::string::const_iterator e, unsigned digits)
  {
    if (b == e || !std::isdigit(*b))
      throw std::invalid_argument("invalid date format");

    unsigned ret = 0;
    for (unsigned d = 0; d < digits && b != e && std::isdigit(*b); ++d, ++b)
      ret = ret * 10 + (*b - '0');

    return ret;
  }

  unsigned getUnsignedF(std::string::const_iterator& b, std::string::const_iterator e, unsigned digits)
  {
    unsigned ret = 0;
    for (unsigned d = 0; d < digits; ++d, ++b)
    {
      if (b == e || !std::isdigit(*b))
        throw std::invalid_argument("invalid date format");
      ret = ret * 10 + (*b - '0');
    }

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
      throw std::invalid_argument("invalid date format");

    int ret = 0;
    for (unsigned d = 0; d < digits && b != e && std::isdigit(*b); ++d, ++b)
      ret = ret * 10 + (*b - '0');

    return ret * sgn;
  }

  unsigned getMicroseconds(std::string::const_iterator& b, std::string::const_iterator e, unsigned digits)
  {
    unsigned m = 0;
    unsigned d = 100000;

    for (unsigned c = 0; c < digits; ++c)
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

  void appendDn(std::string& s, unsigned short n, int v)
  {
      if (v < 0)
      {
          s += '-';
          appendDn(s, n, static_cast<unsigned>(-v));
      }
      else
          appendDn(s, n, static_cast<unsigned>(v));
  }

  unsigned getMonthFromName(std::string::const_iterator& b, std::string::const_iterator e)
  {
    char m[4];
    unsigned d;
    for (d = 0; d < 3 && b != e && std::isalpha(*b); ++d)
      m[d] = *b++;
    if (d != 3)
      throw std::invalid_argument("invalid date format - monthname expected");

    m[3] = '\0';
    for (d = 0; d < 12; ++d)
    {
      if (strcasecmp(monthnames[d], m) == 0)
        return d + 1;
    }

    throw std::invalid_argument("invalid date format - monthname expected");
  }

}
