/*
 * Copyright (C) 2005 Tommi Maekitalo
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

#ifndef CXXTOOLS_NET_H
#define CXXTOOLS_NET_H

#include <cxxtools/ioerror.h>
#include <cxxtools/noncopyable.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

namespace cxxtools
{

namespace net
{
  //////////////////////////////////////////////////////////////////////
  /**
   * net::Timeout is thrown on timeout.
   */
  class Timeout : public IOError
  {
    public:
      Timeout();
  };

  class AddressInUse : public IOError
  {
    public:
      AddressInUse();
  };

  //////////////////////////////////////////////////////////////////////
  /**
   * Wrapper for BSD sockets.
   */
  class Socket : private NonCopyable
  {

    public:
      /// A socket is created. On error a net::Exception is thrown.
      Socket(int domain, int type, int protocol);

      /// A socket is initialized with a existing socket descriptor.
      /// Ownership is transfered to this class.
      explicit Socket(int fd = -1)
        : m_sockFd(fd),
          m_timeout(-1)
        { }

      /// The socket is released.
      /// Errors are printed on stderr.
      virtual ~Socket();

      /// Returns true, if a socket is held.
      bool good() const     { return m_sockFd >= 0; }
      /// Liefert true zur�ck, wenn der Socket nicht ge�ffnet ist
      /// Returns true, if no socket is held.
      bool bad() const      { return m_sockFd <  0; }
      /// Returns true, if a socket is held.
      operator bool() const { return m_sockFd >= 0; }

      /// Creates a new socket. If a socket is already associated with this
      /// class, it is closed.
      void create(int domain, int type, int protocol);
      /// Closes the socket, if a socket is held.
      void close();

      /// Returns the socket handle.
      int getFd() const     { return m_sockFd; }

      /// wrapper around getsockname(2)
      struct sockaddr_storage getSockAddr() const;

      /// Set timeout in milliseconds.
      void setTimeout(int t);
      /// Returns timeout in milliseconds.
      int getTimeout() const  { return m_timeout; }

      /// execute poll(2) - throws Timeout-exception, when no data available
      /// after timeout
      short poll(short events) const;

    protected:
      void setFd(int sockFd);

    private:
      int m_sockFd;
      int m_timeout;
  };

} // namespace net

} // namespace cxxtools

#endif // CXXTOOLS_NET_H
