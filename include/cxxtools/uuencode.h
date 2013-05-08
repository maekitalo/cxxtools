/*
 * Copyright (C) 2007 Tommi Maekitalo
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

#ifndef CXXTOOLS_UUENCODE_H
#define CXXTOOLS_UUENCODE_H

#include <iostream>

namespace cxxtools
{
  class Uuencode_streambuf : public std::streambuf
  {
      std::streambuf* sinksource;
      unsigned length;
      char* obuffer;
      bool inStream;

    public:
      Uuencode_streambuf(std::streambuf* sinksource_, unsigned length_ = 45)
        : sinksource(sinksource_),
          length(length_),
          obuffer(new char[length]),
          inStream(false)
          { }
      ~Uuencode_streambuf()
          { end(); delete[] obuffer; }

      void begin(const std::string& filename, unsigned mode = 0644);
      void end();

    protected:
      std::streambuf::int_type overflow(std::streambuf::int_type ch);
      std::streambuf::int_type underflow();
      int sync();
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

      ~UuencodeOstream()
      {
        end();
      }

      void begin(const std::string& filename, unsigned mode = 0644)
        { streambuf.begin(filename, mode); }
      void end()
        { streambuf.end(); }
  };

}

#endif // CXXTOOLS_UUENCODE_H

