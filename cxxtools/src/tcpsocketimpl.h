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

#ifndef PT_NET_TcpSocketImpl_H
#define PT_NET_TcpSocketImpl_H

#include "cxxtools/api.h"
#include "cxxtools/signal.h"
#include "selectableimpl.h"
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/poll.h>
#include <sys/time.h>
#include <unistd.h>

namespace cxxtools {

class SelectorBase;

namespace net {

class TcpServer;
class TcpSocket;

class TcpSocketImpl : public SelectableImpl
{
    private:
        TcpSocket& _socket;
        bool _isConnected;
        int _fd;
        std::size_t _timeout;
        struct sockaddr_storage _peeraddr;
        pollfd* _pfd;

    public:
        TcpSocketImpl(TcpSocket& socket);

        ~TcpSocketImpl();

        int fd() const
        { return _fd; }

        void close();

        std::string getSockAddr() const;

        void setTimeout(std::size_t msecs)
        { _timeout = msecs; }

        std::size_t timeout() const
        { return _timeout; }

        bool isConnected() const
        { return _isConnected; }

        void connect(const std::string& ipaddr, unsigned short int port);

        bool beginConnect(const std::string& ipaddr, unsigned short int port);

        void endConnect();

        void accept(TcpServer& server);

        bool wait(std::size_t msecs);

        void attach(SelectorBase& sb);

        void detach(SelectorBase& sb);

        // implementation using poll
        std::size_t pollSize() const;

        // implementation using poll
        std::size_t initializePoll(pollfd* pfd, std::size_t pollSize);

        // implementation using poll
        bool checkPollEvent();
};

} // namespace net

} // namespace cxxtools

#endif
