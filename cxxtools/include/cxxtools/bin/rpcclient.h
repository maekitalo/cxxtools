/*
 * Copyright (C) 2011 Tommi Maekitalo
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * is provided AS IS, WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, and
 * NON-INFRINGEMENT.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 *
 */

#ifndef CXXTOOLS_BIN_CLIENT_H
#define CXXTOOLS_BIN_CLIENT_H

#include <cxxtools/remoteclient.h>
#include <string>

namespace cxxtools
{

class SelectorBase;

namespace bin
{

class RpcClientImpl;

class RpcClient : public RemoteClient
{
        RpcClientImpl* _impl;

        RpcClient(RpcClient&) { }
        void operator= (const RpcClient&) { }

    public:
        RpcClient()
        : _impl(0)
        { }

        RpcClient(SelectorBase& selector, const std::string& addr, unsigned short port);

        RpcClient(const std::string& addr, unsigned short port);

        virtual ~RpcClient();

        void setSelector(SelectorBase& selector);

        void connect(const std::string& addr, unsigned short port);

        void close();

        void beginCall(IComposer& r, IRemoteProcedure& method, IDecomposer** argv, unsigned argc);

        void endCall();

        void call(IComposer& r, IRemoteProcedure& method, IDecomposer** argv, unsigned argc);

        const IRemoteProcedure* activeProcedure() const;

        void cancel();

};

}

}

#endif // CXXTOOLS_BIN_CLIENT_H
