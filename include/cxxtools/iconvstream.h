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

#ifndef CXXTOOLS_ICONVSTREAM_H
#define CXXTOOLS_ICONVSTREAM_H

#include <iostream>
#include <stdexcept>
#include "iconvwrap.h"

namespace cxxtools
{

class iconv_error : public std::runtime_error
{
  private:
    mutable std::string msg;
    size_t pos;

  public:
    explicit iconv_error(size_t pos);
    virtual size_t position() const throw();
    virtual const char *what() const throw();
    virtual ~iconv_error() throw();
};

/**
  std::streambuf-Interface for iconv(3) and related.
  iconv converts charactersets.
 */
class iconvstreambuf : public std::streambuf
{
  public:
    /** Behaviour on iconv error. */
    typedef enum {
      /** Old behaviour, just send EOF. */
      mode_eof,
      /** Throw iconv_error. */
      mode_throw,
      /** Skip invalid characters. */
      mode_skip,
      mode_default = mode_eof
    } mode_t;

  private:
    std::ostream* sink;
    iconvwrap conv;
    char buffer[256];
    mode_t mode;
    size_t pos;

  public:
    iconvstreambuf()
      : sink(0), mode(mode_default), pos(0)
      {
        setg(buffer, buffer, buffer + (sizeof(buffer) - 1));
        setp(buffer, buffer + (sizeof(buffer) - 1));
      }
    ~iconvstreambuf()
      { close(); }

    iconvstreambuf* open(std::ostream& sink_,
      const char* tocode, const char* fromcode);
    iconvstreambuf* open(std::ostream& sink_,
      const char* tocode, const char* fromcode, mode_t mode_);
    iconvstreambuf* close() throw();

    /// overloaded from std::streambuf
    int_type overflow(int_type c);
    /// overloaded from std::streambuf
    int_type underflow();
    /// overloaded from std::streambuf
    int sync();
    /** overloaded from std::streambuf, for input returned value represents
     * number of bytes succesfully converted (does not count skipped bytes).
     */
    virtual std::streampos seekoff(std::streamoff off,
      std::ios_base::seekdir way, std::ios_base::openmode which);
    /** overloaded from std::streambuf, for input returned value represents
     * number of bytes succesfully converted (does not count skipped bytes).
     */
    virtual std::streampos seekpos(std::streampos sp,
      std::ios_base::openmode which);
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

    iconvstream(std::ostream& sink, const char* tocode, const char* fromcode,
      iconvstreambuf::mode_t mode)
      : std::ostream(0)
    {
      init(&streambuf);
      open(sink, tocode, fromcode, mode);
    }
    iconvstream()
      : std::ostream(0)
    {
      init(&streambuf);
    }

    void open(std::ostream& sink_,
      const char* tocode, const char* fromcode);
    void open(std::ostream& sink_,
      const char* tocode, const char* fromcode,
      iconvstreambuf::mode_t mode_);

    void close() throw()
      { streambuf.close(); }
};

}

#endif // CXXTOOLS_ICONVSTREAM_H

