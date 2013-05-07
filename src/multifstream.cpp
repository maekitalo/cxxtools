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
  int ret = glob(pattern, flags, 0, &mglob);

  if (ret == 0 && mglob.gl_pathv && mglob.gl_pathv[current])
    file.open(mglob.gl_pathv[current], std::ios::in);
  else
    mglob.gl_pathv = 0;
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
