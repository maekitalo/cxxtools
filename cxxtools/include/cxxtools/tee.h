/* cxxtools/tee.h
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

#ifndef CXXTOOLS_TEE_H
#define CXXTOOLS_TEE_H

#include <iostream>

namespace cxxtools
{

class Teestreambuf : public std::streambuf
{
  public:
    Teestreambuf(std::streambuf* buf1 = 0, std::streambuf* buf2 = 0)
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
    int sync();

    std::streambuf* streambuf1;
    std::streambuf* streambuf2;
};

/////////////////////////////////////////////////////////////////////////////

class Tee : public std::ostream
{
    typedef std::ostream base_class;

  public:
    Tee()
      : base_class(new Teestreambuf(std::cout.rdbuf()))
    { }
    Tee(std::ostream& s1, std::ostream& s2)
      : base_class(new Teestreambuf(s1.rdbuf(), s2.rdbuf()))
    { }
    Tee(std::ostream& s)
      : base_class(new Teestreambuf(s.rdbuf(), std::cout.rdbuf()))
    { }
    ~Tee()
    {
      delete rdbuf();
    }

    void assign(std::ostream& s1, std::ostream& s2);
    void assign(std::ostream& s)
    { assign(s, std::cout); }
    void assign_single(std::ostream& s);
};

}

#endif  // CXXTOOLS_TEE_H
