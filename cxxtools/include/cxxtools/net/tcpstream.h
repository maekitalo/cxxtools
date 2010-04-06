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

#ifndef CXXTOOLS_NET_TCPSTREAM_H
#define CXXTOOLS_NET_TCPSTREAM_H

#include <string>
#include <cxxtools/net/tcpserver.h>
#include <cxxtools/net/tcpsocket.h>
#include <cxxtools/net/addrinfo.h>
#include <cxxtools/iostream.h>

namespace cxxtools
{

namespace net
{
    class TcpStream : public IOStream
    {
        public:
            explicit TcpStream(unsigned bufsize = 8192, std::size_t timeout = Selectable::WaitInfinite)
            : IOStream(bufsize)
            {
                _socket.setTimeout(timeout);
                this->attachDevice(_socket);
            }

            TcpStream(const std::string& ipaddr, unsigned short int port,
                      unsigned bufsize = 8192, std::size_t timeout = Selectable::WaitInfinite)
            : IOStream(bufsize)
            , _socket(ipaddr, port)
            {
                _socket.setTimeout(timeout);
                this->attachDevice(_socket);
            }

            explicit TcpStream(const AddrInfo& addrinfo,
                      unsigned bufsize = 8192, std::size_t timeout = Selectable::WaitInfinite)
            : IOStream(bufsize)
            , _socket(addrinfo)
            {
                _socket.setTimeout(timeout);
                this->attachDevice(_socket);
            }

            TcpStream(const char* ipaddr, unsigned short int port,
                      unsigned bufsize = 8192, std::size_t timeout = Selectable::WaitInfinite)
            : IOStream(bufsize)
            , _socket(ipaddr, port)
            {
                _socket.setTimeout(timeout);
                this->attachDevice(_socket);
            }

            explicit TcpStream(TcpServer& server, unsigned bufsize = 8192,
                      unsigned flags = 0, std::size_t timeout = Selectable::WaitInfinite)
            : IOStream(bufsize)
            , _socket(server, flags)
            {
               _socket.setTimeout(timeout);
                this->attachDevice(_socket);
            }

            /// Set timeout to the given value in milliseconds.
            void setTimeout(std::size_t timeout)
            { _socket.setTimeout(timeout); }

            /// Returns the current value for timeout in milliseconds.
            std::size_t getTimeout() const
            { return _socket.getTimeout(); }

            void close()
            { _socket.close(); }

            void connect(const AddrInfo& addrinfo)
            { _socket.connect(addrinfo); }

            void connect(const std::string& ipaddr, unsigned short int port)
            { _socket.connect(ipaddr, port); }

            void accept(const TcpServer& server, unsigned flags = 0)
            { _socket.accept(server, flags); }

            std::string getSockAddr() const
            { return _socket.getSockAddr(); }

            std::string getPeerAddr() const
            { return _socket.getPeerAddr(); }

            bool isConnected() const
            { return _socket.isConnected(); }

            int getFd() const
            { return _socket.getFd(); }

        private:
            TcpSocket _socket;
    };

    typedef TcpStream iostream;

} // namespace net

} // namespace cxxtools

#endif // CXXTOOLS_NET_TCPSTREAM_H
