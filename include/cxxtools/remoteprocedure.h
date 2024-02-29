/*
 * Copyright (C) 2011 by Tommi Maekitalo
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

#ifndef CXXTOOLS_REMOTEPROCEDURE_H
#define CXXTOOLS_REMOTEPROCEDURE_H

#include <cxxtools/remoteclient.h>
#include <cxxtools/remoteresult.h>
#include <cxxtools/composer.h>
#include <cxxtools/decomposer.h>
#include <cxxtools/signal.h>
#include <cxxtools/string.h>
#include <string>

namespace cxxtools
{

class IRemoteProcedure
{
    public:
        IRemoteProcedure(RemoteClient& client, const String& name)
        : _client(&client)
        , _name(name)
        { }

        virtual ~IRemoteProcedure()
        { cancel(); }

        RemoteClient& client()
        { return *_client; }

        void client(RemoteClient& client)
        { _client = &client; }

        const String& name() const
        { return _name; }

        virtual void setFault(int rc, const std::string& msg) = 0;

        virtual bool failed() const = 0;

        void cancel()
        {
            if (_client && _client->activeProcedure() == this)
                _client->cancel();
        }

        virtual void onFinished() = 0;

    private:
        RemoteClient* _client;
        String _name;
};


/** Base class for Remote procedure class.
 *
 */

template <typename R>
class RemoteProcedureBase : public IRemoteProcedure
{
    public:
        RemoteProcedureBase(RemoteClient& client, const String& name)
        : IRemoteProcedure(client, name),
          _result(client)
        { }

        void setFault(int rc, const std::string& msg)
        {
            _result.setFault(rc, msg);
        }

        /// access result value.
        const R& value() const
        {
            return _result.value();
        }

        /** Finalize call and return result.
         *  Exceptions are thrown when the call resulted in a exception.
         *  The method should be called just once since the exception state
         *  is partly reset.
         */
        R&& result()
        {
            return _result.get();
        }

        virtual bool failed() const
        {
            return _result.failed();
        }

        Signal< RemoteResult<R> & > finished;

        R&& end(Milliseconds msecs = RemoteClient::WaitInfinite)
        {
            try
            {
                _result.client().wait(msecs);
            }
            catch (const std::exception&)
            {
                _result.client().endCall();
                throw;
            }

            return _result.get();
        }

    protected:
        void onFinished()
        { finished.send(_result); }

        RemoteResult<R> _result;
        Composer<R> _r;
};


//! @endcond internal

#include <cxxtools/remoteprocedure.tpp>

}

#endif
