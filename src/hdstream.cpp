/*
 * Copyright (C) 2003 Tommi Maekitalo
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

#include "cxxtools/hdstream.h"
#include <ios>
#include <iomanip>
#include <cctype>
#include <cstdio>

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
    out << std::setw(7) << std::setfill('0') << std::hex << offset << '|';
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
  return traits_type::eof();
}

int Hdstreambuf::sync()
{
  return overflow(traits_type::eof());
}

}
