/* cxxtools/multifstream.h
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

#ifndef CXXTOOLS_MULTIFSTREAM_H
#define CXXTOOLS_MULTIFSTREAM_H

#include <fstream>
#include <glob.h>

class multifstreambuf : public std::streambuf
{
    glob_t mglob;
    unsigned current;
    std::filebuf file;
    char ch;

  public:
    explicit multifstreambuf(const char* pattern, int flags = 0);
    ~multifstreambuf();

    bool open_next();

    int_type overflow(int_type c);
    int_type underflow();
    int sync();

    const char* current_filename() const
      { return mglob.gl_pathv[current]; }
};

class multi_ifstream : public std::istream
{
    multifstreambuf buffer;

  public:
    multi_ifstream(const char* pattern, int flags = 0)
      : std::istream(&buffer),
        buffer(pattern, flags)
    { }

    const char* current_filename() const
      { return buffer.current_filename(); }
};

#endif // CXXTOOLS_MULTIFSTREAM_H

