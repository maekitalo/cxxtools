/* tee.cpp
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

#include "cxxtools/tee.h"

namespace cxxtools
{

std::streambuf::int_type Teestreambuf::overflow(std::streambuf::int_type ch)
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

std::streambuf::int_type Teestreambuf::underflow()
{
  return traits_type::eof();
}

int Teestreambuf::sync()
{
  if(streambuf1 && streambuf1->pubsync() == traits_type::eof())
    return traits_type::eof();

  if(streambuf2 && streambuf2->pubsync() == traits_type::eof())
    return traits_type::eof();

  return 0;
}

/////////////////////////////////////////////////////////////////////////////
void Tee::assign(std::ostream& s1, std::ostream& s2)
{
  Teestreambuf* buf = dynamic_cast<Teestreambuf*>(rdbuf());
  if(buf)
    buf->tie(s1.rdbuf(), s2.rdbuf());
}

void Tee::assign_single(std::ostream& s)
{
  Teestreambuf* buf = dynamic_cast<Teestreambuf*>(rdbuf());
  if(buf)
    buf->tie(s.rdbuf());
}

}
