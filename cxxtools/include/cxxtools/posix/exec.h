/*
 * Copyright (C) 2011 Tommi Maekitalo
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

#include <cxxtools/noncopyable.h>
#include <cxxtools/systemerror.h>
#include <stdexcept>
#include <string>
#include <unistd.h>

namespace cxxtools
{
  namespace posix
  {
    /** cxxtools::posix::Exec is a wrapper around the exec?? functions of posix.

        Running one of the exec-variants of a posix system is difficult and
        error prone to do it right. This class tries to make it as easy as
        possible.

        Usage is like this:
        \code
          cxxtools::posix::Exec e("ls");
          e.push_back("-l");
          e.exec();
        \endcode

        This replaces the current process with the unix command "ls -l".
     */
    template <unsigned dataSize, unsigned maxArgs>
    class BasicExec : private cxxtools::NonCopyable
    {
        char data[dataSize];
        char* args[maxArgs + 2];
        unsigned argc;

      public:
        typedef const std::string& const_reference;
        typedef std::string& reference;

        explicit BasicExec(const std::string& cmd)
          : argc(0)
        {
          if (cmd.size() >= dataSize - 1)
            throw std::out_of_range("command <" + cmd + "> too large");
          args[0] = &data[0];
          cmd.copy(args[0], cmd.size());
          args[0][cmd.size()] = '\0';
          args[1] = args[0] + cmd.size() + 1;
        }

        void push_back(const std::string& arg)
        {
          if (args[argc + 1] + arg.size() - data >= dataSize)
            throw std::out_of_range("argument list too long");
          if (argc >= maxArgs)
            throw std::out_of_range("too many arguments");

          ++argc;
          arg.copy(args[argc], arg.size());
          args[argc][arg.size()] = '\0';
          args[argc + 1] = args[argc] + arg.size() + 1;
        }

        void exec()
        {
          args[argc + 1] = 0;
          ::execvp(args[0], args);
          throw cxxtools::SystemError("execvp");
        }
    };

    typedef BasicExec<0xffff, 256> Exec;
  }
}
