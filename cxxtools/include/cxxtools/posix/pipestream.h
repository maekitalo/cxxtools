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

#ifndef CXXTOOLS_POSIX_PIPESTREAM_H
#define CXXTOOLS_POSIX_PIPESTREAM_H

#include <iostream>
#include <cxxtools/posix/pipe.h>

namespace cxxtools
{
namespace posix
{
  /** @brief Simple unix pipe iostream

      This iostream works only on POSIX systems. A portable alternative is
      to use an IOStream attached to the input or output end of a Pipe.
  */
  class Pipestreambuf : public std::streambuf
  {
      std::streambuf::int_type overflow(std::streambuf::int_type ch);
      std::streambuf::int_type underflow();
      int sync();

      posix::Pipe pipe;

      unsigned bufsize;
      char* ibuffer;
      char* obuffer;

    public:
      explicit Pipestreambuf(unsigned bufsize = 8192);
      ~Pipestreambuf();

      void closeReadFd()      { sync(); pipe.closeReadFd(); }
      void closeWriteFd()     { sync(); pipe.closeWriteFd(); }

      int getReadFd() const   { return pipe.getReadFd(); }
      int getWriteFd() const  { return pipe.getWriteFd(); }

      void redirectStdout(bool close = true)   { pipe.redirectStdout(close); }
      void redirectStdin(bool close = true)    { pipe.redirectStdin(close); }
      void redirectStderr(bool close = true)   { pipe.redirectStderr(close); }

      IODevice& out()
      { return pipe.out(); }

      IODevice& in()
      { return pipe.in(); }

  };

  class Pipestream : public std::iostream
  {
      Pipestreambuf streambuf;

    public:
      explicit Pipestream(unsigned bufsize = 8192)
        : std::iostream(0),
          streambuf(bufsize)
      {
        init(&streambuf);
      }

      void closeReadFd()        { streambuf.closeReadFd(); }
      void closeWriteFd()       { streambuf.closeWriteFd(); }
      void close()
      {
        closeReadFd();
        closeWriteFd();
      }

      int getReadFd() const   { return streambuf.getReadFd(); }
      int getWriteFd() const  { return streambuf.getWriteFd(); }

      void redirectStdout(bool close = true)   { streambuf.redirectStdout(close); }
      void redirectStdin(bool close = true)    { streambuf.redirectStdin(close); }
      void redirectStderr(bool close = true)   { streambuf.redirectStderr(close); }

      IODevice& out()
      { return streambuf.out(); }

      IODevice& in()
      { return streambuf.in(); }

  };
}
}

#endif // CXXTOOLS_POSIX_PIPESTREAM_H
