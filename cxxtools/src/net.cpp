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

#ifdef DEBUG

#include "cxxtools/log.h"
log_define("cxxtools.net");

#else

#define log_warn(expr)
#define log_debug(expr)

#endif

namespace cxxtools
{

namespace net
{
  Exception::Exception(int Errno, const std::string& msg)
    : std::runtime_error(msg + ": " + strerror(Errno)),
      m_Errno(Errno)
    { }

  Exception::Exception(const std::string& msg)
    : std::runtime_error(msg + ": " + strerror(errno)),
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

    if ((m_sockFd = ::socket(domain, type, protocol)) < 0)
      throw Exception("cannot create socket");
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
    m_timeout = t;

    if (getFd() >= 0)
    {
      long a = m_timeout >= 0 ? O_NONBLOCK : 0;
      log_debug("fcntl(" << getFd() << ", F_SETFL, " << a << ')');
      fcntl(getFd(), F_SETFL, a);
    }
  }

} // namespace net

} // namespace cxxtools
