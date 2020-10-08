/*
 * Copyright (C) 2006-2009 by Marc Boris Duerner, Tommi Maekitalo
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

#ifndef CXXTOOLS_net_TcpSocket_h
#define CXXTOOLS_net_TcpSocket_h

#include <cxxtools/iodevice.h>
#include <cxxtools/net/addrinfo.h>
#include <cxxtools/signal.h>
#include <cxxtools/delegate.h>
#include <cxxtools/string.h>
#include <cxxtools/datetime.h>
#include <string>

namespace cxxtools {

class SslCertificate;
class SslCertificateNotAccepted : public std::runtime_error
{
public:
    SslCertificateNotAccepted()
        : std::runtime_error("certificate not accepted")
        { }
};


namespace net {

class TcpServer;
class AddrInfo;

class TcpSocket : public IODevice
{
        class TcpSocketImpl* _impl;

    public:
        // flags for accept method
        enum { INHERIT = 1, DEFER_ACCEPT = 2 };

        TcpSocket();

        TcpSocket(const TcpServer& server, unsigned flags = 0);

        TcpSocket(const std::string& ipaddr, unsigned short int port);

        explicit TcpSocket(const AddrInfo& addrinfo);

        ~TcpSocket();

        std::string getSockAddr() const;

        std::string getPeerAddr() const;

        void setTimeout(Milliseconds timeout);

        Milliseconds timeout() const;

        Milliseconds getTimeout() const
        { return timeout(); }

        void accept(const TcpServer& server, unsigned flags = 0);

        void connect(const AddrInfo& addrinfo);

        void connect(const std::string& ipaddr, unsigned short int port)
        { connect(AddrInfo(ipaddr, port)); }

        bool beginConnect(const AddrInfo& addrinfo);

        bool beginConnect(const std::string& ipaddr, unsigned short int port)
        { return beginConnect(AddrInfo(ipaddr, port)); }

        void endConnect();

        Signal<TcpSocket&> connected;
        Signal<TcpSocket&> sslAccepted;
        Signal<TcpSocket&> sslConnected;
        Signal<TcpSocket&> sslClosed;
        Delegate<bool, const SslCertificate&> acceptSslCertificate;

        /** @brief Notifies when the device is closed while no reading or writing is pending
         */
        Signal<TcpSocket&> closed;
 
        bool isConnected() const;
        bool isSslConnected() const;

        int getFd() const;

        short poll(short events) const;

        void loadSslCertificateFile(const std::string& certFile, const std::string& privateKeyFile = std::string());

        /** Enables ssl peer certificate check.
         *
         *  Level 0: Disables peer certificate checking, which is the default.
         *
         *  Level 1: In server mode sends a client certificate request.  Verify
         *  peer certificate and fail if not valid.  In server mode accept if
         *  client sends no certificate.
         *
         *  Level 2: As level 1 but in server mode do not accept ssl connection
         *  without client certificate.
         *
         *  A `ca` must be given if `level` > 0. It specifies the certification
         *  authority file or directory.
         *
         *  In any case the delegate `acceptSslCertificate` is called if
         *  connected.  If the delegate returns false, the ssl connection is
         *  denied. A exception of type `cxxtools::SslCertificateNotAccepted`
         *  is thrown.
         */
        void setSslVerify(int level, const std::string& ca = std::string());

        bool hasSslPeerCertificate() const;
        const SslCertificate& getSslPeerCertificate() const;

        /// initiates a ssl connection on the socket
        void beginSslConnect();
        void endSslConnect();

        /// blocking call for initiating ssl
        void sslConnect();

        /// accept a ssl connection from the peer
        void beginSslAccept();
        void endSslAccept();

        /// blocking call to accept a ssl connection from the peer
        void sslAccept();

        /// terminates ssl
        void beginSslShutdown();
        void endSslShutdown();

        /// blocking call to terminate ssl
        void sslShutdown();

    protected:
        TcpSocket(TcpSocketImpl* impl)
        : _impl(impl)
        { }

        // inherit doc
        virtual size_t onBeginRead(char* buffer, size_t n, bool& eof);

        // inherit doc
        virtual size_t onBeginWrite(const char* buffer, size_t n);

    public:
        // inherit doc
        virtual SelectableImpl& simpl();

        // inherit doc
        virtual IODeviceImpl& ioimpl();
};

} // namespace net

} // namespace cxxtools

#endif
