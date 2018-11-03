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

/**
 * This demo shows, how to create a child-process, use a pipe to signal
 * from child to parent and another pipe to send a datastream.
 */
#include <cxxtools/pipe.h>
#include <cxxtools/posix/pipestream.h>
#include <cxxtools/posix/fork.h>
#include <cxxtools/thread.h>

int main(int argc, char* argv[])
{
  try
  {
    // create pipe, where child signals, that he is initialized
    cxxtools::posix::Pipe pipe;

    cxxtools::posix::Pipestream pstream;

    // fork child-process
    cxxtools::posix::Fork fork;

    if (fork.parent())
    {
      pipe.closeWriteFd();
      pstream.closeWriteFd();

      std::cout << "waiting for child to become ready" << std::endl;

      char ch = pipe.read();
      std::cout << "child is ready - he sent '" << ch << '\'' << std::endl;

      // now we copy everything, the child sends through the stream
      std::cout << pstream.rdbuf() << std::flush;

      fork.wait();

      std::cout << "child terminated normally" << std::endl;
    }
    else // child
    {
      pipe.closeReadFd();
      pstream.closeReadFd();

      // we simulate some long initialization:
      cxxtools::Thread::sleep(cxxtools::Seconds(1));

      pipe.write('a');

      // make another break
      cxxtools::Thread::sleep(cxxtools::Seconds(1));

      pstream << "Hello World!" << std::endl;
    }
  }
  catch (const std::exception& e)
  {
    std::cerr << e.what() << std::endl;
  }
}
