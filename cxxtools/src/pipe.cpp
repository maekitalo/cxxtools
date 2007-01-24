/* pipe.cpp
 * Copyright (C) 2007 Tommi Maekitalo
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * is provided AS IS, WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, and
 * NON-INFRINGEMENT.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 *
 */

#include "cxxtools/pipe.h"
#include "cxxtools/syserror.h"

namespace cxxtools
{
  Pipe::~Pipe()
  {
    if (fd[0] >= 0)
      ::close(fd[0]);
    if (fd[1] >= 0)
      ::close(fd[1]);
  }

  void Pipe::create()
  {
    if (::pipe(fd) != 0)
      throw SysError("pipe");
  }

  void Pipe::closeReadFd()
  {
    if (fd[0] >= 0)
    {
      ::close(fd[0]);
      fd[0] = -1;
    }
  }

  void Pipe::closeWriteFd()
  {
    if (fd[1] >= 0)
    {
      ::close(fd[1]);
      fd[1] = -1;
    }
  }

  ssize_t Pipe::write(const void* buf, size_t count)
  {
    ssize_t c = ::write(fd[1], buf, count);
    if (c < 0)
      throw SysError("write pipe");
    return c;
  }

  ssize_t Pipe::read(void* buf, size_t count)
  {
    ssize_t c = ::read(fd[0], buf, count);
    if (c < 0)
      throw SysError("read pipe");
    return c;
  }

  char Pipe::read()
  {
    char ch;
    size_t c = read(&ch, 1);
    if (c <= 0)
      throw std::runtime_error("nothing read from pipe");
    return ch;
  }
}
