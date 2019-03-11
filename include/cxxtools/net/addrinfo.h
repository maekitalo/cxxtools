/*
 * Copyright (C) 2005,2009 Tommi Maekitalo
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

#ifndef CXXTOOLS_NET_ADDRINFO_H
#define CXXTOOLS_NET_ADDRINFO_H

#include <string>
#include <cxxtools/systemerror.h>

namespace cxxtools
{

namespace net
{

    class AddrInfoImpl;

    class AddrInfo
    {
        public:
            AddrInfo()
                : _impl(0)
                { }
            explicit AddrInfo(AddrInfoImpl* impl);
            AddrInfo(const std::string& host, unsigned short port, bool listen = false);
            AddrInfo(const AddrInfo& src);
            ~AddrInfo();

            AddrInfo& operator= (const AddrInfo& src);

            const std::string& host() const;
            unsigned short port() const;

            AddrInfoImpl* impl()               { return _impl; }
            const AddrInfoImpl* impl() const   { return _impl; }

        private:
            AddrInfoImpl* _impl;
    };

    inline bool operator== (const AddrInfo& a1, const AddrInfo& a2)
    {
        return a1.impl() == a2.impl()
            || (a1.impl() != 0 && a2.impl() != 0
                && a1.host() == a2.host()
                && a1.port() == a2.port());
    }

    inline bool operator!= (const AddrInfo& a1, const AddrInfo& a2)
    {
        return !(a1 == a2);
    }

    class AddrInfoError : public SystemError
    {
        public:
            explicit AddrInfoError(int ret);
    };

} // namespace net

} // namespace cxxtools

#endif // CXXTOOLS_NET_ADDRINFO_H
