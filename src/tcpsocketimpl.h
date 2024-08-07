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

#ifndef CXXTOOLS_NET_TcpSocketImpl_H
#define CXXTOOLS_NET_TcpSocketImpl_H

#include "iodeviceimpl.h"
#include <cxxtools/net/addrinfo.h>
#include <cxxtools/sslcertificate.h>
#include "addrinfoimpl.h"

#include <openssl/ssl.h>

#include <string>
#include <vector>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/poll.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <unistd.h>

namespace cxxtools
{

class SelectorBase;
class Timespan;
class SslCtx;

namespace net
{

class TcpServer;
class TcpSocket;

union Sockaddr
{
    struct sockaddr_storage storage;
    struct sockaddr         sa;
    struct sockaddr_in      sa_in;
#ifdef HAVE_IPV6
    struct sockaddr_in6     sa_in6;
#endif
    struct sockaddr_un      sa_un;
};

void formatIp(const Sockaddr& addr, std::string& str);

inline std::string formatIp(const Sockaddr& addr)
{
    std::string ret;
    formatIp(addr, ret);
    return ret;
}

std::string getSockAddr(int fd);

class TcpSocketImpl : public IODeviceImpl
{
    private:
        TcpSocket& _socket;
        enum State {
            IDLE,
            CONNECTING,
            CONNECTED,
            SSLACCEPTING,
            SSLCONNECTING,
            SSLSHUTTINGDOWN,

            SSLCONNECTED,
            THROWING
        } _state;

        struct sockaddr_storage _peeraddr;
        AddrInfo _addrInfo;
        AddrInfoImpl::const_iterator _addrInfoPtr;
        std::string _connectResult;
        std::vector<std::string> _connectFailedMessages;
        DestructionSentry* _sentry;
        SSL* _ssl;
        mutable bool _peerCertificateLoaded;
        mutable SslCertificate _peerCertificate;

        // methods
        int checkConnect();
        size_t callSend(const char* buffer, size_t n);
        void checkPendingError();
        std::string tryConnect();
        std::string connectFailedMessages();

        void checkSslOperation(int ret, const char* fn, pollfd* pfd);
        void waitSslOperation(int ret, cxxtools::Timespan timeout);

        void initSsl(const SslCtx& sslCtx);

    public:
        explicit TcpSocketImpl(TcpSocket& socket);

        ~TcpSocketImpl();

        void close();

        std::string getSockAddr() const;

        std::string getPeerAddr() const;

        bool isConnected() const
        { return _state >= CONNECTED; }

        bool isSslConnected() const
        { return _state == SSLCONNECTED; }

        bool beginConnect(const AddrInfo& addrinfo);

        void endConnect();

        void accept(const TcpServer& server, unsigned flags);

        void terminateAccept();

        // implementation using poll
        void initWait(pollfd& pfd) override;

        // implementation using poll
        bool checkPollEvent(pollfd& pfd) override;

        // override beginWrite to use send(2) instead of write(2)
        size_t beginWrite(const char* buffer, size_t n) override;

        // override write to use send(2) instead of write(2)
        size_t write(const char* buffer, size_t count) override;

        // override for ssl
        size_t read(char* buffer, size_t count, bool& eof) override;

        // override for ssl
        void inputReady() override;

        // override for ssl
        void outputReady() override;

        const SslCertificate& getSslPeerCertificate() const;

        // initiates a ssl connection on the socket
        bool beginSslConnect(const SslCtx& ctx);
        bool continueSslConnect();
        void endSslConnect();

        // accept a ssl connection from the peer
        bool beginSslAccept(const SslCtx& ctx);
        bool continueSslAccept();
        void endSslAccept();

        // terminates ssl
        bool beginSslShutdown();
        void endSslShutdown();

        void verifySslCertificate();
};

} // namespace net

} // namespace cxxtools

#endif
