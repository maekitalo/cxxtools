/*
 * Copyright (C) 2009 by Dr. Marc Boris Duerner, Tommi Maekitalo
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
#include <cxxtools/remoteclient.h>
#include <string>

namespace cxxtools {

namespace xmlrpc {

class ClientImpl;

class CXXTOOLS_XMLRPC_API Client : public RemoteClient
{
        ClientImpl* _impl;

    protected:
        Client(Client&) { }
        Client& operator= (const Client&) { return *this; }

        void impl(ClientImpl* i) { _impl = i; }

    public:
        Client()
        : _impl(0)
        { }

        virtual ~Client();

        void beginCall(IComposer& r, IRemoteProcedure& method, IDecomposer** argv, unsigned argc);

        void endCall();

        void call(IComposer& r, IRemoteProcedure& method, IDecomposer** argv, unsigned argc);

        std::size_t timeout() const;
        void timeout(std::size_t t);

        std::size_t connectTimeout() const;
        void connectTimeout(std::size_t t);

        std::string url() const;

        const IRemoteProcedure* activeProcedure() const;

        void cancel();
};

}

}

#endif
