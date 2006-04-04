/* 
   Copyright (C) 2006 Tommi Maekitalo

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
along with tntnet; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330,
Boston, MA  02111-1307  USA
*/

#ifndef CXXTOOLS_STREAMCOUNTER_H
#define CXXTOOLS_STREAMCOUNTER_H

#include <iostream>

namespace cxxtools
{
  class StreamcounterBuf : public std::streambuf
  {
      unsigned count;

    public:
      StreamcounterBuf()
        : count(0)
        { }

      unsigned getCount() const         { return count; }
      void resetCount(unsigned count_)  { count = count_; }

    private:
      std::streambuf::int_type overflow(std::streambuf::int_type ch);
      std::streambuf::int_type underflow();
      int sync();
  };

  class Streamcounter : public std::ostream
  {
      StreamcounterBuf streambuf;
    public:
      Streamcounter()
        : std::ostream(&streambuf)
        { }

      unsigned getCount() const           { return streambuf.getCount(); }
      void resetCount(unsigned count = 0) { streambuf.resetCount(count); }
  };
}

#endif // CXXTOOLS_STREAMCOUNTER_H

