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

        BasicExec& push_back(const std::string& arg)
        {
          if (static_cast<unsigned>(args[argc + 1] + arg.size() - data) >= dataSize)
            throw std::out_of_range("argument list too long");
          if (argc >= maxArgs)
            throw std::out_of_range("too many arguments");

          ++argc;
          arg.copy(args[argc], arg.size());
          args[argc][arg.size()] = '\0';
          args[argc + 1] = args[argc] + arg.size() + 1;

          return *this;
        }

        // nice alias of push_back
        BasicExec& arg(const std::string& arg)
        { return push_back(arg); }

        void exec()
        {
          args[argc + 1] = 0;
          ::execvp(args[0], args);
          throw SystemError("execvp");
        }

    };

    typedef BasicExec<0x8000, 256> Exec;
  }
}
