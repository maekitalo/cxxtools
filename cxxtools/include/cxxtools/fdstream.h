/*
 * Copyright (C) 2006 Tommi Maekitalo
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

#ifndef CXXTOOLS_FDSTREAM_H
#define CXXTOOLS_FDSTREAM_H

#include <iostream>

namespace cxxtools
{
  class Fdstreambuf : public std::streambuf
  {
      std::streambuf::int_type overflow(std::streambuf::int_type ch);
      std::streambuf::int_type underflow();
      int sync();

      int fd;
      bool doClose;
      unsigned bufsize;
      char* buffer;

    public:
      explicit Fdstreambuf(int fd, unsigned bufsize = 8192, bool doClose = false);
      ~Fdstreambuf();

      void setClose(bool sw = true)  { doClose = sw; }
      bool isClose() const           { return doClose; }
      void close();

      int getFd() const  { return fd; }
  };

  class Fdiostream : public std::iostream
  {
      Fdstreambuf streambuf;

    public:
      explicit Fdiostream(int fd, unsigned bufsize = 8192, bool close = false)
        : std::iostream(0),
          streambuf(fd, bufsize, close)
          {
            init(&streambuf);
          }

      void setClose(bool sw = true)  { streambuf.setClose(sw); }
      bool isClose() const           { return streambuf.isClose(); }
      void close()                   { streambuf.close(); }

      int getFd() const  { return streambuf.getFd(); }
  };
}

#endif // CXXTOOLS_FDSTREAM_H
