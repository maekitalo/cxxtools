/* cxxtools/iconvstream.h
   Copyright (C) 2003 Tommi Maekitalo

This file is part of cxxtools.

Cxxtools is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

Cxxtools is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with cxxtools; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330,
Boston, MA  02111-1307  USA
*/

#ifndef CXXTOOLS_ICONVSTREAM_H
#define CXXTOOLS_ICONVSTREAM_H

#include <iostream>
#include <iconv.h>

namespace cxxtools
{

/**
  std::streambuf-Interface for iconv(3) and related.
  iconv converts charactersets.
 */
class iconvstreambuf : public std::streambuf
{
    std::ostream* sink;
    iconv_t cd;
    char buffer[16];

  public:
    iconvstreambuf()
      : sink(0),
        cd((iconv_t)-1)
      { }
    ~iconvstreambuf()
      { close(); }

    iconvstreambuf* open(std::ostream& sink_,
      const char* tocode, const char* fromcode);
    iconvstreambuf* close() throw();

    /// overloaded from std::streambuf
    int_type overflow(int_type c);
    /// overloaded from std::streambuf
    int_type underflow();
    /// overloaded from std::streambuf
    int sync();

};

/**
  std::ostream-Interface for iconv(3) and related.
  iconv converts charactersets.

  To perform a character set conversion from one characterset to
  another, instantiate a iconvstream with a std::ostream as a sink.
  Write the data to the iconvstream and the converted stream is
  written to the sink.

  example (unix2win-filter):
  \code
    int main(int argc, char* argv[])
    {
      cxxtools::iconvstream out(std::cout, "LATIN1", "WINDOWS-1251");
      // copy input to output:
      out << std::cin.rdbuf();
    }
  \endcode
 */
class iconvstream : public std::ostream
{
    iconvstreambuf streambuf;

  public:
    iconvstream(std::ostream& sink, const char* tocode, const char* fromcode)
      : std::ostream(&streambuf)
    {
      streambuf.open(sink, tocode, fromcode);
    }

    void open(std::ostream& sink_,
      const char* tocode, const char* fromcode);
    void close() throw()
      { streambuf.close(); }
};

}

#endif // CXXTOOLS_ICONVSTREAM_H

