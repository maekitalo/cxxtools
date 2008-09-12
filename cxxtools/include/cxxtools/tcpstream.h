/* cxxtools/tcpstream.h
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

#ifndef CXXTOOLS_NET_TCPSTREAM_H
#define CXXTOOLS_NET_TCPSTREAM_H

#include <iostream>
#include <string>
#include <stdexcept>
#include <netinet/in.h>
#include <sys/socket.h>
#include <cxxtools/net.h>

namespace cxxtools
{

namespace net
{
  //////////////////////////////////////////////////////////////////////
  /**
   * Serversocket
   */
  class Server : public Socket
  {
      struct sockaddr_storage servaddr;

    public:
      Server()  {}

      /// creates a server socket and listens on the address
      Server(const std::string& ipaddr, unsigned short int port, int backlog = 5)
             throw (Exception);

      void listen(const std::string& ipaddr, unsigned short int port, int backlog = 5) throw (Exception);

      const struct sockaddr_storage& getAddr() const
        { return servaddr; }

    private:
  };

  //////////////////////////////////////////////////////////////////////
  /**
   * Stream client socket.
   *
   * Stream is a unbuffered client stream socket.
   */
  class Stream : public Socket
  {
      struct sockaddr_storage peeraddr;

    public:
      typedef size_t size_type;

      /// initialize an empty stream socket
      Stream();
      /// initialize and accepts a connection from a server.
      explicit Stream(const Server& server);

      /// Creates a connected socket to the specified ip adress.
      Stream(const std::string& ipaddr, unsigned short int port);

      /// Creates a socket from an existing file descriptor. The class takes
      /// the ownership of this descriptor. That means, that the descriptor is
      /// closed, when the stream is destroyed.
      explicit Stream(int fd)
        : Socket(fd)
        { }

      /// On error a exception of type cxxtools::net::Exception is thrown.
      void accept(const Server& server);
      /// Connects to the passed ip address.
      void connect(const std::string& ipaddr, unsigned short int port);

      /// reads up to bufsize bytes from the socket.
      size_type read(char* buffer, size_type bufsize) const;
      /// Writes up to bufsize bytes to the socket.
      /// If flush is not set, the write may return after less than bufsize
      /// bytes are sent.
      size_type write(const char* buffer, size_type bufsize, bool flush = true) const;

      /// returns the current peer address.
      const struct sockaddr_storage& getPeeraddr() const
        { return peeraddr; }
  };

  //////////////////////////////////////////////////////////////////////
  /**
   * cxxtools::net::streambuf is a std::streambuf with a underlying socket.
   *
   */
  class streambuf : public std::streambuf
  {
    public:
      /// initializes  the streambuf and allocates a buffer.
      explicit streambuf(Stream& stream, unsigned bufsize = 8192,
        int timeout = -1);

      /// all resources are freed
      ~streambuf()
      { delete[] m_buffer; }

      /// sets the timeout to t in milliseconds.
      /// If the time is < 0, the socket uses blocking operations.
      void setTimeout(int t)   { m_stream.setTimeout(t); }

      /// return the current timeout value.
      int getTimeout() const   { return m_stream.getTimeout(); }

      /// overridden from std::streambuf
      int_type overflow(int_type c);
      /// overridden from std::streambuf
      int_type underflow();
      /// overridden from std::streambuf
      int sync();

    private:
      Stream&    m_stream;
      unsigned   m_bufsize;
      char_type* m_buffer;
  };

  //////////////////////////////////////////////////////////////////////
  /**
   * std::iostream around a stream socket.
   */
  class iostream : public std::iostream, public Stream
  {
    public:
      /// Initializes a iostream with the given buffer sizce
      explicit iostream(unsigned bufsize = 8192, int timeout = -1)
        : std::iostream(0),
          m_buffer(*this, bufsize, timeout)
      {
        init(&m_buffer);
      }

      /// Accepts a connection from a server socket.
      explicit iostream(const Server& server, unsigned bufsize = 8192, int timeout = -1)
        : std::iostream(0),
          Stream(server),
          m_buffer(*this, bufsize, timeout)
      {
        rdbuf(&m_buffer);
      }

      /// Connects to a server.
      iostream(const char* ipaddr, unsigned short int port, unsigned bufsize = 8192)
        : std::iostream(0),
          Stream(ipaddr, port),
          m_buffer(*this, bufsize)
      {
        rdbuf(&m_buffer);
      }

      /// Connects to a server.
      iostream(const std::string& ipaddr, unsigned short int port, unsigned bufsize = 8192)
        : std::iostream(0),
          Stream(ipaddr, port),
          m_buffer(*this, bufsize)
      {
        rdbuf(&m_buffer);
      }

      /// override to resolve ambiguity between istream::read and Stream::read
      std::istream& read(char* s, std::streamsize n)
        { return std::iostream::read(s, n); }

      /// override to resolve ambiguity between ostream::write and Stream::write
      std::ostream& write(const char* s, std::streamsize n)
        { return std::iostream::write(s, n); }

      /// Set timeout to the given value in milliseconds.
      void setTimeout(int timeout)  { m_buffer.setTimeout(timeout); }
      /// Returns the current value for timeout in milliseconds.
      int getTimeout() const        { return m_buffer.getTimeout(); }

    private:
      streambuf m_buffer;
  };

} // namespace net

} // namespace cxxtools

#endif // CXXTOOLS_NET_TCPSTREAM_H
