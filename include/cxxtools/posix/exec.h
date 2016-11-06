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

        Usage is like this:
        \code
          cxxtools::posix::Exec e("ls");
          e.push_back("-l");
          e.exec();
        \endcode

        This replaces the current process with the unix command "ls -l".
     */
    class Exec
    {
        std::vector<std::string> _args;

      public:
        explicit Exec(const std::string& cmd)
        {
          _args.push_back(cmd);
        }

        Exec& push_back(const std::string& arg)
        {
          _args.push_back(arg);
          return *this;
        }

        // nice alias of push_back
        Exec& arg(const std::string& arg)
        { return push_back(arg); }

        void exec()
        {
          std::vector<const char*> argv;
          for (unsigned n = 0; n < _args.size(); ++n)
              argv.push_back(_args[n].c_str());
          argv.push_back(0);
          ::execvp(argv[0], (char**)&argv[0]);
          throw SystemError("execvp");
        }

    };
  }
}

#endif
