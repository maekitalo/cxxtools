/* hirestime.cpp
   Copyright (C) 2005 Tommi Maekitalo

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
along with tntnet; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330,
Boston, MA  02111-1307  USA
*/

#include "cxxtools/hirestime.h"
#include <iostream>

namespace cxxtools
{
  HiresTime& HiresTime::operator+= (const HiresTime& ht)
  {
    tv.tv_sec += ht.tv.tv_sec;
    tv.tv_usec += ht.tv.tv_usec;
    if (tv.tv_usec >= 1000000)
    {
      tv.tv_usec -= 1000000;
      ++tv.tv_sec;
    }
    return *this;
  }

  HiresTime& HiresTime::operator-= (const HiresTime& ht)
  {
    if (ht.tv.tv_usec > tv.tv_usec)
    {
      tv.tv_sec  = tv.tv_sec - ht.tv.tv_sec - 1;
      tv.tv_usec = tv.tv_usec + 1000000 - ht.tv.tv_usec;
    }
    else
    {
      tv.tv_sec  = tv.tv_sec - ht.tv.tv_sec;
      tv.tv_usec = tv.tv_usec + ht.tv.tv_usec;
    }
    return *this;
  }

  std::ostream& operator<< (std::ostream& out, const HiresTime& ht)
  {
    out << ht.getSeconds();
    return out;
  }

}
