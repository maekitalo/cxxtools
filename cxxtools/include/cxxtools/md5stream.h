/* cxxtools/md5stream.h
   Copyright (C) 2003 Tommi Mäkitalo

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

#ifndef MD5STREAM_H
#define MD5STREAM_H

#include <cxxtools/md5.h>
#include <iostream>

class md5streambuf : public std::streambuf
{
  public:
    md5streambuf()
    { }

    void getDigest(unsigned char digest[16]);

  private:
    static const unsigned int bufsize = 64;
    char buffer[bufsize];
    cxxtools_MD5_CTX context;

    std::streambuf::int_type overflow(std::streambuf::int_type ch);
    std::streambuf::int_type underflow();
    std::streambuf::int_type sync();
};

class md5stream : public std::ostream
{
    md5streambuf streambuf;
    char hexdigest[33];

  public:
    md5stream()
      : std::ostream(&streambuf)
    { }

    void getDigest(unsigned char digest[16])
    { streambuf.getDigest(digest); }
    const char* getHexDigest();
};

#endif
