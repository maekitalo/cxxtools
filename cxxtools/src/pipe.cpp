/*
 * Copyright (C) 2007 Tommi Maekitalo
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

#include "cxxtools/pipe.h"
#include "cxxtools/syserror.h"
#include <errno.h>
#include <string>

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
    if(::pipe(fd) != 0)
      throw SystemError(errno, "pipe");
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

    if(c < 0)
      throw SystemError(errno, "write");

    return c;
  }

  ssize_t Pipe::read(void* buf, size_t count)
  {
    ssize_t c = ::read(fd[0], buf, count);

    if(c < 0)
      throw SystemError(errno, "read");

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
