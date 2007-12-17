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
#include <map>

namespace cxxtools
{

static char cv[65] = "`!\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_";
typedef std::map<char, unsigned> vc_type;
static vc_type vc;

void Uuencode_streambuf::end()
{
  char A = obuffer[0];
  char B = obuffer[1];
  char C = obuffer[2];
  switch(pptr() - pbase())
  {
    case 1:
      B = 0;
      C = 0;
      putChar(cv[(A >> 2) & 0x3F]);
      putChar(cv[((A << 4) | ((B >> 4) & 0xF)) & 0x3F]);
      putChar('=');
      putChar('=');
      break;

    case 2:
      C = 0;
      putChar(cv[(A >> 2) & 0x3F]);
      putChar(cv[((A << 4) | ((B >> 4) & 0xF)) & 0x3F]);
      putChar(cv[((B << 2) | ((C >> 6) & 0x3)) & 0x3F]);
      putChar('=');
      break;

    case 3:
      putChar(cv[(A >> 2) & 0x3F]);
      putChar(cv[((A << 4) | ((B >> 4) & 0xF)) & 0x3F]);
      putChar(cv[((B << 2) | ((C >> 6) & 0x3)) & 0x3F]);
      putChar(cv[( C                         ) & 0x3F]);
      break;
  }

  setp(obuffer, obuffer + 3);
  indecode = false;
}

std::streambuf::int_type Uuencode_streambuf::overflow(std::streambuf::int_type ch)
{
  if (pptr() != pbase())
  {
    char A = obuffer[0];
    char B = obuffer[1];
    char C = obuffer[2];
    putChar(cv[(A >> 2) & 0x3F]);
    putChar(cv[((A << 4) | ((B >> 4) & 0xF)) & 0x3F]);
    putChar(cv[((B << 2) | ((C >> 6) & 0x3)) & 0x3F]);
    putChar(cv[( C                         ) & 0x3F]);
  }

  setp(obuffer, obuffer + 3);

  if (ch != traits_type::eof())
  {
    *pptr() = (char_type)ch;
    pbump(1);
  }

  indecode = true;

  return 0;
}

std::streambuf::int_type Uuencode_streambuf::underflow()
{
  if (eofflag)
    return traits_type::eof();

  // input:
  // |....,....|....,....|....,....|
  //  <   A   > <   B   > <   C   >
  //  < c1  ><  c2 > <  c3 ><  c4 >
  char& A = decodebuf[0];
  char& B = decodebuf[1];
  char& C = decodebuf[2];

  int c1 = getval();
  if (c1 == -1)
    return traits_type::eof();
  A = (char)(c1 << 2);

  int c2 = getval();
  if (c2 == -1)
    return traits_type::eof();
  A |= (char)(c2 >> 4);
  B = (char)(c2 << 4);

  int c3 = getval();
  if (c3 == -1)
  {
    setg(decodebuf, decodebuf, decodebuf + 1);
    return traits_type::to_int_type(A);
  }
  B |= (char)(c3 >> 2);
  C = (char)(c3 << 6);

  int c4 = getval();
  if (c4 == -1)
  {
    setg(decodebuf, decodebuf, decodebuf + 2);
    return traits_type::to_int_type(A);
  }
  C |= (char)c4;

  setg(decodebuf, decodebuf, decodebuf + 3);

  return traits_type::to_int_type(A);
}

int Uuencode_streambuf::sync()
{
  return 0;
}

void Uuencode_streambuf::putChar(char ch)
{
  sinksource->sputc(ch);
  if (ch == '\n')
    count = 0;
  else if (++count >= 60)
  {
    sinksource->sputc('\n');
    count = 0;
  }
}

int Uuencode_streambuf::getval()
{
  if (vc.empty())
  {
    for (unsigned u = 0; u < 64; ++u)
      vc[cv[u]] = u;
  }

  unsigned count = 0;
  while (true)
  {
    int ret = sinksource->sbumpc();
    if (ret == traits_type::eof())
    {
      eofflag = true;
      return -1;
    }

    char ch = traits_type::to_char_type(ret);
    if (ch == '=')
    {
      if (++count >= 4)
      {
        eofflag = true;
        return -1;
      }
    }
    else
    {
      vc_type::const_iterator it = vc.find(ch);
      if (it != vc.end())
        return it->second;
      else
        count = 0;
    }
  }
}

}
