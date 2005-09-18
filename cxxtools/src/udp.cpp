/* udp.cpp
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
along with Cxxtools; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330,
Boston, MA  02111-1307  USA
*/

#include "cxxtools/udp.h"
#include "cxxtools/dynbuffer.h"
#include <netdb.h>

namespace cxxtools
{

namespace net
{
  //////////////////////////////////////////////////////////////////////
  // UdpSender
  //
  UdpSender::UdpSender(const char* ipaddr, unsigned short int port)
    : Socket(AF_INET, SOCK_DGRAM, 0),
      connected(false)
  {
    connect(ipaddr, port);
  }

  void UdpSender::connect(const char* ipaddr, unsigned short int port)
  {
    union {
      struct sockaddr sockaddr;
      struct sockaddr_in sockaddr_in;
    } peeraddr;

    struct hostent* host = ::gethostbyname(ipaddr);
    if (host == 0)
      throw Exception(std::string("invalid ipaddress ") + ipaddr);

    memset(&peeraddr, 0, sizeof(peeraddr));
    peeraddr.sockaddr_in.sin_family = AF_INET;
    peeraddr.sockaddr_in.sin_port = htons(port);

    memmove(&(peeraddr.sockaddr_in.sin_addr.s_addr), host->h_addr, host->h_length);
    socklen_t peeraddr_len;
    peeraddr_len = sizeof(peeraddr);
    int ret = ::connect(getFd(), &peeraddr.sockaddr, peeraddr_len);
    if (ret < 0)
      throw Exception("error in connect");
    connected = true;
  }

  UdpSender::size_type UdpSender::send(const void* message, size_type length, int flags) const
  {
    ssize_t ret = ::send(getFd(), message, length, flags);
    if (ret < 0)
      throw Exception("error in send");
    return static_cast<size_type>(ret);
  }

  UdpSender::size_type UdpSender::send(const std::string& message, int flags) const
  {
    return send(message.data(), message.size(), flags);
  }

  UdpSender::size_type UdpSender::recv(void* buffer, size_type length, int flags) const
  {
    ssize_t ret = ::recv(getFd(), buffer, length, flags);
    if (ret < 0)
      throw Exception("error in recv");
    return static_cast<size_type>(ret);
  }

  std::string UdpSender::recv(size_type length, int flags) const
  {
    cxxtools::Dynbuffer<char> buffer(length);
    size_type len = recv(buffer.data(), length, flags);
    return std::string(buffer.data(), len);
  }

  //////////////////////////////////////////////////////////////////////
  // UdpReceiver
  //
  UdpReceiver::UdpReceiver()
    : Socket(AF_INET, SOCK_DGRAM, 0)
  {
    memset(&peeraddr, 0, sizeof(peeraddr));
  }

  UdpReceiver::UdpReceiver(const char* ipaddr, unsigned short int port)
    : Socket(AF_INET, SOCK_DGRAM, 0)
  {
    memset(&peeraddr, 0, sizeof(peeraddr));
    bind(ipaddr, port);
  }

  void UdpReceiver::bind(const char* ipaddr, unsigned short int port)
  {
    struct hostent* host = ::gethostbyname(ipaddr);
    if (host == 0)
      throw Exception(std::string("invalid ipaddress ") + ipaddr);

    memset(&peeraddr, 0, sizeof(peeraddr));
    peeraddr.sockaddr_in.sin_family = AF_INET;
    peeraddr.sockaddr_in.sin_port = htons(port);

    memmove(&(peeraddr.sockaddr_in.sin_addr.s_addr), host->h_addr, host->h_length);
    peeraddrLen = sizeof(peeraddr);
    int ret = ::bind(getFd(), &peeraddr.sockaddr, peeraddrLen);
    if (ret < 0)
      throw Exception("error in bind");
  }

  UdpReceiver::size_type UdpReceiver::recv(void* buffer, size_type length, int flags)
  {
    ssize_t ret = ::recvfrom(getFd(), buffer, length, flags, &peeraddr.sockaddr, &peeraddrLen);
    if (ret < 0)
      throw Exception("error in recvfrom");
    return static_cast<size_type>(ret);
  }

  std::string UdpReceiver::recv(size_type length, int flags)
  {
    cxxtools::Dynbuffer<char> buffer(length);
    size_type len = recv(buffer.data(), length, flags);
    return std::string(buffer.data(), len);
  }

  UdpReceiver::size_type UdpReceiver::send(const void* message, size_type length, int flags) const
  {
    ssize_t ret = ::sendto(getFd(), message, length, flags, &peeraddr.sockaddr, peeraddrLen);
    if (ret < 0)
      throw Exception("error in sendto");
    return static_cast<size_type>(ret);
  }

  UdpReceiver::size_type UdpReceiver::send(const std::string& message, int flags) const
  {
    return send(message.data(), message.size(), flags);
  }

} // namespace net

} // namespace cxxtools
