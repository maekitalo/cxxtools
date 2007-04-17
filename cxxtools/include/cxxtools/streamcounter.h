/*
 * cxxtools - general purpose C++-toolbox
 * Copyright (C) 2006 Tommi Maekitalo
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

#ifndef CXXTOOLS_STREAMCOUNTER_H
#define CXXTOOLS_STREAMCOUNTER_H

#include <iostream>

namespace cxxtools
{
  template <typename CharType, typename Traits>
  class BasicStreamcounterBuf : public std::basic_streambuf<CharType, Traits>
  {
      typedef Traits traits_type;
      typedef typename Traits::int_type int_type;

      unsigned count;

    public:
      BasicStreamcounterBuf()
        : count(0)
        { }

      unsigned getCount() const         { return count; }
      void resetCount(unsigned count_)  { count = count_; }

    private:
      int_type overflow(int_type ch)
      { ++count; return 0; }

      int_type underflow()
      { return traits_type::eof(); }

      int sync()
      { return 0; }
  };

  template <typename CharType, typename Traits>
  class BasicStreamcounter : public std::basic_ostream<CharType, Traits>
  {
      BasicStreamcounterBuf<CharType, Traits> streambuf;

    public:
      BasicStreamcounter()
        : std::basic_ostream<CharType, Traits>()
      {
        init(&streambuf);
      }

      unsigned getCount() const           { return streambuf.getCount(); }
      void resetCount(unsigned count = 0) { streambuf.resetCount(count); }
  };

  typedef BasicStreamcounter<char, std::char_traits<char> > Streamcounter;
}

#endif // CXXTOOLS_STREAMCOUNTER_H

