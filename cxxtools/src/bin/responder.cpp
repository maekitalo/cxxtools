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
#include <cxxtools/bin/valueparser.h>
#include <cxxtools/serviceprocedure.h>

namespace cxxtools
{
namespace bin
{
void Responder::reply(IOStream& out)
{
    out << '\x41';
    _formatter.begin(out);
    _result->format(_formatter);
    out << '\xff';

    out.buffer().beginWrite();
}

void Responder::replyError(IOStream& out, const char* msg)
{
    out << '\x42'
        << msg
        << '\0' << '\xff';

    out.buffer().beginWrite();
}

void Responder::onInput(IOStream& ios)
{
    while (ios.buffer().in_avail() > 0)
    {
        if (advance(ios.buffer().sbumpc()))
        {
            _args = 0;
            try
            {
                _result = _proc->endCall();
                reply(ios);
            }
            catch (const std::exception& e)
            {
                ios.buffer().discard();
                replyError(ios, e.what());
                return;
            }
        }
    }

}

Responder::~Responder()
{
    if (_proc)
        _server.releaseProcedure(_proc);
}

bool Responder::advance(char ch)
{
    switch (_state)
    {
        case state_0:
            if (ch != '\x40')
                throw std::runtime_error("method name expected");
            _state = state_method;
            break;

       case state_method:
            if (ch == '\0')
            {
                _proc = _server.getProcedure(_methodName);
                _methodName.clear();
                _args = _proc->beginCall();
                _state = state_params;
            }
            else
                _methodName += ch;
            break;

       case state_params:
            if (ch == '\xff')
            {
                if (_args && *_args)
                    throw std::runtime_error("argument expected");

                return true;
            }
            else
            {
                if (_args == 0 || *_args == 0)
                    throw std::runtime_error("too many arguments");

                _valueParser.begin(**_args, _context);
                _valueParser.advance(ch);
                _state = state_param;
            }
            break;

       case state_param:
            if (_valueParser.advance(ch))
            {
                (*_args)->fixup(_context);
                ++_args;
                _state = state_params;
            }
            break;
    }

    return false;
}

}
}
