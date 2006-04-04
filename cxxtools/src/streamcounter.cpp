/* tee.cpp
   Copyright (C) 2006 Tommi Maekitalo

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

#include "cxxtools/streamcounter.h"

namespace cxxtools
{

std::streambuf::int_type StreamcounterBuf::overflow(std::streambuf::int_type ch)
{
  ++count;
  return 0;
}

std::streambuf::int_type StreamcounterBuf::underflow()
{
  return traits_type::eof();
}

int StreamcounterBuf::sync()
{
  return 0;
}

}
