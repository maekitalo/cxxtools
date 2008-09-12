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
 *
 */

#include "cxxtools/uuencode.h"

namespace cxxtools
{

static char cv[65] = "`!\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_";

void Uuencode_streambuf::begin(const std::string& filename, unsigned mode)
{
  sinksource->sputn("begin ", 6);
  sinksource->sputc(((mode >> 6) & 0x7) + '0');
  sinksource->sputc(((mode >> 3) & 0x7) + '0');
  sinksource->sputc((mode & 0x7) + '0');
  sinksource->sputc(' ');
  sinksource->sputn(filename.data(), filename.size());
  sinksource->sputc('\n');
  inStream = true;
}

void Uuencode_streambuf::end()
{
  if (pbase() != pptr())
  {
    sinksource->sputc(cv[pptr() - pbase()]);
    for (const char* p = pbase(); p < pptr(); p += 3)
    {
      char A = p[0];
      char B = p < pptr() - 1 ? p[1] : 0;
      char C = p < pptr() - 2 ? p[2] : 0;
      sinksource->sputc(cv[(A >> 2) & 0x3F]);
      sinksource->sputc(cv[((A << 4) | ((B >> 4) & 0xF)) & 0x3F]);
      sinksource->sputc(cv[((B << 2) | ((C >> 6) & 0x3)) & 0x3F]);
      sinksource->sputc(cv[( C                         ) & 0x3F]);
    }

    sinksource->sputn("\n`\n", 3);

    setp(obuffer, obuffer + length);
  }

  if (inStream)
  {
    sinksource->sputn("end\n", 4);
    inStream = false;
  }
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
