/*
 * Copyright (C) 2009 by Dr. Marc Boris Duerner
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
#ifndef cxxtools_xmlrpc_Client_h
#define cxxtools_xmlrpc_Client_h

#include <cxxtools/xmlrpc/api.h>
#include <cxxtools/noncopyable.h>
#include <string>

namespace cxxtools {

class SelectorBase;
class ISerializer;
class IDeserializer;

namespace xmlrpc {

class IRemoteProcedure;
class ClientImpl;


class CXXTOOLS_XMLRPC_API Client : public NonCopyable
{
        ClientImpl* _impl;

    public:
        Client();

        Client(SelectorBase& selector, const std::string& addr,
               unsigned short port, const std::string& url);

        Client(const std::string& addr, unsigned short port, const std::string& url);

        virtual ~Client();

        void connect(const std::string& addr, unsigned short port,
                     const std::string& url);

        void auth(const std::string& username, const std::string& password);

        void clearAuth();

        void beginCall(IDeserializer& r, IRemoteProcedure& method, ISerializer** argv, unsigned argc);

        void call(IDeserializer& r, IRemoteProcedure& method, ISerializer** argv, unsigned argc);

        std::size_t timeout() const;

        void timeout(std::size_t t);

        std::string url() const;

};

}

}

#endif
