/* cxxtools/quotedprimtablestream.h
 *
 * cxxtools - general purpose C++-toolbox
 * Copyright (C) 2007 Tommi Maekitalo
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

#ifndef CXXTOOLS_QUOTEDPRINTABLE_H
#define CXXTOOLS_QUOTEDPRINTABLE_H

#include <iostream>

namespace cxxtools
{

class QuotedPrintable_streambuf : public std::streambuf
{
    std::streambuf* sinksource;
    unsigned col;

  public:
    QuotedPrintable_streambuf(std::streambuf* sinksource_)
      : sinksource(sinksource_),
        col(0)
      { }

  protected:
    std::streambuf::int_type overflow(std::streambuf::int_type ch);
    std::streambuf::int_type underflow();
    int sync();
};

class QuotedPrintable_ostream : public std::ostream
{
    QuotedPrintable_streambuf streambuf;

  public:
    /// instantiate encode with a output-stream, which received encoded
    /// Data.
    QuotedPrintable_ostream(std::ostream& out)
      : std::ostream(0),
        streambuf(out.rdbuf())
      {
        init(&streambuf);
      }
    /// instantiate encode with a output-std::streambuf.
    QuotedPrintable_ostream(std::streambuf* sb)
      : std::ostream(0),
        streambuf(sb)
      {
        init(&streambuf);
      }
};

}

#endif // CXXTOOLS_QUOTEDPRINTABLE_H

