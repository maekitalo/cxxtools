/*
 * Copyright (C) 2009 Marc Boris Duerner, Tommi Maekitalo
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

#include "tcpsocketimpl.h"
#include "tcpserverimpl.h"
#include "addrinfo.h"
#include "cxxtools/log.h"
#include "cxxtools/tcpserver.h"
#include "cxxtools/systemerror.h"
#include <string.h>

log_define("cxxtools.net.tcp")

namespace cxxtools {

namespace net {

TcpSocketImpl::TcpSocketImpl()
: _fd(-1)
{
}


TcpSocketImpl::~TcpSocketImpl()
{
}


void TcpSocketImpl::create(int domain, int type, int protocol)
{
    log_debug("create socket");
    _fd = ::socket(domain, type, protocol);
    if (_fd < 0)
      throw SystemError("socket");
}


void TcpSocketImpl::close()
{
  if (_fd >= 0)
  {
    log_debug("close socket");
    ::close(_fd);
    _fd = -1;
  }
}


void TcpSocketImpl::connect(const std::string& ipaddr, unsigned short int port)
{
    log_debug("connect to " << ipaddr << " port " << port);

    Addrinfo ai(ipaddr, port);

    log_debug("do connect");
    for (Addrinfo::const_iterator it = ai.begin(); it != ai.end(); ++it)
    {
        try
        {
            this->create(it->ai_family, SOCK_STREAM, 0);
        }
        catch (const SystemError&)
        {
            continue;
        }

        if (::connect(this->fd(), it->ai_addr, it->ai_addrlen) == 0)
        {
            // save our information
            memmove(&_peeraddr, it->ai_addr, it->ai_addrlen);
            return;
        }

        // TODO add timeout handling
        //if (errno == EINPROGRESS && getTimeout() > 0)
        {
            //poll(POLLOUT);

            int sockerr;
            socklen_t optlen = sizeof(sockerr);
            if (::getsockopt(this->fd(), SOL_SOCKET, SO_ERROR, &sockerr, &optlen) != 0)
            {
                this->close();
                throw SystemError("getsockopt");
            }

            if(sockerr != 0)
            {
                this->close();
                throw SystemError(sockerr, "connect");
            }

            // save our information
            memmove(&_peeraddr, it->ai_addr, it->ai_addrlen);
            return;
        }
    }

    throw SystemError("connect");
}


void TcpSocketImpl::accept(TcpServer& server)
{
    socklen_t peeraddr_len = sizeof(_peeraddr);

    log_debug( "accept " << server.impl().fd() );
    _fd = ::accept(server.impl().fd(), reinterpret_cast <struct sockaddr*>(&_peeraddr), &peeraddr_len);
    if( _fd < 0 )
      throw SystemError("accept");

    //TODO ECONNABORTED EINTR EPERM

    log_debug( "accepted " << server.impl().fd() << " => " << _fd );
}

} // namespace net

} // namespace cxxtools
