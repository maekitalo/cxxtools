/* cxxtools/pipestream.h
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

#ifndef CXXTOOLS_PIPESTREAM_H
#define CXXTOOLS_PIPESTREAM_H

#include <iostream>
#include <cxxtools/pipe.h>

namespace cxxtools
{
  class Pipestreambuf : public std::streambuf
  {
      std::streambuf::int_type overflow(std::streambuf::int_type ch);
      std::streambuf::int_type underflow();
      int sync();

      Pipe pipe;

      unsigned bufsize;
      char* ibuffer;
      char* obuffer;

    public:
      explicit Pipestreambuf(unsigned bufsize = 8192);
      ~Pipestreambuf();

      void closeReadFd()      { pipe.closeReadFd(); }
      void closeWriteFd()     { pipe.closeWriteFd(); }

      int getReadFd() const   { return pipe.getReadFd(); }
      int getWriteFd() const  { return pipe.getWriteFd(); }
  };

  class Pipestream : public std::iostream
  {
      Pipestreambuf streambuf;

    public:
      explicit Pipestream(unsigned bufsize = 8192)
        : std::iostream(),
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
  };
}

#endif // CXXTOOLS_PIPESTREAM_H
