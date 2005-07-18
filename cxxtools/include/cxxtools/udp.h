/* cxxtools/udp.h
   Copyright (C) 2003-2005 Tommi Maekitalo

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

#ifndef CXXTOOLS_UDP_H
#define CXXTOOLS_UDP_H

#include <cxxtools/net.h>

namespace cxxtools
{

namespace net
{
  class UdpSender : public Socket
  {
      bool connected;

    public:
      typedef size_t size_type;

      UdpSender()
        : Socket(AF_INET, SOCK_DGRAM, 0),
          connected(false)
        { }

      UdpSender(const char* ipaddr, unsigned short int port);

      void connect(const char* ipaddr, unsigned short int port);
      bool isConnected() const   { return connected; }

      size_type send(const void* message, size_type length, int flags = 0) const;
      size_type send(const std::string& message, int flags = 0) const;
  };

  class UdpReceiver : public Socket
  {
    public:
      typedef size_t size_type;

      UdpReceiver()
        : Socket(AF_INET, SOCK_DGRAM, 0)
        { }

      UdpReceiver(const char* ipaddr, unsigned short int port);

      void bind(const char* ipaddr, unsigned short int port);

      size_type recv(void* buffer, size_type length, int flags = 0) const;
      std::string recv(size_type length, int flags = 0) const;
  };

} // namespace net

} // namespace cxxtools

#endif // CXXTOOLS_UDP_H
