/* tcpstream.cpp
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

#include "cxxtools/tcpstream.h"
#include <sys/poll.h>
#include <unistd.h>
#include <netdb.h>
#include <errno.h>
#include "cxxtools/log.h"

log_define("cxxtools.net");

namespace cxxtools
{

namespace net
{
  ////////////////////////////////////////////////////////////////////////
  // implementation of Server
  //
  Server::Server()
    : Socket(AF_INET, SOCK_STREAM, 0)
  { }

  Server::Server(const std::string& ipaddr, unsigned short int port,
      int backlog) throw (Exception)
    : Socket(AF_INET, SOCK_STREAM, 0)
  {
    listen(ipaddr, port, backlog);
  }

  Server::Server(const char* ipaddr, unsigned short int port,
      int backlog) throw (Exception)
    : Socket(AF_INET, SOCK_STREAM, 0)
  {
    listen(ipaddr, port, backlog);
  }

  void Server::listen(const char* ipaddr, unsigned short int port,
      int backlog) throw (Exception)
  {
    struct hostent* host = ::gethostbyname(ipaddr);
    if (host == 0)
      throw Exception(std::string("invalid ipaddress ") + ipaddr);

    memset(&servaddr.sockaddr_in, 0, sizeof(servaddr.sockaddr_in));

    servaddr.sockaddr_in.sin_family = AF_INET;
    servaddr.sockaddr_in.sin_port = htons(port);

    memmove(&(servaddr.sockaddr_in.sin_addr.s_addr), host->h_addr, host->h_length);
    int reuseAddr = 1;
    if (::setsockopt(getFd(), SOL_SOCKET, SO_REUSEADDR,
        &reuseAddr, sizeof(reuseAddr)) < 0)
      throw Exception(errno, "setsockopt");

    if (::bind(getFd(),
               (struct sockaddr *)&servaddr.sockaddr_in,
               sizeof(servaddr.sockaddr_in)) < 0)
      throw Exception(errno, "bind");

    if (::listen(getFd(), backlog) < 0)
      throw Exception(errno, "listen");
  }

  ////////////////////////////////////////////////////////////////////////
  // implementation of Stream
  //
  Stream::Stream()
    { }

  Stream::Stream(const Server& server)
  {
    accept(server);
  }

  Stream::Stream(const std::string& ipaddr, unsigned short int port)
    : Socket(AF_INET, SOCK_STREAM, 0)
  {
    connect(ipaddr, port);
  }

  Stream::Stream(const char* ipaddr, unsigned short int port)
    : Socket(AF_INET, SOCK_STREAM, 0)
  {
    connect(ipaddr, port);
  }

  void Stream::accept(const Server& server)
  {
    close();

    socklen_t peeraddr_len;
    peeraddr_len = sizeof(peeraddr);
    setFd(::accept(server.getFd(), &peeraddr.sockaddr, &peeraddr_len));
    if (bad())
      throw Exception(errno, "accept");

    setTimeout(getTimeout());
  }

  void Stream::connect(const char* ipaddr, unsigned short int port)
  {
    if (getFd() < 0)
      create(AF_INET, SOCK_STREAM, 0);

    struct hostent* host = ::gethostbyname(ipaddr);
    if (host == 0)
      throw Exception(std::string("invalid ipaddress ") + ipaddr);

    memset(&peeraddr, 0, sizeof(peeraddr));
    peeraddr.sockaddr_in.sin_family = AF_INET;
    peeraddr.sockaddr_in.sin_port = htons(port);

    memmove(&(peeraddr.sockaddr_in.sin_addr.s_addr), host->h_addr, host->h_length);

    if (::connect(getFd(), &peeraddr.sockaddr,
        sizeof(peeraddr)) < 0)
      throw Exception(errno, "connect");

    setTimeout(getTimeout());
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
      } while (n <= 0 && errno == EINTR);

      if (n < 0)
        throw Exception(errno, "read");
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
      } while (n <= 0 && errno == EINTR);

      if (n <= 0)
      {
        // no data available

        if (errno == EAGAIN)
        {
          if (getTimeout() == 0)
          {
            log_debug("read timeout (" << getTimeout() << "ms)");
            throw Timeout();
          }

          doPoll(POLLIN);

          do
          {
            log_debug("read");
            n = ::read(getFd(), buffer, bufsize);
            log_debug("read returns " << n);
          } while (n <= 0 && errno == EINTR);

          if (n < 0)
            throw Exception(errno, "read");
        }
        else if (errno != ECONNRESET && errno != ENOENT)
          throw Exception(errno, "read");
      }

    }

    return n;
  }

  Stream::size_type Stream::write(const char* buffer,
                                  Stream::size_type bufsize) const
  {
    log_debug("Stream::write " << bufsize << " bytes");

    ssize_t n = 0;
    size_type s = bufsize;

    while (true)
    {
      do
      {
        n = ::write(getFd(), buffer, s);
        log_debug("::write returns " << n << " errno=" << errno);
      } while (n <= 0 && errno == EINTR);

      if (n <= 0)
      {
        if (errno == EAGAIN)
          n = 0;
        else
        {
          log_error("error in write; errno=" << errno);
          throw Exception(errno, "write");
        }
      }

      buffer += n;
      s -= n;

      if (s <= 0)
        break;

      if (getTimeout() == 0)
      {
        log_debug("write timeout (" << getTimeout() << "ms)");
        throw Timeout();
      }

      doPoll(POLLOUT);
    }

    return bufsize;
  }

  streambuf::streambuf(Stream& stream, unsigned bufsize, int timeout)
    : m_stream(stream),
      m_bufsize(bufsize),
      m_buffer(new char_type[bufsize])
  {
    setTimeout(timeout);
  }

  streambuf::int_type streambuf::overflow(streambuf::int_type c)
  {
    log_debug("streambuf::overflow");

    if (pptr() != pbase())
    {
      int n = m_stream.write(pbase(), pptr() - pbase());
      if (n <= 0)
        return traits_type::eof();
    }

    setp(m_buffer, m_buffer + m_bufsize);
    if (c != traits_type::eof())
    {
      *pptr() = (char_type)c;
      pbump(1);
    }

    return 0;
  }

  streambuf::int_type streambuf::underflow()
  {
    log_debug("streambuf::underflow");

    Stream::size_type n = m_stream.read(m_buffer, m_bufsize);
    if (n <= 0)
      return traits_type::eof();

    setg(m_buffer, m_buffer, m_buffer + n);
    return (int_type)(unsigned char)m_buffer[0];
  }

  int streambuf::sync()
  {
    log_debug("streambuf::sync");

    if (pptr() != pbase())
    {
      int n = m_stream.write(pbase(), pptr() - pbase());
      if (n <= 0)
        return -1;
      else
        setp(m_buffer, m_buffer + m_bufsize);
    }
    return 0;
  }

} // namespace net

} // namespace cxxtools
