/* cxxtools/iconvstream.h
 *
 * cxxtools - general purpose C++-toolbox
 * Copyright (C) 2003 Tommi Maekitalo
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
    char buffer[256];

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
      : std::ostream(0)
    {
      init(&streambuf);
      open(sink, tocode, fromcode);
    }
    iconvstream()
      : std::ostream(0)
    {
      init(&streambuf);
    }

    void open(std::ostream& sink_,
      const char* tocode, const char* fromcode);
    void close() throw()
      { streambuf.close(); }
};

}

#endif // CXXTOOLS_ICONVSTREAM_H

