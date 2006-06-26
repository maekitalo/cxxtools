/* cxxtools/udp.h
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

#ifndef CXXTOOLS_NET_UDP_H
#define CXXTOOLS_NET_UDP_H

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
        : connected(false)
        { }

      UdpSender(const char* ipaddr, unsigned short int port);

      void connect(const char* ipaddr, unsigned short int port);
      bool isConnected() const   { return connected; }

      size_type send(const void* message, size_type length, int flags = 0) const;
      size_type send(const std::string& message, int flags = 0) const;
      size_type recv(void* buffer, size_type length, int flags = 0) const;
      std::string recv(size_type length, int flags = 0) const;
  };

  class UdpReceiver : public Socket
  {
      struct sockaddr_storage peeraddr;
      socklen_t peeraddrLen;

    public:
      typedef size_t size_type;

      UdpReceiver();
      UdpReceiver(const char* ipaddr, unsigned short int port);

      void bind(const char* ipaddr, unsigned short int port);

      size_type recv(void* buffer, size_type length, int flags = 0);
      std::string recv(size_type length, int flags = 0);
      size_type send(const void* message, size_type length, int flags = 0) const;
      size_type send(const std::string& message, int flags = 0) const;
  };

} // namespace net

} // namespace cxxtools

#endif // CXXTOOLS_NET_UDP_H
