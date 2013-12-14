/*
 * Copyright (C) 2007 Tommi Maekitalo
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

#include "cxxtools/quotedprintablestream.h"

namespace cxxtools
{

std::streambuf::int_type QuotedPrintable_streambuf::overflow(std::streambuf::int_type ch)
{
  if (ch >= 32 && ch < 128 && ch != '=')
  {
    if (++col > 75)
    {
      sinksource->sputc('=');
      sinksource->sputc('\n');
      col = 0;
    }

    sinksource->sputc(ch);
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
      col = 3;
    }
    else
      col += 3;

    static const char hex[] = "0123456789ABCDEF";
    sinksource->sputc('=');
    sinksource->sputc(hex[(ch >> 4) & 0xf]);
    sinksource->sputc(hex[ch & 0xf]);
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
