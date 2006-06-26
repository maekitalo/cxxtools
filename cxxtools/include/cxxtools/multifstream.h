/* cxxtools/multifstream.h
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
      : std::istream(&buffer)
    { }
    multi_ifstream(const char* pattern, int flags = 0)
      : std::istream(&buffer),
        buffer(pattern, flags)
    { }

    const char* current_filename() const
      { return buffer.current_filename(); }

    void add_pattern(const std::string& pattern, int flags = 0)
      { buffer.add_pattern(pattern, flags); }

    /// stl-compatible inserter
    class pattern_inserter
      : public std::iterator<std::output_iterator_tag, const std::string>
    {
        multi_ifstream& m;
      public:
        pattern_inserter(multi_ifstream& m_)
          : m(m_) { }
        pattern_inserter& operator= (const std::string& p)
          { m.add_pattern(p); return *this; }
        pattern_inserter& operator* ()
          { return *this; }
        pattern_inserter& operator++ ()
          { return *this; }
        pattern_inserter& operator++ (int)
          { return *this; }
    };
    pattern_inserter back_inserter()
      { return pattern_inserter(*this); }
};

}

#endif // CXXTOOLS_MULTIFSTREAM_H

