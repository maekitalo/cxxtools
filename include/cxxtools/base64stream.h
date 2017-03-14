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
#include <cxxtools/textstream.h>
#include <cxxtools/base64codec.h>

namespace cxxtools
{

/**
 Base64ostream is a base64-encoder.
 
 To base64-encode, instantiate a Base64ostream with an outputstream.
 Write the data to encode into the stream. Base64ostream writes the
 base64-encoded data to the outputstream.  When ready call terminate()
 to pad and flush. The stream is also padded and flushed in the
 destructor, when there are characters left.
 */

class Base64ostream : public BasicTextOStream<char, char>
{
    Base64Codec* _codec;

  public:
    explicit Base64ostream(std::ostream& out)
      : BasicTextOStream<char, char>(out, _codec = new Base64Codec())
      { }

    void end()  { terminate(); }

    unsigned maxcol() const             { return _codec->maxcol(); }
    const std::string& lineend() const  { return _codec->lineend(); }
    bool padding() const                { return _codec->padding(); }

    void maxcol(unsigned m)             { _codec->maxcol(m); }
    void lineend(const std::string& l)  { _codec->lineend(l); }
    void padding(bool p)                { _codec->padding(p); }
};

/**
 Base64istream is a base64-decoder.
 
 To base64-decode, instantiate a base64istream with an inputstream.
 The class reads base64-encoded data from the inputstream and you get
 decoded output.
 */

class Base64istream : public BasicTextIStream<char, char>
{
  public:
    explicit Base64istream(std::istream& in)
      : BasicTextIStream<char, char>(in, new Base64Codec())
      { }

    void reset()  { terminate(); }
};

}

#endif // CXXTOOLS_BASE64STREAM_H

