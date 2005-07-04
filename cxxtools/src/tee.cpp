/* tee.cpp
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

#include "cxxtools/tee.h"

namespace cxxtools
{

std::streambuf::int_type teestreambuf::overflow(std::streambuf::int_type ch)
{
  if(ch != traits_type::eof())
  {
    if(streambuf1 && streambuf1->sputc(ch) == traits_type::eof())
      return traits_type::eof();

    if(streambuf2 && streambuf2->sputc(ch) == traits_type::eof())
      return traits_type::eof();
  }

  return 0;
}

std::streambuf::int_type teestreambuf::underflow()
{
  return traits_type::eof();
}

int teestreambuf::sync()
{
  if(streambuf1 && streambuf1->pubsync() == traits_type::eof())
    return traits_type::eof();

  if(streambuf2 && streambuf2->pubsync() == traits_type::eof())
    return traits_type::eof();

  return 0;
}

/////////////////////////////////////////////////////////////////////////////
void tee::assign(std::ostream& s1, std::ostream& s2)
{
  teestreambuf* buf = dynamic_cast<teestreambuf*>(rdbuf());
  if(buf)
    buf->tie(s1.rdbuf(), s2.rdbuf());
}

void tee::assign_single(std::ostream& s)
{
  teestreambuf* buf = dynamic_cast<teestreambuf*>(rdbuf());
  if(buf)
    buf->tie(s.rdbuf());
}

}
