/*
 * Copyright (C) 2006 Tommi Maekitalo
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
      char* ibuffer;
      char* obuffer;

    public:
      explicit Fdstreambuf(int fd, unsigned bufsize = 8192, bool doClose = false);
      ~Fdstreambuf();

      void setClose(bool sw = true)  { doClose = true; }
      bool isClose() const           { return doClose; }
      void close();

      int getFd() const  { return fd; }
  };

  class Fdiostream : public std::iostream
  {
      Fdstreambuf streambuf;

    public:
      explicit Fdiostream(int fd, unsigned bufsize = 8192, bool close = false)
        : std::iostream(&streambuf),
          streambuf(fd, bufsize, close)
          { }

      void setClose(bool sw = true)  { streambuf.setClose(sw); }
      bool isClose() const           { return streambuf.isClose(); }
      void close()                   { streambuf.close(); }

      int getFd() const  { return streambuf.getFd(); }
  };
}

#endif // CXXTOOLS_FDSTREAM_H
