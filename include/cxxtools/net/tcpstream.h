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
    /** A buffered TCP/IP stream which supports asyncronous communication.
     *
     *  When a object is created a buffer size is passed. By default the buffer size
     *  is 0, which means, that the buffer is dynamically extended when needed so that
     *  writing do not block.
     */
    class TcpStream : public IOStream, public Connectable
    {
            void init(cxxtools::Timespan timeout);

        public:
            /// Creates a not connected TCP stream object.
            explicit TcpStream(unsigned bufsize = 0, cxxtools::Milliseconds timeout = Selectable::WaitInfinite)
            : IOStream(bufsize > 0 ? bufsize : 8192, bufsize == 0)
            {
                init(timeout);
            }

            /// Creates a TCP stream object and connects to the specified IP address and port.
            TcpStream(const std::string& ipaddr, unsigned short int port,
                      unsigned bufsize = 0, cxxtools::Milliseconds timeout = Selectable::WaitInfinite)
            : IOStream(bufsize > 0 ? bufsize : 8192, bufsize == 0)
            , _socket(ipaddr, port)
            {
                init(timeout);
            }

            /// Creates a TCP stream object and connects to the specified address info.
            explicit TcpStream(const AddrInfo& addrinfo,
                      unsigned bufsize = 0, cxxtools::Milliseconds timeout = Selectable::WaitInfinite)
            : IOStream(bufsize > 0 ? bufsize : 8192, bufsize == 0)
            , _socket(addrinfo)
            {
                init(timeout);
            }

            /// Creates a TCP stream object and connects to the specified IP address and port.
            TcpStream(const char* ipaddr, unsigned short int port,
                      unsigned bufsize = 8192, cxxtools::Milliseconds timeout = Selectable::WaitInfinite)
            : IOStream(bufsize, true)
            , _socket(ipaddr, port)
            {
                init(timeout);
            }

            /// Creates a TCP stream object and accepts a connection from a server.
            explicit TcpStream(TcpServer& server, unsigned bufsize = 8192,
                      unsigned flags = 0, cxxtools::Milliseconds timeout = Selectable::WaitInfinite)
            : IOStream(bufsize, true)
            , _socket(server, flags)
            {
                init(timeout);
            }

            /// Set timeout to the given value in milliseconds.
            void setTimeout(Milliseconds timeout)
            { _socket.setTimeout(timeout); }

            /// Returns the current value for timeout in milliseconds.
            Milliseconds getTimeout() const
            { return _socket.getTimeout(); }

            void close()
            { _socket.close(); }

            bool beginConnect(const AddrInfo& addrinfo)
            { return _socket.beginConnect(addrinfo); }

            bool beginConnect(const std::string& ipaddr, unsigned short int port)
            { return _socket.beginConnect(ipaddr, port); }

            void endConnect()
            { _socket.endConnect(); }

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

            TcpSocket& socket()
            { return _socket; }

            /** @brief Notifies about available data

                This signal is send when the Socket is monitored
                in a Selector or EventLoop and data becomes available.

                The system must call beginRead() to monitor the tcpstream
                for reading. After the signal is received, data is available
                in the stream or in case of disconnection, reading fails.
            */
            Signal<TcpStream&> inputReady;

            /** @brief Notifies when data can be written

                This signal is send when the Socket is monitored
                in a Selector or EventLoop and the device is ready
                to write data.
            */
            Signal<TcpStream&> outputReady;

            /** @brief Notifies when the device is connected after beginConnect
             */
            Signal<TcpStream&> connected;

            /** @brief Notifies when the device is closed while no reading or writing is pending
             */
            Signal<TcpStream&> closed;

        private:
            TcpSocket _socket;

            void onInput(IODevice&);
            void onOutput(IODevice&);
            void onConnected(TcpSocket&);
            void onClosed(TcpSocket&);
    };

    typedef TcpStream iostream;

} // namespace net

} // namespace cxxtools

#endif // CXXTOOLS_NET_TCPSTREAM_H
