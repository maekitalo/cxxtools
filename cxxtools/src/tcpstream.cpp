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

#include "addrinfo.h"
#include <cxxtools/tcpstream.h>
#include <cxxtools/systemerror.h>
#include <cxxtools/log.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <netdb.h>
#include <errno.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "config.h"

log_define("cxxtools.net.tcp")

namespace cxxtools
{

namespace net
{
  namespace
  {
    void formatIp(const sockaddr_storage& addr, std::string& str)
    {
#ifdef HAVE_INET_NTOP
      const sockaddr_in* sa = reinterpret_cast<const sockaddr_in*>(&addr);
      char strbuf[INET6_ADDRSTRLEN + 1];
      const char* p = inet_ntop(sa->sin_family, &sa->sin_addr, strbuf, sizeof(strbuf));
      str = (p == 0 ? "-" : strbuf);
#else
      static cxxtools::Mutex monitor;
      cxxtools::MutexLock lock(monitor);

      const sockaddr_in* sa = reinterpret_cast<const sockaddr_in*>(&addr);
      const char* p = inet_ntoa(sa->sin_addr);
      if (p)
        str = p;
      else
        str.clear();
#endif
    }
  }

  ////////////////////////////////////////////////////////////////////////
  // implementation of Server
  //
/*  Server::Server(const std::string& ipaddr, unsigned short int port,
      int backlog)
  {
    listen(ipaddr, port, backlog);
  }

  void Server::listen(const std::string& ipaddr, unsigned short int port,
      int backlog)
  {
    log_debug("listen on " << ipaddr << " port " << port << " backlog " << backlog);

    Addrinfo ai(ipaddr, port);

    int reuseAddr = 1;

    // getaddrinfo() may return more than one addrinfo structure, so work
    // them all out, until we find a pretty useable one
    for (Addrinfo::const_iterator it = ai.begin(); it != ai.end(); ++it)
    {
      try
      {
        Socket::create(it->ai_family, SOCK_STREAM, 0);
      }
      catch (const SystemError&)
      {
        continue;
      }

      log_debug("setsockopt SO_REUSEADDR");
      if (::setsockopt(getFd(), SOL_SOCKET, SO_REUSEADDR,
          &reuseAddr, sizeof(reuseAddr)) < 0)
        throw SystemError("setsockopt");

      log_debug("bind");
      if (::bind(getFd(), it->ai_addr, it->ai_addrlen) == 0)
      {
        // save our information
        memmove(&servaddr, it->ai_addr, it->ai_addrlen);

        log_debug("listen");
        if (::listen(getFd(), backlog) < 0)
        {
          if (errno == EADDRINUSE)
            throw AddressInUse();
          else
            throw SystemError("listen");
        }

        return;
      }
    }

    throw SystemError("bind");
  }
*/
  ////////////////////////////////////////////////////////////////////////
  // implementation of Stream
  //
/*  Stream::Stream()
    { }

  Stream::Stream(const Server& server)
  {
    accept(server);
  }

  Stream::Stream(const std::string& ipaddr, unsigned short int port)
  {
    connect(ipaddr, port);
  }

  void Stream::accept(const Server& server)
  {
    close();

    socklen_t peeraddr_len;
    peeraddr_len = sizeof(peeraddr);
    log_debug("accept " << server.getFd());
    int fd = ::accept(server.getFd(), reinterpret_cast <struct sockaddr *> (&peeraddr), &peeraddr_len);
    if (fd < 0)
      throw SystemError("accept");
    setFd(fd);
    log_debug("accepted " << server.getFd() << " => " << getFd());
  }

  void Stream::connect(const std::string& ipaddr, unsigned short int port)
  {
    log_debug("connect to " << ipaddr << " port " << port);

    Addrinfo ai(ipaddr, port);

    log_debug("do connect");
    for (Addrinfo::const_iterator it = ai.begin(); it != ai.end(); ++it)
    {
      try
      {
        Socket::create(it->ai_family, SOCK_STREAM, 0);
      }
      catch (const SystemError&)
      {
        continue;
      }

      if (::connect(getFd(), it->ai_addr, it->ai_addrlen) == 0)
      {
        // save our information
        memmove(&peeraddr, it->ai_addr, it->ai_addrlen);
        return;
      }

      if (errno == EINPROGRESS && getTimeout() > 0)
      {
        poll(POLLOUT);

        int sockerr;
        socklen_t optlen = sizeof(sockerr);
        if (::getsockopt(getFd(), SOL_SOCKET, SO_ERROR, &sockerr, &optlen) != 0)
        {
          ::close(getFd());
          throw SystemError("getsockopt");
        }

        if (sockerr != 0)
          throw SystemError(sockerr, "connect");

        // save our information
        memmove(&peeraddr, it->ai_addr, it->ai_addrlen);
        return;
      }

    }

    throw SystemError("connect");
  }

  Stream::size_type Stream::read(char* buffer, Stream::size_type bufsize) const
  {
    ssize_t n;

    if (getTimeout() < 0)
    {
      // blocking read
      do
      {
        log_debug("blocking read");
        n = ::read(getFd(), buffer, bufsize);
        log_debug("blocking read ready, return " << n);
      } while (n < 0 && errno == EINTR);

      if (n < 0 && errno != ECONNRESET)
        throw SystemError(errno, "read");
    }
    else
    {
      // non-blocking read

      // try reading without timeout
      do
      {
        log_debug("non blocking read fd=" << getFd());
        n = ::read(getFd(), buffer, bufsize);
        log_debug("non blocking read returns " << n);
      } while (n < 0 && errno == EINTR);

      if (n < 0)
      {
        // no data available

        if (errno == EAGAIN)
        {
          if (getTimeout() == 0)
          {
            log_debug("read timeout (" << getTimeout() << "ms)");
            throw Timeout();
          }

          if (poll(POLLIN) & POLLHUP)
          {
            log_debug("eof in read");
            return 0;
          }

          do
          {
            log_debug("read");
            n = ::read(getFd(), buffer, bufsize);
            log_debug("read returns " << n);
          } while (n < 0 && errno == EINTR);

          if (n < 0)
            throw SystemError("read");
        }
        else if (errno != 0 && errno != ECONNRESET)
          throw SystemError("read");
      }

    }

    return n < 0 ? 0 : n;
  }

  Stream::size_type Stream::write(const char* buffer,
                                  Stream::size_type bufsize, bool flush) const
  {
    log_debug("Stream::write " << bufsize << " bytes");

    ssize_t n = 0;
    size_type s = bufsize;  // number of bytes left

    while (true)
    {
      do
      {
        n = ::write(getFd(), buffer, s);
        log_debug("::write returns " << n << " errno=" << errno);
      } while (n < 0 && errno == EINTR);

      if (n < 0)
      {
        if (errno == EAGAIN)
          n = 0;
        else if (errno == EPIPE || errno == ECONNRESET)
          return 0;
        else
        {
          log_error("error in write; errno=" << errno << " (" << strerror(errno) << ')');
          throw SystemError("write");
        }
      }

      buffer += n;
      s -= n;

      if (s > 0 && n > 0 && !flush)
      {
        log_debug("partial write; " << n << " bytes written from " << bufsize << "; return " << (bufsize - s));
        return bufsize - s;
      }

      if (s <= 0)
        break;

      if (getTimeout() <= 0)
      {
        log_debug("write timeout (" << getTimeout() << "ms)");
        throw Timeout();
      }

      if (poll(POLLOUT) & POLLHUP)
      {
        log_debug("eof in write");
        return bufsize - s;
      }
    }

    log_debug("full write - return " << bufsize);
    return bufsize;
  }

  std::string Stream::getPeerAddr() const
  {
    std::string ret;
    formatIp(peeraddr, ret);
    return ret;
  }


  streambuf::streambuf(Stream& stream, unsigned bufsize, int timeout)
    : m_stream(stream),
      m_bufsize(bufsize),
      m_buffer(new char_type[bufsize * 2])
  {
    setTimeout(timeout);
  }

  streambuf::int_type streambuf::overflow(streambuf::int_type c)
  {
    log_debug("streambuf::overflow");

    if (pptr())
    {
      // NOTE: use std::streamsize, not Stream::size_type
      std::streamsize N = pptr() - m_buffer; // bytes to write

      // NOTE: use std::streamsize, not Stream::size_type
      // NOTE: default is to write some bytes
      std::streamsize n = m_stream.write(m_buffer, N); //, false);
      if (n <= 0)
        return traits_type::eof();

      if (n < N)
      {
        // there are bytes left - move them to the start of our buffer
        memmove(m_buffer, m_buffer + n, N - n);
        setp(m_buffer + N - n, m_buffer + m_bufsize);
      }
      else
        setp(m_buffer, m_buffer + m_bufsize);
    }
    else
      setp(m_buffer, m_buffer + m_bufsize);

    if (c != traits_type::eof())
    {
      *pptr() = traits_type::to_char_type(c);
      pbump(1);
    }

    return 0;
  }

  bool streambuf::canRead()
  {
    if (in_avail() > 0)
      return true;

    int timeout = getTimeout();
    setTimeout(0);
    try
    {
      streambuf::int_type n = underflow();
      setTimeout(timeout);
      return n != traits_type::eof();
    }
    catch (...)
    {
      setTimeout(timeout);
      throw;
    }
  }

  streambuf::int_type streambuf::underflow()
  {
    log_debug("streambuf::underflow");

    // NOTE: use std::streamsize, not Stream::size_type
    std::streamsize n = m_stream.read(m_buffer + m_bufsize, m_bufsize);
    if (n <= 0)
      return traits_type::eof();

    setg(m_buffer + m_bufsize, m_buffer + m_bufsize, m_buffer + m_bufsize + n);
    return traits_type::to_int_type(m_buffer[m_bufsize]);
  }

  int streambuf::sync()
  {
    log_debug("streambuf::sync");

    if (pptr())
    {
      // NOTE: use std::streamsize, not Stream::size_type
      std::streamsize N = pptr() - m_buffer; // bytes to write
      if (N > 0)
      {
        // NOTE: use std::streamsize, not Stream::size_type
        std::streamsize n = m_stream.write(m_buffer, N);
        if (n <= 0)
          return -1;
        else
          setp(m_buffer, m_buffer + m_bufsize);
      }
    }
    return 0;
  }
*/

  bool TcpStream::canRead()
  {
      if( buffer().in_avail() > 0 )
          return true;

      unsigned long pendingBytes = 0;
      if( -1 != ::ioctl(_socket.getFd(), FIONREAD, &pendingBytes) )
      {
           return pendingBytes;
      }

      return 0;
   }

} // namespace net

} // namespace cxxtools
