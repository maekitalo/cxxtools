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

#ifndef CXXTOOLS_MULTIFSTREAM_H
#define CXXTOOLS_MULTIFSTREAM_H

#include <fstream>
#include <glob.h>
#include <string>
#include <queue>

namespace cxxtools
{

/**
 Read multiple files as one stream.

 Multiple files are
 */

class multifstreambuf : public std::streambuf
{
    glob_t mglob;
    unsigned current;
    std::filebuf file;
    char ch;
    typedef std::queue<std::pair<std::string, int> > patterns_type;
    patterns_type patterns;

  public:
    multifstreambuf();
    explicit multifstreambuf(const char* pattern, int flags = 0);
    ~multifstreambuf();

    bool open_next();

    int_type overflow(int_type c);
    int_type underflow();
    int sync();

    const char* current_filename() const
      { return mglob.gl_pathv[current]; }

    void add_pattern(const std::string& pattern, int flags = 0)
      { patterns.push(patterns_type::value_type(pattern, flags)); }
};

class multi_ifstream : public std::istream
{
    multifstreambuf buffer;

  public:
    multi_ifstream()
      : std::istream(0)
    {
      init(&buffer);
    }
    multi_ifstream(const char* pattern, int flags = 0)
      : std::istream(0),
        buffer(pattern, flags)
    {
      init(&buffer);
    }

    const char* current_filename() const
      { return buffer.current_filename(); }

    void add_pattern(const std::string& pattern, int flags = 0)
      { buffer.add_pattern(pattern, flags); }
};

}

#endif // CXXTOOLS_MULTIFSTREAM_H

