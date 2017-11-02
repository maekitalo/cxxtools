/*
 * Copyright (C) 2011 Tommi Maekitalo
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

#ifndef CXXTOOLS_POSIX_EXEC_H
#define CXXTOOLS_POSIX_EXEC_H

#include <cxxtools/systemerror.h>
#include <vector>
#include <string>
#include <unistd.h>

namespace cxxtools
{
  namespace posix
  {
    /** cxxtools::posix::Exec is a wrapper around the exec?? functions of posix.

        Note that after an fork, memory allocations are not allowed in a
        multithreaded application. So arguments must be set befor a possible
        call to fork.

        Usage is like this:
        \code
          cxxtools::posix::Exec e("ls");
          e.push_back("-l");
          // possibly fork here
          e.exec();
        \endcode

        This replaces the current process with the unix command "ls -l".
     */
    class Exec
    {
        std::vector<std::string> _args;
        std::vector<const char*> _argv;

      public:
        explicit Exec(const std::string& cmd)
        {
          _args.push_back(cmd);
          _argv.reserve(1);
        }

        /// Adds a parameter to the process.
        Exec& push_back(const std::string& arg)
        {
          _args.push_back(arg);
          _argv.reserve(_args.size());
          return *this;
        }

        /// Alias for push_back if you prefer that name.
        Exec& arg(const std::string& arg)
        { return push_back(arg); }

        /// Call `execvp` to replace the current process.
        void exec()
        {
          for (unsigned n = 0; n < _args.size(); ++n)
              _argv.push_back(_args[n].c_str());
          _argv.push_back(0);
          ::execvp(_argv[0], (char**)&_argv[0]);

          // `execvp` do not return and hence this won't never be executed.
          // Just in case:
          throw SystemError("execvp");
        }

    };
  }
}

#endif
