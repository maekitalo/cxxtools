/* hdstream.cpp
 *
 * cxxtools - general purpose C++-toolbox
 * Copyright (C) 2003 Tommi Maekitalo
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

#include "cxxtools/hdstream.h"
#include <iomanip>
#include <cctype>

namespace cxxtools
{
const unsigned Hdstreambuf::BUFFERSIZE;

std::streambuf::int_type Hdstreambuf::overflow(std::streambuf::int_type ch)
{
  static char hexdigit[] = "0123456789abcdef";

  std::ostream out(Dest);
  size_t count = pptr() - pbase();
  if(count > 0)
  {
    out << std::setw(7) << std::hex << offset << '|';
    offset += count;
    size_t i;
    for(i = 0; i < count; ++i)
    {
      unsigned char ch = static_cast<unsigned char>(pbase()[i]);
      out << hexdigit[ch >> 4] << hexdigit[ch & 0xf]
          << (i == 7 ? ':' : ' ');
    }
    for( ; i < BUFFERSIZE; ++i)
      out << "   ";
    out << '|';
    for(i = 0; i < count; ++i)
      out << (char)(std::isprint(pbase()[i]) ? pbase()[i] : '.');
    for( ; i < BUFFERSIZE; ++i)
      out << ' ';
    out << std::endl;
  }

  setp(pbase(), epptr());
  if (ch != -1)
    return sputc(ch);
  return 0;
}

std::streambuf::int_type Hdstreambuf::underflow()
{
  return EOF;
}

int Hdstreambuf::sync()
{
  return overflow(EOF);
}

}
