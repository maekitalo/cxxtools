/*
 * Copyright (C) 2003,2009 Tommi Maekitalo
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

#include "addrinfoimpl.h"
#include "error.h"

#include <cxxtools/net/addrinfo.h>
#include <cxxtools/systemerror.h>

#include <string>
#include <sstream>
#include <string.h>

namespace cxxtools
{

namespace net
{
  namespace
  {
      std::string getAddrInfoError(int ret, const std::string& host, unsigned short port)
      {
          if (ret == EAI_SYSTEM)
              return getErrnoString("getaddrinfo");

          std::ostringstream msg;
          msg << "error " << ret << " in function getaddrinfo while searching for <" << host << ':' << port << ">: " << ::gai_strerror(ret);
          return msg.str();
      }
  }

  AddrInfoError::AddrInfoError(int ret, const std::string& host, unsigned short port)
    : SystemError(getAddrInfoError(ret, host, port))
  {
  }


  void AddrInfoImpl::init(const std::string& host, unsigned short port)
  {
    struct addrinfo hints;

    // give some useful default values to use for getaddrinfo()
    memset(&hints, 0, sizeof(hints));
    hints.ai_socktype = SOCK_STREAM;

    init(host, port, hints);
  }

  void AddrInfoImpl::init(const std::string& host, unsigned short port,
    const addrinfo& hints)
  {
    if (_ai)
    {
      freeaddrinfo(_ai);
      _ai = 0;
    }

    _host = host;
    _port = port;

    std::ostringstream p;
    p << port;

    while (true)
    {
      int ret = ::getaddrinfo(host.empty() ? 0 : host.c_str(), p.str().c_str(), &hints, &_ai);
      if (ret == 0)
        break;

      if (ret == EAI_AGAIN)
        continue;

      throw AddrInfoError(ret, host, port);
    }

    if (_ai == 0)
      throw SystemError("getaddrinfo");
  }

  AddrInfoImpl::~AddrInfoImpl()
  {
    if (_ai)
      freeaddrinfo(_ai);
  }

  const std::string& AddrInfoImpl::host() const
  {
    return _host;
  }

  unsigned short AddrInfoImpl::port() const
  {
    return _port;
  }

} // namespace net

} // namespace cxxtools
