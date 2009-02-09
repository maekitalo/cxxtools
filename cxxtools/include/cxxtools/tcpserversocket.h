/*
 * Copyright (C) 2006 by Marc Boris Duerner, Tommi Maekitalo
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

#ifndef CXXTOOLS_NET_TcpServerSocket_H
#define CXXTOOLS_NET_TcpServerSocket_H

#include <string>

namespace cxxtools {

namespace net {

  class TcpServerSocket
  {
    class TcpServerSocketImpl* _impl;

    public:
      TcpServerSocket()
      : _impl(0)
      {}

      /// creates a server socket and listens on the address
      TcpServerSocket(const std::string& ipaddr, unsigned short int port, int backlog = 5)
      : _impl(0)
      { this->listen(ipaddr, port, backlog); }

      ~TcpServerSocket();

      void close();

      void listen(const std::string& ipaddr, unsigned short int port, int backlog = 5);

      const struct sockaddr_storage& getAddr() const;

      int getFd() const;
  };

} // namespace net

} // namespace cxxtools

#endif // CXXTOOLS_NET_TCPSTREAM_H
