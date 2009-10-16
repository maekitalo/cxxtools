/*
 * Copyright (C) 2003 Tommi Maekitalo
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

#include <cxxtools/net/addrinfo.h>
#include "addrinfoimpl.h"
#include <cxxtools/net/udp.h>
#include <cxxtools/dynbuffer.h>
#include <cxxtools/log.h>
#include <cxxtools/systemerror.h>
#include <netdb.h>
#include <sys/poll.h>
#include <errno.h>
#include <string.h>

log_define("cxxtools.net.udp")

namespace cxxtools
{

namespace net
{
  //////////////////////////////////////////////////////////////////////
  // UdpSender
  //
  UdpSender::UdpSender(const char* ipaddr, unsigned short int port, bool bcast)
    : connected(false)
  {
    connect(ipaddr, port, bcast);
  }

  void UdpSender::connect(const char* ipaddr, unsigned short int port, bool bcast)
  {
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_socktype = SOCK_DGRAM;

    AddrInfo ai(new AddrInfoImpl(ipaddr, port, hints));

    for (AddrInfoImpl::const_iterator it = ai.impl()->begin(); it != ai.impl()->end(); ++it)
    {
      try
      {
        Socket::create(it->ai_family, SOCK_DGRAM, 0);
      }
      catch (const SystemError&)
      {
        continue;
      }

      if (bcast)
      {
        const int on = 1;
        if (::setsockopt(getFd(), SOL_SOCKET, SO_BROADCAST, &on, sizeof(on)) < 0)
          throw SystemError("setsockopt");
      }

      if (::connect(getFd(), it->ai_addr, it->ai_addrlen) == 0)
      {
        connected = true;
        return;
      }
    }

    throw SystemError("connect");
  }

  UdpSender::size_type UdpSender::send(const void* message, size_type length, int flags) const
  {
    ssize_t ret = ::send(getFd(), message, length, flags);
    if (ret < 0)
      throw SystemError("send");
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

      poll(POLLIN);

      ret = ::recv(getFd(), buffer, length, flags);
    }

    if (ret < 0)
      throw SystemError("recv");

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
    AddrInfo ai(ipaddr, port);

    int reuseAddr = 1;
    for (AddrInfoImpl::const_iterator it = ai.impl()->begin(); it != ai.impl()->end(); ++it)
    {
      try
      {
        Socket::create(it->ai_family, SOCK_DGRAM, 0);
      }
      catch (const SystemError&)
      {
        continue;
      }

      log_debug("setsockopt");
      if (::setsockopt(getFd(), SOL_SOCKET, SO_REUSEADDR,
          &reuseAddr, sizeof(reuseAddr)) < 0)
        throw SystemError(errno, "setsockopt");

      log_debug("bind ip " << ipaddr << " port " << port);
      if (::bind(getFd(), it->ai_addr, it->ai_addrlen) == 0)
      {
        memmove(&peeraddr, it->ai_addr, it->ai_addrlen);
        peeraddrLen = it->ai_addrlen;
        return;
      }
    }

    throw SystemError(errno, "bind");
  }

  UdpReceiver::size_type UdpReceiver::recv(void* buffer, size_type length, int flags)
  {
    log_debug("recvfrom");

    ssize_t ret = ::recvfrom(getFd(), buffer, length, flags, reinterpret_cast <struct sockaddr *> (&peeraddr), &peeraddrLen);

    if (ret < 0 && errno == EAGAIN)
    {
      if (getTimeout() == 0)
        throw Timeout();

      poll(POLLIN);

      ret = ::recvfrom(getFd(), buffer, length, flags, reinterpret_cast <struct sockaddr *> (&peeraddr), &peeraddrLen);
    }

    if (ret < 0)
      throw SystemError("recvfrom");

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
      throw SystemError("sendto");
    return static_cast<size_type>(ret);
  }

  UdpReceiver::size_type UdpReceiver::send(const std::string& message, int flags) const
  {
    return send(message.data(), message.size(), flags);
  }

} // namespace net

} // namespace cxxtools
