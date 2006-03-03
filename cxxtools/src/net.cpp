/* net.cpp
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

#include "cxxtools/net.h"
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/poll.h>
#include <sstream>
#include "cxxtools/log.h"

log_define("cxxtools.net");

namespace cxxtools
{

namespace net
{
  static std::string mkerrmsg(const std::string& msg, int _errno)
  {
    if (_errno != 0)
    {
      std::ostringstream m;
      m << msg << ": errno " << _errno << " - " << strerror(_errno);
      return m.str();
    }
    else
      return msg;
  }

  Exception::Exception(int _errno, const std::string& msg)
    : std::runtime_error(mkerrmsg(msg, _errno)),
      m_Errno(_errno)
    { }

  Exception::Exception(const std::string& msg)
    : std::runtime_error(mkerrmsg(msg, errno)),
      m_Errno(errno)
  { }

  ////////////////////////////////////////////////////////////////////////
  // implementation of Socket
  //
  Socket::Socket(int domain, int type, int protocol) throw (Exception)
    : m_timeout(-1)
  {
    if ((m_sockFd = ::socket(domain, type, protocol)) < 0)
      throw Exception("cannot create socket");
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

    int fd = ::socket(domain, type, protocol);
    if (fd < 0)
      throw Exception("cannot create socket");
    setFd(fd);
  }

  void Socket::close()
  {
    if (m_sockFd >= 0)
    {
      ::close(m_sockFd);
      m_sockFd = -1;
    }
  }

  struct sockaddr Socket::getSockAddr() const throw (Exception)
  {
    struct sockaddr ret;

    socklen_t slen = sizeof(ret);
    if (::getsockname(getFd(), &ret, &slen) < 0)
      throw Exception("error in getsockname");

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
    fcntl(getFd(), F_SETFL, a);
  }

  short Socket::doPoll(short events) const
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
    else if (fds.revents & (POLLERR | POLLHUP | POLLNVAL))
    {
      log_error("poll returns with error; revents=" << fds.revents);
      throw Exception("poll");
    }

    return fds.revents;
  }

} // namespace net

} // namespace cxxtools
