/* hdstream.cpp
   Copyright (C) 2003 Tommi Mäkitalo

This file is part of cxxtools.

Cxxtools is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

Cxxtools is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Cxxtools; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330,
Boston, MA  02111-1307  USA
*/

#include "cxxtools/hdstream.h"
#include <iomanip>

namespace cxxtools
{

int hdstreambuf::overflow(int ch)
{
  using namespace std;

  ostream out(Dest);
  out.fill('0');
  size_t count = pptr() - pbase();
  if(count > 0)
  {
    out << setw(7) << hex << offset << '|';
    offset += count;
    size_t i;
    for(i = 0; i < count; ++i)
      out << setw(2) << hex
        << (unsigned int)(unsigned char)pbase()[i] << (i == 7 ? ':' : ' ');
    for( ; i < BUFFERSIZE; ++i)
      out << "   ";
    out << '|';
    for(i = 0; i < count; ++i)
      out << (char)(isprint(pbase()[i]) ? pbase()[i] : '.');
    for( ; i < BUFFERSIZE; ++i)
      out << ' ';
    out << endl;
  }

  setp(pbase(), epptr());
  if (ch != -1)
    sputc(ch);
  return 0;
}

int hdstreambuf::underflow()
{
  return EOF;
}

int hdstreambuf::sync()
{
  return overflow(EOF);
}

}
