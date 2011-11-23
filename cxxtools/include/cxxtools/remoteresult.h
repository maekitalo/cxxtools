/*
 * Copyright (C) 2011 by Dr. Marc Boris Duerner, Tommi Maekitalo
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
#ifndef CXXTOOLS_REMOTERESULT_H
#define CXXTOOLS_REMOTERESULT_H

#include <string>
#include <cxxtools/remoteclient.h>
#include <cxxtools/remoteexception.h>

namespace cxxtools
{

class IRemoteResult
{
    public:
        IRemoteResult()
            : _failed(false)
            { }

        void setFault(int rc, const std::string& msg)
        {
            _failed = true;
            _rc = rc;
            _msg = msg;
        }

        void clearFault()
        {
            _failed = false;
            _rc = 0;
            _msg.clear();
        }

        bool failed() const
        { return _failed; }

    protected:
        void checkFault() const
        {
            if (_failed)
                throw RemoteException(_msg, _rc);
        }

    private:
        bool _failed;
        int _rc;
        std::string _msg;
};

template <typename R>
class RemoteResult : public IRemoteResult
{
    public:
        explicit RemoteResult(RemoteClient& client)
            : _client(client)
        {
        }

        R& value()
        {
            return _result;
        }

        const R& get() const
        {
            _client.endCall();
            checkFault();
            return _result;
        }

    private:
        RemoteClient& _client;
        R _result;
};

}

#endif
