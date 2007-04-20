/* cxxtools/udpstream.h
 *
 * cxxtools - general purpose C++-toolbox
 * Copyright (C) 2005 Tommi Maekitalo
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
        UdpSender* sender;
        UdpStreambuf streambuf;

      public:
        explicit UdpOStream(UdpSender& sender, int flags = 0)
          : std::ostream(0),
            sender(0),
            streambuf(sender, flags)
        {
          init(&streambuf);
        }
        UdpOStream(const char* ipaddr, unsigned short int port, bool bcast = false,
            int flags = 0)
          : std::ostream(0),
            sender(new UdpSender(ipaddr, port, bcast)),
            streambuf(*sender, flags)
        {
          init(&streambuf);
        }
        ~UdpOStream()
          { delete sender; }
    };

  } // namespace net

} // namespace cxxtools

#endif // CXXTOOLS_UDPSTREAM_H
