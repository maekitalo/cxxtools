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
  /** A simple wrapper for the system function fork(2).
   *
   *  The advantage of using this class instead of fork directly is, easyness,
   *  robustness and readability due to less code. The constructor executes
   *  fork(2) and does error checking. The destructor waits for the child
   *  process, which prevents the creation of zombie processes. The user may
   *  decide to deactivate it or waiting explicitely to receive the return
   *  status, but this has to be done explicitely, which helps robustness.
   *
   *  Example:
   *  \code
   *    {
   *      cxxtools::Fork process;
   *      if (process.child())
   *      {
   *        // we are in the child process here.
   *
   *        exit(0);  // normally the child either exits or execs an other
   *                  // process
   *      }
   *    }
   *  \endcode
   */
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
