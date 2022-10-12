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

#ifndef CXXTOOLS_POSIX_COMMANDINPUT_H
#define CXXTOOLS_POSIX_COMMANDINPUT_H

#include <cxxtools/posix/exec.h>
#include <cxxtools/posix/pipestream.h>
#include <cxxtools/posix/fork.h>

namespace cxxtools
{
  namespace posix
  {
    /**
     cxxtools::posix::CommandInput starts a process and the stdin is connected to the current process.

     The class is derived from std::ostream. You can use all methods provided
     from that class to send data to the stdin of the process.

     Typical usage example:
     \code
       cxxtools::posix::CommandInput grep("grep");
       grep.push_back("foo");  // add a parameter
       grep.run();             // runs the process
       grep << "foo" << std::endl;     // this line is printed
       grep << "bar" << std::endl;     // this line is not printed
       grep << "foobar" << std::endl;     // this line is printed
     \endcode
     */
    class CommandInput : public std::ostream
    {
        Fork _fork;
        Exec _exec;
        Pipestreambuf streambuf;

      public:
        explicit CommandInput(const std::string& cmd, unsigned bufsize = 8192)
          : std::ostream(&streambuf),
            _fork(false),
            _exec(cmd),
            streambuf(bufsize)
        {
        }

        void push_back(const std::string& arg)
        { _exec.push_back(arg); }

        void run();

        int wait(int options = 0)
        { return _fork.wait(options); }

        IODevice& out()
        { return streambuf.out(); }

        void close()
        { streambuf.closeWriteFd(); }

        pid_t pid() const
        { return _fork.getPid(); }

        // signal is sent from child process before exec
        Signal<> child;
    };

  }
}

#endif // CXXTOOLS_POSIX_COMMANDINPUT_H
