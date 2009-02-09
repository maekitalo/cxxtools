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

#include <cxxtools/addrinfo.h>
#include <cxxtools/systemerror.h>
#include <string>
#include <sstream>

namespace cxxtools {

namespace net {

  Addrinfo::Addrinfo(const std::string& ipaddr, unsigned short port)
    : ai(0)
  {
    struct addrinfo hints;

    // give some useful default values to use for getaddrinfo()
    memset(&hints, 0, sizeof(hints));
    hints.ai_socktype = SOCK_STREAM;

    init(ipaddr, port, hints);
  }

  Addrinfo::~Addrinfo()
  {
    if (ai)
      freeaddrinfo(ai);
  }

  void Addrinfo::init(const std::string& ipaddr, unsigned short port,
    const addrinfo& hints)
  {
    std::ostringstream p;
    p << port;

    // TODO: exception type
    if (0 != ::getaddrinfo(ipaddr.c_str(), p.str().c_str(), &hints, &ai))
      throw SystemError(0, ("invalid ipaddress " + ipaddr).c_str());

    // TODO: exception type
    if (ai == 0)
      throw SystemError("getaddrinfo");
  }

} // namespace net

} // namespace cxxtools
