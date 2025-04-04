/*
 * Copyright (C) 2011 Tommi Maekitalo
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

#ifndef CXXTOOLS_BIN_RESPONDER_H
#define CXXTOOLS_BIN_RESPONDER_H

#include <cxxtools/bin/deserializer.h>
#include <cxxtools/iostream.h>
#include <cxxtools/bin/formatter.h>
#include <cxxtools/serviceregistry.h>

#include <iosfwd>

namespace cxxtools
{

class ServiceProcedure;
class IComposer;
class IDecomposer;

namespace bin
{
class RpcServerImpl;
class Socket;

class Responder
{
        friend class Socket;

        enum class State
        {
            header,
            params,
            params_skip,
            param,
            param_skip
        };

    public:
        explicit Responder(ServiceRegistry& serviceRegistry)
            : _serviceRegistry(serviceRegistry),
              _state(State::header),
              _proc(0),
              _args(0),
              _result(0),
              _failed(false)
        { }

        ~Responder();

        // returns true, if request is ready and reply is put to the socket
        bool onInput(IOStream& ios);
        bool advance(std::streambuf& in);
        void reply(IOStream& out);
        void replyError(IOStream& out, const char* msg, int rc);

    private:
        ServiceRegistry& _serviceRegistry;
        State _state;
        RequestHeaderParser _headerParser;
        Deserializer _deserializer;

        ServiceProcedure* _proc;
        IComposers* _args;
        IDecomposer* _result;
        Formatter _formatter;

        bool _failed;
        std::string _errorMessage;
};
}
}
#endif // CXXTOOLS_BIN_RESPONDER_H
