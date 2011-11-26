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
#include <cxxtools/log.h>

log_define("cxxtools.bin.responder")

namespace cxxtools
{
namespace bin
{
Responder::~Responder()
{
    if (_proc)
        _server.releaseProcedure(_proc);
}

void Responder::reply(IOStream& out)
{
    log_info("send reply");

    out << '\x41';
    _formatter.begin(out);
    _result->format(_formatter);
    out << '\xff';

    out.buffer().beginWrite();
}

void Responder::replyError(IOStream& out, const char* msg)
{
    log_info("send error \"" << msg << '"');

    out << '\x42'
        << '\0' << '\0' << '\0' << '\0'
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
            if (_failed)
            {
                replyError(ios, _errorMessage.c_str());
            }
            else
            {
                try
                {
                    _result = _proc->endCall();
                }
                catch (const std::exception& e)
                {
                    ios.buffer().discard();
                    replyError(ios, e.what());
                }

                reply(ios);
            }

            _server.releaseProcedure(_proc);
            _proc = 0;
            _args = 0;
            _result = 0;
            _state = state_0;
            _failed = false;
            _errorMessage.clear();

        }
    }

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
                log_info("rpc method \"" << _methodName << '"');

                _proc = _server.getProcedure(_methodName);

                if (_proc)
                {
                    _args = _proc->beginCall();
                    _state = state_params;
                }
                else
                {
                    _failed = true;
                    _errorMessage = "unknown method \"" + _methodName + '"';
                    _state = state_params_skip;
                }

                _methodName.clear();
            }
            else
                _methodName += ch;
            break;

        case state_params:
            if (ch == '\xff')
            {
                if (_args && *_args)
                {
                    _failed = true;
                    _errorMessage = "argument expected";
                }

                return true;
            }
            else
            {
                if (_args == 0 || *_args == 0)
                {
                    _failed = true;
                    _errorMessage = "too many arguments";
                    _state = state_params_skip;
                }
                else
                {
                    _valueParser.begin(**_args, _context);
                    _valueParser.advance(ch);
                    _state = state_param;
                }
            }
            break;

        case state_params_skip:
            if (ch == '\xff')
                return true;
            else
            {
                _valueParser.beginSkip();
                _valueParser.advance(ch);
                _state = state_param_skip;
            }

            break;

        case state_param:
            if (_valueParser.advance(ch))
            {
                try
                {
                    (*_args)->fixup(_context);
                    ++_args;
                    _state = state_params;
                }
                catch (const std::exception& e)
                {
                    _failed = true;
                    _errorMessage = e.what();
                    _state = state_params_skip;
                }
            }
            break;

        case state_param_skip:
            if (_valueParser.advance(ch))
                _state = state_params_skip;

            break;
    }

    return false;
}

}
}
