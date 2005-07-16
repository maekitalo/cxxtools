/* cxxtools/hdstream.h
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

#ifndef CXXTOOLS_HDSTREAM_H
#define CXXTOOLS_HDSTREAM_H

#include <iostream>

namespace cxxtools
{

class Hdstreambuf : public std::streambuf
{
  public:
    Hdstreambuf(std::streambuf* dest)
      : Dest(dest),
        offset(0)
    {
      setp(Buffer, Buffer + BUFFERSIZE);
    }

  private:
    static const unsigned BUFFERSIZE = 16;
    
    std::streambuf::int_type overflow(std::streambuf::int_type ch);
    std::streambuf::int_type underflow();
    int sync();

    char Buffer[BUFFERSIZE];
    std::streambuf* Dest;
    unsigned offset;
};

/**
 hexdumper as a outputstream.

 Data written to a hdostream are passed as a hexdump to the given sink.
 */
class Hdostream : public std::ostream
{
    typedef std::ostream base_class;

  public:
    Hdostream()
      : base_class(new Hdstreambuf(std::cout.rdbuf()))
    { }
    Hdostream(std::ostream& out)
      : base_class(new Hdstreambuf(out.rdbuf()))
    { }

    ~Hdostream()
    { delete rdbuf(); }
};

}

#endif  // CXXTOOLS_HDSTREAM_H
