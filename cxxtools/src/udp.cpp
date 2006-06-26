/* udp.cpp
 *
 * cxxtools - general purpose C++-toolbox
 * Copyright (C) 2003 Tommi Maekitalo
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

#include "cxxtools/udp.h"
#include "cxxtools/dynbuffer.h"
#include "cxxtools/log.h"
#include <netdb.h>
#include <sys/poll.h>
#include <errno.h>

log_define("cxxtools.net.udp")

namespace cxxtools
{

namespace net
{
  //////////////////////////////////////////////////////////////////////
  // UdpSender
  //
  UdpSender::UdpSender(const char* ipaddr, unsigned short int port)
    : connected(false)
  {
    connect(ipaddr, port);
  }

  void UdpSender::connect(const char* ipaddr, unsigned short int port)
  {
    Addrinfo ai(ipaddr, port);

    for (Addrinfo::const_iterator it = ai.begin(); it != ai.end(); ++it)
    {
      Socket::create(it->ai_family, SOCK_DGRAM, 0);

      if (::connect(getFd(), it->ai_addr, it->ai_addrlen) == 0)
      {
        connected = true;
        return;
      }
    }

    throw Exception(errno, "connect");
  }

  UdpSender::size_type UdpSender::send(const void* message, size_type length, int flags) const
  {
    ssize_t ret = ::send(getFd(), message, length, flags);
    if (ret < 0)
      throw Exception("send");
    return static_cast<size_type>(ret);
  }

  UdpSender::size_type UdpSender::send(const std::string& message,
    int flags) const
  {
    return send(message.data(), message.size(), flags);
  }

  UdpSender::size_type UdpSender::recv(void* buffer, size_type length,
    int flags) const
  {
    ssize_t ret = ::recv(getFd(), buffer, length, flags);

    if (ret < 0 && errno == EAGAIN)
    {
      if (getTimeout() == 0)
        throw Timeout();

      doPoll(POLLIN);

      ret = ::recv(getFd(), buffer, length, flags);
    }

    if (ret < 0)
      throw Exception("recv");

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
  {
    memset(&peeraddr, 0, sizeof(peeraddr));
  }

  UdpReceiver::UdpReceiver(const char* ipaddr, unsigned short int port)
  {
    memset(&peeraddr, 0, sizeof(peeraddr));
    bind(ipaddr, port);
  }

  void UdpReceiver::bind(const char* ipaddr, unsigned short int port)
  {
    Addrinfo ai(ipaddr, port);

    int reuseAddr = 1;
    for (Addrinfo::const_iterator it = ai.begin(); it != ai.end(); ++it)
    {
      Socket::create(it->ai_family, SOCK_DGRAM, 0);

      log_debug("setsockopt");
      if (::setsockopt(getFd(), SOL_SOCKET, SO_REUSEADDR,
          &reuseAddr, sizeof(reuseAddr)) < 0)
        throw Exception(errno, "setsockopt");

      log_debug("bind");
      if (::bind(getFd(), it->ai_addr, it->ai_addrlen) == 0)
      {
        memmove(&peeraddr, it->ai_addr, it->ai_addrlen);
        return;
      }
    }

    throw Exception(errno, "bind");
  }

  UdpReceiver::size_type UdpReceiver::recv(void* buffer, size_type length, int flags)
  {
    log_debug("recvfrom");

    ssize_t ret = ::recvfrom(getFd(), buffer, length, flags, reinterpret_cast <struct sockaddr *> (&peeraddr), &peeraddrLen);

    if (ret < 0 && errno == EAGAIN)
    {
      if (getTimeout() == 0)
        throw Timeout();

      doPoll(POLLIN);

      ret = ::recvfrom(getFd(), buffer, length, flags, reinterpret_cast <struct sockaddr *> (&peeraddr), &peeraddrLen);
    }

    if (ret < 0)
      throw Exception("recvfrom");

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
    ssize_t ret = ::sendto(getFd(), message, length, flags, reinterpret_cast <const struct sockaddr *> (&peeraddr), peeraddrLen);
    if (ret < 0)
      throw Exception("sendto");
    return static_cast<size_type>(ret);
  }

  UdpReceiver::size_type UdpReceiver::send(const std::string& message, int flags) const
  {
    return send(message.data(), message.size(), flags);
  }

} // namespace net

} // namespace cxxtools
