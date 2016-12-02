/*
 * Copyright (C) 2003 Tommi Maekitalo
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

#ifndef CXXTOOLS_HDSTREAM_H
#define CXXTOOLS_HDSTREAM_H

#include <iostream>
#include <string>
#include <sstream>

namespace cxxtools
{

class Hdstreambuf : public std::streambuf
{
    static const unsigned BUFFERSIZE = 16;
    
    std::streambuf::int_type overflow(std::streambuf::int_type ch);
    std::streambuf::int_type underflow();
    int sync();

    char Buffer[BUFFERSIZE];
    std::streambuf* Dest;
    unsigned offset;

  public:
    Hdstreambuf(std::streambuf* dest)
      : Dest(dest),
        offset(0)
    {
      setp(Buffer, Buffer + BUFFERSIZE);
    }

    unsigned getOffset() const         { return offset; }
    void setOffset(unsigned offset_)   { offset = offset_; }
};

/**
 hexdumper as a outputstream.

 Data written to a hdostream are passed as a hexdump to the given sink.
 */
class Hdostream : public std::ostream
{
    typedef std::ostream base_class;
    Hdstreambuf streambuf;

  public:
    Hdostream()
      : base_class(0),
        streambuf(std::cout.rdbuf())
    {
      init(&streambuf);
    }
    Hdostream(std::ostream& out)
      : base_class(0),
        streambuf(out.rdbuf())
    {
      init(&streambuf);
    }

    unsigned getOffset() const         { return streambuf.getOffset(); }
    void setOffset(unsigned offset_)   { streambuf.setOffset(offset_); }
};

template <typename T>
void hexDump(std::ostream& out, const T& t)
{
  Hdostream hd(out);
  hd << t << std::flush;
}

template <typename T>
std::string hexDump(const T& t)
{
  std::ostringstream out;
  Hdostream hd(out);
  hd << t << std::flush;
  return out.str();
}

inline std::string hexDump(const char* p, unsigned n)
{
  std::ostringstream out;
  Hdostream hd(out);
  hd.write(p, n);
  hd.flush();
  return out.str();
}

/** Helper object for dumping content to output stream without creating another output stream.
 *
 *  This makes dumping data a little more efficient than using `hexDump(const char*, n)`,
 *  which creates a ostringstream, a cxxtools::Hdostream and a temporary std::string when
 *  printed to a output stream.
 */
class HexDump
{
  friend std::ostream& operator<< (std::ostream& out, const HexDump& hd);
  const char* _p;
  unsigned _n;

public:
  HexDump(const char* p, unsigned n)
    : _p(p),
      _n(n)
      { }
};

/** Outputs data to output stream as a hex dump.
 *
 *  Example:
 *  \code
 *    const char* buffer = ...;
 *    unsigned bufsize = ...
 *    std::cout << HexDump(buffer, bufsize);
 *  \endcode
 *
 *  This outputs the content of the buffer to standard out as a hex dump.
 */
inline std::ostream& operator<< (std::ostream& out, const HexDump& hd)
{
   Hdstreambuf buf(out.rdbuf());
   if (buf.sputn(hd._p, hd._n) == hd._n)
     buf.pubsync();
   else
     out.setstate(std::ios::failbit);
   return out;
}

}

#endif  // CXXTOOLS_HDSTREAM_H
