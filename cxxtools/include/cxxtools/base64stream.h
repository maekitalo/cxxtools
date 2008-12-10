/*
 * Copyright (C) 2004 Tommi Maekitalo
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

#ifndef CXXTOOLS_BASE64STREAM_H
#define CXXTOOLS_BASE64STREAM_H

#include <iostream>

namespace cxxtools
{

class Base64stream_streambuf : public std::streambuf
{
    std::streambuf* sinksource;
    char obuffer[3];
    char decodebuf[3];
    unsigned count;
    bool indecode;
    bool eofflag;

  public:
    Base64stream_streambuf(std::streambuf* sinksource_)
      : sinksource(sinksource_),
        count(0),
        indecode(false),
        eofflag(false)
      { }
    ~Base64stream_streambuf()
      { if (indecode) end(); }

    void end();
    void reset()  { eofflag = false; }

  protected:
    std::streambuf::int_type overflow(std::streambuf::int_type ch);
    std::streambuf::int_type underflow();
    int sync();

  private:
    void putChar(char ch);
    int getval();
};

/**
 Base64ostream is a base64-encoder.
 
 To base64-encode, instantiate a Base64ostream with an outputstream.
 Write the data to encode into the stream. Base64ostream writes the
 base64-encoded data to the outputstream.  When ready call end() to
 pad and flush. The stream is also padded and flushed in the
 destructor, when there are characters left.
 */

class Base64ostream : public std::ostream
{
    Base64stream_streambuf streambuf;

  public:
    /// instantiate encode with a output-stream, which received encoded
    /// Data.
    Base64ostream(std::ostream& out)
      : std::ostream(0),
        streambuf(out.rdbuf())
      {
        init(&streambuf);
      }
    /// instantiate encode with a output-std::streambuf.
    Base64ostream(std::streambuf* sb)
      : std::ostream(0),
        streambuf(sb)
      {
        init(&streambuf);
      }

    /// pad and flush stream.
    void end()  { streambuf.end(); }
};

/**
 Base64istream is a base64-decoder.
 
 To base64-decode, instantiate a base64istream with an inputstream.
 The class reads base64-encoded data from the inputstream and you get
 decoded output.
 */

class Base64istream : public std::istream
{
    Base64stream_streambuf streambuf;

  public:
    Base64istream(std::istream& in)
      : std::istream(0),
        streambuf(in.rdbuf())
      { init(&streambuf); }
    Base64istream(std::streambuf* sb)
      : std::istream(0),
        streambuf(sb)
      { init(&streambuf); }

    /// If a istream has multiple base64-streams, you can skip to
    /// the next stream after getting eof.
    void reset()  { streambuf.reset(); }
};

}

#endif // CXXTOOLS_BASE64STREAM_H

