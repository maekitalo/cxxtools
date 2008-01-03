/* quotedprintablestream.cpp
 *
 * cxxtools - general purpose C++-toolbox
 * Copyright (C) 2007 Tommi Maekitalo
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#include "cxxtools/quotedprintablestream.h"

namespace cxxtools
{

std::streambuf::int_type QuotedPrintable_streambuf::overflow(std::streambuf::int_type ch)
{
  if (ch > 32 && ch < 128)
  {
    sinksource->sputc(ch);
    if (++col > 76)
    {
      sinksource->sputc('\n');
      col = 0;
    }
  }
  else if (ch == 32)
  {
    sinksource->sputc(ch);
    if (++col > 70)
    {
      sinksource->sputc('=');
      sinksource->sputc('\n');
      col = 0;
    }
  }
  else if (ch == '\n')
  {
    sinksource->sputc('\n');
    col = 0;
  }
  else
  {
    if (col > 73)
    {
      sinksource->sputc('=');
      sinksource->sputc('\n');
      col = 0;
    }

    static const char hex[] = "0123456789ABCDEF";
    sinksource->sputc('=');
    sinksource->sputc(hex[(ch >> 4) & 0xf]);
    sinksource->sputc(hex[ch & 0xf]);

    if (++col > 73)
    {
      sinksource->sputc('=');
      sinksource->sputc('\n');
      col = 0;
    }
  }

  return 0;
}

std::streambuf::int_type QuotedPrintable_streambuf::underflow()
{
  return traits_type::eof();
}

int QuotedPrintable_streambuf::sync()
{
  return sinksource->pubsync();
}

}
