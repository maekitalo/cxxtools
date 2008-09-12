/* net.cpp
 *
 * cxxtools - general purpose C++-toolbox
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
 *
 */

#include "cxxtools/net.h"
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/poll.h>
#include <sstream>
#include <stdio.h>
#include <string.h>
#include "cxxtools/log.h"

log_define("cxxtools.net")

namespace cxxtools
{

namespace net
{
  ////////////////////////////////////////////////////////////////////////
  // implementation of Socket
  //
  Socket::Socket(int domain, int type, int protocol) throw (Exception)
    : m_timeout(-1)
  {
    if ((m_sockFd = ::socket(domain, type, protocol)) < 0)
      throw Exception("socket");
  }

  Socket::~Socket()
  {
    if (m_sockFd >= 0)
    {
      if (::close(m_sockFd) < 0)
        fprintf(stderr, "error in close(%d)\n", (int)m_sockFd);
    }
  }

  void Socket::create(int domain, int type, int protocol) throw (Exception)
  {
    close();

    log_debug("create socket");
    int fd = ::socket(domain, type, protocol);
    if (fd < 0)
      throw Exception("socket");
    setFd(fd);
  }

  void Socket::close()
  {
    if (m_sockFd >= 0)
    {
      log_debug("close socket");
      ::close(m_sockFd);
      m_sockFd = -1;
    }
  }

  struct sockaddr_storage Socket::getSockAddr() const throw (Exception)
  {
    struct sockaddr_storage ret;

    socklen_t slen = sizeof(ret);
    if (::getsockname(getFd(), reinterpret_cast <struct sockaddr *> (&ret), &slen) < 0)
      throw Exception("getsockname");

    return ret;
  }

  void Socket::setTimeout(int t)
  {
    if (m_timeout != t)
    {
      log_debug("set timeout " << t << ", fd=" << getFd() << ", previous=" << m_timeout);

      if (getFd() >= 0 && (t >= 0 && m_timeout < 0 || t < 0 && m_timeout >= 0))
      {
        long a = t >= 0 ? O_NONBLOCK : 0;
        log_debug("fcntl(" << getFd() << ", F_SETFL, " << a << ')');
        fcntl(getFd(), F_SETFL, a);
      }

      m_timeout = t;
    }
  }

  void Socket::setFd(int sockFd)
  {
    close();

    m_sockFd = sockFd;

    long a = m_timeout >= 0 ? O_NONBLOCK : 0;
    log_debug("fcntl(" << getFd() << ", F_SETFL, " << a << ')');
    int ret = ::fcntl(getFd(), F_SETFL, a);
    if (ret < 0)
      throw Exception("fcntl");
  }

  short Socket::poll(short events) const
  {
    struct pollfd fds;
    fds.fd = getFd();
    fds.events = events;

    log_debug("poll timeout " << getTimeout());

    int p;

    p = ::poll(&fds, 1, getTimeout());

    log_debug("poll returns " << p << " revents " << fds.revents);

    if (p < 0)
    {
      log_error("error in poll; errno=" << errno);
      throw Exception("poll");
    }
    else if (p == 0)
    {
      log_debug("poll timeout (" << getTimeout() << ')');
      throw Timeout();
    }

    return fds.revents;
  }

  Addrinfo::Addrinfo(const std::string& ipaddr, unsigned short port)
    : ai(0)
  {
    struct addrinfo hints;

    // give some useful default values to use for getaddrinfo()
    memset(&hints, 0, sizeof(hints));
    hints.ai_socktype = SOCK_STREAM;

    init(ipaddr, port, hints);
  }

  Addrinfo::~Addrinfo()
  {
    if (ai)
      freeaddrinfo(ai);
  }

  void Addrinfo::init(const std::string& ipaddr, unsigned short port,
    const addrinfo& hints)
  {
    std::ostringstream p;
    p << port;

    if (0 != ::getaddrinfo(ipaddr.c_str(), p.str().c_str(), &hints, &ai))
      throw Exception(0, ("invalid ipaddress " + ipaddr).c_str());

    if (ai == 0)
      throw Exception("getaddrinfo");
  }

} // namespace net

} // namespace cxxtools
