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

#ifndef CXXTOOLS_MD5STREAM_H
#define CXXTOOLS_MD5STREAM_H

#include <cxxtools/md5.h>
#include <iostream>

namespace cxxtools
{

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
    unsigned char digest[16];

    std::streambuf::int_type overflow(std::streambuf::int_type ch);
    std::streambuf::int_type underflow();
    std::streambuf::int_type sync();
};

/**
 This is a easy an safe interface to MD5-calculation.

 To get a MD5-sum of data, instantiate a md5stream, copy your data
 into it and read the digest.

 After calling getDigest or getHexDigest, the class can be reused
 for another md5-calculation. The algorithm is automatically
 reinitialized when the first character is received.

 example:
 \code
  int main(int argc, char* argv[])
  {
    md5stream s;
    for (int i = 1; i < argc; ++i)
    {
      std::ifstream in(argv[i]);
      if (in)
      {
        s << in.rdbuf();
        std::cout << s.getHexDigest() << "  " << argv[i] << std::endl;
      }
    }
  }
 \endcode
 */
class md5stream : public std::ostream
{
    md5streambuf streambuf;
    char hexdigest[33];

  public:
    /// initializes md5-calculation
    md5stream()
      : std::ostream(&streambuf)
    { }

    /// ends md5-calculation and returns 16 bytes digest
    void getDigest(unsigned char digest[16])
    { streambuf.getDigest(digest); }
    /// ends md5-calculation and digest as 32 bytes hex
    const char* getHexDigest();
};

}

#endif  // CXXTOOLS_MD5STREAM_H
