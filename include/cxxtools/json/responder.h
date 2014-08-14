/*
 * Copyright (C) 2011 by Tommi Meakitalo
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
#ifndef CXXTOOLS_JSON_RESPONDER_H
#define CXXTOOLS_JSON_RESPONDER_H

#include <cxxtools/remoteexception.h>
#include <cxxtools/json/scanner.h>
#include <cxxtools/json/formatter.h>
#include <cxxtools/http/responder.h>
#include <cxxtools/textstream.h>

namespace cxxtools
{

class ServiceProcedure;
class IComposer;
class IDecomposer;

namespace json
{

class ServiceRegistry;

class Responder : public http::Responder
{

    public:
        explicit Responder(ServiceRegistry& service);

        ~Responder();

        void beginRequest(std::istream& in, http::Request& request);

        std::size_t readBody(std::istream& is);

        void replyError(std::ostream& os, http::Request& request,
                        http::Reply& reply, const std::exception& ex);

        void reply(std::ostream& os, http::Request& request, http::Reply& reply);

    private:
        ServiceRegistry& _serviceRegistry;

        JsonDeserializer _deserializer;

        ServiceProcedure* _proc;
        IComposer** _args;
        IDecomposer* _result;
        RemoteException _fault;
};

}

}

#endif
