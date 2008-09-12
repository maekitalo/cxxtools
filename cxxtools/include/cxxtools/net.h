/* cxxtools/net.h
 *
 * cxxtools - general purpose C++-toolbox
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
 *
 */

#ifndef CXXTOOLS_NET_H
#define CXXTOOLS_NET_H

#include <string>
#include <stdexcept>
#include <iterator>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <cxxtools/syserror.h>
#include <cxxtools/noncopyable.h>

namespace cxxtools
{

namespace net
{
  //////////////////////////////////////////////////////////////////////
  /**
   * net::Exception is thrown on error
   */
  class Exception : public SysError
  {
    public:
      /// The class is initialized with a message containing the error number,
      /// the error message from strerror(3) and the passed function name.
      Exception(int _errno, const char* fn)
        : SysError(_errno, fn)
        { }
      /// Initializes the excpetion with a message containing the error number
      /// from errno, the error message from strerror(3) and the passed function name.
      explicit Exception(const char* fn)
        : SysError(fn)
        { }
  };

  //////////////////////////////////////////////////////////////////////
  /**
   * net::Timeout is thrown on timeout.
   */
  class Timeout : public Exception
  {
    public:
      Timeout()
        : Exception(0, "Timeout")
      { }
  };

  //////////////////////////////////////////////////////////////////////
  /**
   * Wrapper for BSD sockets.
   */
  class Socket : private NonCopyable
  {

    public:
      /// A socket is created. On error a net::Exception is thrown.
      Socket(int domain, int type, int protocol) throw (Exception);

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
      /// Liefert true zurück, wenn der Socket nicht geöffnet ist
      /// Returns true, if no socket is held.
      bool bad() const      { return m_sockFd <  0; }
      /// Returns true, if a socket is held.
      operator bool() const { return m_sockFd >= 0; }

      /// Creates a new socket. If a socket is already associated with this
      /// class, it is closed.
      void create(int domain, int type, int protocol) throw (Exception);
      /// Closes the socket, if a socket is held.
      void close();

      /// Returns the socket handle.
      int getFd() const     { return m_sockFd; }

      /// wrapper around getsockname(2)
      struct sockaddr_storage getSockAddr() const throw (Exception);

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

  class Addrinfo : private cxxtools::NonCopyable
  {
      struct addrinfo* ai;
      void init(const std::string& ipaddr, unsigned short port,
                const addrinfo& hints);

    public:
      Addrinfo(const std::string& ipaddr, unsigned short port);
      Addrinfo(const std::string& ipaddr, unsigned short port,
               const addrinfo& hints)
        : ai(0)
        { init(ipaddr, port, hints); }
      ~Addrinfo();

      class const_iterator : public std::iterator<std::forward_iterator_tag, addrinfo>
      {
          struct addrinfo* current;

        public:
          explicit const_iterator(struct addrinfo* ai = 0)
            : current(ai)
            { }
          bool operator== (const const_iterator& it) const
            { return current == it.current; }
          bool operator!= (const const_iterator& it) const
            { return current != it.current; }
          const_iterator& operator++ ()
            { current = current->ai_next; return *this; }
          const_iterator operator++ (int)
            {
              const_iterator ret(current);
              current = current->ai_next;
              return ret;
            }
          reference operator* () const
            { return *current; }
          pointer operator-> () const
            { return current; }
      };

      const_iterator begin() const  { return const_iterator(ai); }
      const_iterator end() const    { return const_iterator(); }
  };

} // namespace net

} // namespace cxxtools

#endif // CXXTOOLS_NET_H
