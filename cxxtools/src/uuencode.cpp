/* uuencode.cpp
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

#include "cxxtools/uuencode.h"

namespace cxxtools
{

static char cv[65] = "`!\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_";

void Uuencode_streambuf::end()
{
  if (pbase() != pptr())
  {
    sinksource->sputc(cv[pptr() - pbase()]);
    for (const char* p = pbase(); p < pptr(); p += 3)
    {
      char A = p[0];
      char B = p[1];
      char C = p[2];
      sinksource->sputc(cv[(A >> 2) & 0x3F]);
      sinksource->sputc(cv[((A << 4) | ((B >> 4) & 0xF)) & 0x3F]);
      sinksource->sputc(cv[((B << 2) | ((C >> 6) & 0x3)) & 0x3F]);
      sinksource->sputc(cv[( C                         ) & 0x3F]);
    }
    sinksource->sputc('\n');
  }
  sinksource->sputc('`');
  sinksource->sputc('\n');

  setp(obuffer, obuffer + length);
}

std::streambuf::int_type Uuencode_streambuf::overflow(std::streambuf::int_type ch)
{
  if (pbase() != epptr())
  {
    sinksource->sputc(cv[pptr() - pbase()]);
    for (const char* p = pbase(); p < pptr(); p += 3)
    {
      char A = p[0];
      char B = p[1];
      char C = p[2];
      sinksource->sputc(cv[(A >> 2) & 0x3F]);
      sinksource->sputc(cv[((A << 4) | ((B >> 4) & 0xF)) & 0x3F]);
      sinksource->sputc(cv[((B << 2) | ((C >> 6) & 0x3)) & 0x3F]);
      sinksource->sputc(cv[( C                         ) & 0x3F]);
    }

    sinksource->sputc('\n');
  }

  setp(obuffer, obuffer + length);

  if (ch != traits_type::eof())
  {
    *pptr() = (char_type)ch;
    pbump(1);
  }

  return 0;
}

std::streambuf::int_type Uuencode_streambuf::underflow()
{
  return traits_type::eof();
}

int Uuencode_streambuf::sync()
{
  return 0;
}

}
