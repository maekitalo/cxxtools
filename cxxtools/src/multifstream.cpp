/* multifstream.cpp
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

#include "cxxtools/multifstream.h"

namespace cxxtools
{

multifstreambuf::multifstreambuf()
  : current(0)
{
  mglob.gl_pathv = 0;
}

multifstreambuf::multifstreambuf(const char* pattern, int flags)
  : current(0)
{
  glob(pattern, flags, 0, &mglob);

  if (mglob.gl_pathv && mglob.gl_pathv[current])
    file.open(mglob.gl_pathv[current], std::ios::in);
}

multifstreambuf::~multifstreambuf()
{
  if (mglob.gl_pathv)
    globfree(&mglob);
}

std::streambuf::int_type multifstreambuf::overflow(std::streambuf::int_type c)
{
  return traits_type::eof();
}

std::streambuf::int_type multifstreambuf::underflow()
{
  if (mglob.gl_pathv == 0 || mglob.gl_pathv[current] == 0)
    open_next();

  int_type r;
  do
  {
    r = file.sbumpc();
  } while (r == traits_type::eof() && open_next());

  if (r != traits_type::eof())
  {
    ch = static_cast<char>(r);
    setg(&ch, &ch, &ch + 1);
  }

  return r;
}

int multifstreambuf::sync()
{
  return 0;
}

bool multifstreambuf::open_next()
{
  if (file.is_open())
    file.close();

  if (mglob.gl_pathv
   && mglob.gl_pathv[current + 1])
  {
    ++current;
    file.open(mglob.gl_pathv[current], std::ios::in);
    // error-handling is done in underflow()
    return true;
  }
  else
  {
    if (mglob.gl_pathv)
      globfree(&mglob);

    if (!patterns.empty())
    {
      glob(patterns.front().first.c_str(),
           patterns.front().second, 0, &mglob);

      current = 0;
      if (mglob.gl_pathv && mglob.gl_pathv[0])
        file.open(mglob.gl_pathv[0], std::ios::in);
      patterns.pop();
      // error-handling is done in underflow()
      return true;
    }
    else
    {
      mglob.gl_pathv = 0;
      return false;
    }
  }
}

}
