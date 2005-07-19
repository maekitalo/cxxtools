/* cxxtools/udpstream.h
   Copyright (C) 2005 Tommi Maekitalo

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

#ifndef CXXTOOLS_UDPSTREAM_H
#define CXXTOOLS_UDPSTREAM_H

#include <cxxtools/udp.h>
#include <sstream>

namespace cxxtools
{
  namespace net
  {
    class UdpStreambuf : public std::streambuf
    {
        std::string message;
        UdpSender& sender;
        int flags;

      public:
        explicit UdpStreambuf(UdpSender& sender_, int flags_ = 0)
          : sender(sender_),
            flags(flags_)
          { }

      protected:
        std::streambuf::int_type overflow(std::streambuf::int_type ch);
        std::streambuf::int_type underflow();
        int sync();
    };

    class UdpOStream : public std::ostream
    {
        UdpStreambuf streambuf;

      public:
        explicit UdpOStream(UdpSender& sender, int flags = 0)
          : std::ostream(&streambuf),
            streambuf(sender, flags)
          { }
    };

  } // namespace net

} // namespace cxxtools

#endif // CXXTOOLS_UDPSTREAM_H
