/* cxxtools/fork.h
 *
 * cxxtools - general purpose C++-toolbox
 * Copyright (C) 2007 Tommi Maekitalo
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

#ifndef CXXTOOLS_FORK_H
#define CXXTOOLS_FORK_H

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <cxxtools/syserror.h>

namespace cxxtools
{
  class Fork
  {
      Fork(const Fork&);
      Fork& operator= (const Fork&);

      pid_t pid;

    public:
      Fork()
        : pid(::fork())
      {
        if (pid < 0)
          throw SysError("fork");
      }
      ~Fork()
      {
        if (pid)
          wait();
      }

      pid_t getPid() const  { return pid; }
      bool parent() const   { return pid > 0; }
      bool child() const    { return !parent(); }
      void setNowait()      { pid = 0; }
      int wait(int options = 0)
      {
        int status;
        ::waitpid(pid, &status, options);
        pid = 0;
        return status;
      }
  };
}

#endif // CXXTOOLS_FORK_H
