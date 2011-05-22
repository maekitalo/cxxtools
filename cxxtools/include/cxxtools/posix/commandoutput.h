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

#ifndef CXXTOOLS_POSIX_COMMANDOUTPUT_H
#define CXXTOOLS_POSIX_COMMANDOUTPUT_H

#include <cxxtools/posix/exec.h>
#include <cxxtools/posix/pipestream.h>
#include <cxxtools/posix/fork.h>

namespace cxxtools
{
  namespace posix
  {
    /**
     cxxtools::posix::CommandOutput starts a process and the stdout of the process can be read.

     The class is derived from std::istream, so all methods provided from that
     base class can be used to read the output.

     Typical usage example:
     \code
       cxxtools::posix::CommandOutput ls("ls");
       ls.push_back("-l");   // add a parameter
       ls.push_back("/bin"); // and another one
       ls.run();             // start the process

       // read output line by line
       std::string line;
       while (std::getline(ls, line))
         std::cout << line << std::endl;
     \endcode
     */
    class CommandOutput : public std::istream
    {
        Fork _fork;
        Exec _exec;
        Pipestreambuf streambuf;

      public:
        typedef Exec::const_reference const_reference;
        typedef Exec::reference reference;

        explicit CommandOutput(const std::string& cmd, unsigned bufsize = 8192)
          : _fork(false),
            _exec(cmd),
            streambuf(bufsize)
        {
          init(&streambuf);
        }

        void push_back(const std::string& arg)
        { _exec.push_back(arg); }

        void run(bool combineStderr = false);

        int wait(int options = 0);

        IODevice& in()
        { return streambuf.in(); }

        void close()
        { streambuf.closeReadFd(); }

    };

  }
}

#endif // CXXTOOLS_POSIX_COMMANDOUTPUT_H
