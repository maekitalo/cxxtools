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
    class udpostream : public std::ostringstream
    {
        UdpSender& sender;
        int flags;

      public:
        explicit udpostream(UdpSender& sender_, int flags_ = 0)
          : sender(sender_),
            flags(flags_)
          { }
        ~udpostream()
          { send(); }

        void send()
        {
          if (!str().empty())
          {
            sender.send(str());
            str(std::string());
          }
        }
    };

  } // namespace net

} // namespace cxxtools

#endif // CXXTOOLS_UDPSTREAM_H
