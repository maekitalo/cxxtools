/* cxxtools/tee.h
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

#ifndef TEE_H
#define TEE_H

#include <iostream>

class teestreambuf : public std::streambuf
{
  public:
    teestreambuf(std::streambuf* buf1 = 0, std::streambuf* buf2 = 0)
      : streambuf1(buf1),
        streambuf2(buf2)
    { setp(0, 0); }
    
    void tie(std::streambuf* buf1, std::streambuf* buf2 = 0)
    {
      streambuf1 = buf1;
      streambuf2 = buf2;
    }

  private:
    std::streambuf::int_type overflow(std::streambuf::int_type ch);
    std::streambuf::int_type underflow();
    std::streambuf::int_type sync();

    std::streambuf* streambuf1;
    std::streambuf* streambuf2;
};

/////////////////////////////////////////////////////////////////////////////

class tee : public std::ostream
{
    typedef std::ostream base_class;

  public:
    tee()
      : base_class(new teestreambuf(std::cout.rdbuf()))
    { }
    tee(std::ostream& s1, std::ostream& s2)
      : base_class(new teestreambuf(s1.rdbuf(), s2.rdbuf()))
    { }
    tee(std::ostream& s)
      : base_class(new teestreambuf(s.rdbuf(), std::cout.rdbuf()))
    { }
    ~tee()
    {
      delete rdbuf();
    }

    void assign(std::ostream& s1, std::ostream& s2);
    void assign(std::ostream& s)
    { assign(s, std::cout); }
    void assign_single(std::ostream& s);
};

#endif  // TEE_H
