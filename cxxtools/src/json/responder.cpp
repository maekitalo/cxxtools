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

#include "responder.h"
#include "rpcserverimpl.h"
#include <cxxtools/serviceprocedure.h>
#include <cxxtools/serviceregistry.h>
#include <cxxtools/remoteexception.h>
#include <cxxtools/textstream.h>
#include <cxxtools/utf8codec.h>
#include <cxxtools/log.h>

log_define("cxxtools.json.responder")

namespace cxxtools
{
namespace json
{
Responder::Responder(ServiceRegistry& serviceRegistry)
    : _serviceRegistry(serviceRegistry)
{
    _deserializer.begin();
    _parser.begin(_deserializer);
}

Responder::~Responder()
{
}

void Responder::onInput(IOStream& ios)
{
    while (ios.buffer().in_avail() > 0)
    {
        if (advance(ios.buffer().sbumpc()))
        {
            finalize(ios);
            ios.buffer().beginWrite();
            _deserializer.begin();
            _parser.begin(_deserializer);
        }
    }

}

void Responder::finalize(std::ostream& out)
{
    log_trace("finalize");

    std::string methodName;
    _deserializer.si()->getMember("method") >>= methodName;

    log_debug("method = " << methodName);
    ServiceProcedure* proc = _serviceRegistry.getProcedure(methodName);

    // compose arguments
    IComposer** args = proc->beginCall();

    // process args
    const SerializationInfo& params = _deserializer.si()->getMember("params");
    SerializationInfo::ConstIterator it = params.begin();
    if (args)
    {
        for (int a = 0; args[a]; ++a)
        {
            if (it == params.end())
                throw RemoteException("argument expected");
            args[a]->fixup(*it);
            ++it;
        }
    }

    if (it != params.end())
        throw RemoteException("too many arguments");

    TextOStream ts(out, new Utf8Codec());

    _formatter.begin(ts);

    _formatter.beginObject(std::string(), std::string(), std::string());

    _formatter.addValue("id", "int", static_cast<Formatter::int_type>(0), std::string());

    try
    {
        IDecomposer* result;
        result = proc->endCall();

        _formatter.beginValue("result");
        result->format(_formatter);
        _formatter.finishValue();
    }
    catch (const RemoteException& e)
    {
        log_debug("method \"" << methodName << "\" exited with RemoteException: " << e.what());

        _formatter.beginObject("error", std::string(), std::string());

        _formatter.beginMember("code");
        _formatter.addValue("code", "int", static_cast<Formatter::int_type>(e.rc()), std::string());
        _formatter.finishMember();

        _formatter.beginMember("message");
        _formatter.addValue("message", std::string(), String(e.what()), std::string());
        _formatter.finishMember();

        _formatter.finishObject();
    }
    catch (const std::exception& e)
    {
        log_debug("method \"" << methodName << "\" exited with exception: " << e.what());
        _formatter.addValue("error", std::string(), String(e.what()), std::string());
    }

    _formatter.finishObject();

    _serviceRegistry.releaseProcedure(proc);
}

bool Responder::advance(char ch)
{
    return _parser.advance(ch) != 0;
}

}
}
