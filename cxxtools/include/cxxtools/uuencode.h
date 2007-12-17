/* cxxtools/uuencode.h
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

#ifndef CXXTOOLS_UUENCODE_H
#define CXXTOOLS_UUENCODE_H

#include <iostream>

namespace cxxtools
{
  class Uuencode_streambuf : public std::streambuf
  {
      std::streambuf* sinksource;
      char obuffer[3];
      char decodebuf[3];
      unsigned count;
      bool indecode;
      bool eofflag;

    public:
      Uuencode_streambuf(std::streambuf* sinksource_)
        : sinksource(sinksource_),
          count(0),
          indecode(false),
          eofflag(false)
          { }
      ~Uuencode_streambuf()
          { }

      void end();
      void reset()   { eofflag = false; }
    protected:
      std::streambuf::int_type overflow(std::streambuf::int_type ch);
      std::streambuf::int_type underflow();
      int sync();

    private:
      void putChar(char ch);
      int getval();
  };

  /**
   * uuencoder.
   */
  class UuencodeOstream : public std::ostream
  {
      Uuencode_streambuf streambuf;

    public:
      UuencodeOstream(std::ostream& out)
        : std::ostream(0),
          streambuf(out.rdbuf())
      {
        init(&streambuf);
      }

      UuencodeOstream(std::streambuf* sb)
        : std::ostream(0),
          streambuf(sb)
      {
        init(&streambuf);
      }
  };

  /**
   * uudecoder.
   */
  class UudecodeIstream : public std::istream
  {
      Uuencode_streambuf streambuf;

    public:
      UudecodeIstream(std::istream& in)
        : std::istream(0),
          streambuf(in.rdbuf())
        { init(&streambuf); }
      UudecodeIstream(std::streambuf* sb)
        : std::istream(0),
          streambuf(sb)
        { init(&streambuf); }
  };

}

#endif // CXXTOOLS_UUENCODE_H

