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
#include <cxxtools/bin/parser.h>
#include <cxxtools/serviceprocedure.h>
#include <cxxtools/remoteexception.h>
#include <cxxtools/log.h>

log_define("cxxtools.bin.responder")

namespace cxxtools
{
namespace bin
{
Responder::~Responder()
{
    if (_proc)
        _serviceRegistry.releaseProcedure(_proc);
}

void Responder::reply(IOStream& out)
{
    log_info("send reply");

    out << '\xc1';
    _formatter.begin(out.buffer());
    _result->format(_formatter);
    _formatter.finish();
    out << '\xff';
}

void Responder::replyError(IOStream& out, const char* msg, int rc)
{
    log_info("send error \"" << msg << '"');

    out << '\xc2'
        << static_cast<char>(static_cast<uint32_t>(rc) >> 24)
        << static_cast<char>(static_cast<uint32_t>(rc) >> 16)
        << static_cast<char>(static_cast<uint32_t>(rc) >> 8)
        << static_cast<char>(static_cast<uint32_t>(rc))
        << msg
        << '\0' << '\xff';
}

bool Responder::onInput(IOStream& ios)
{
    while (ios.buffer().in_avail() > 0)
    {
        if (advance(ios.buffer()))
        {
            if (_failed)
            {
                replyError(ios, _errorMessage.c_str(), 0);
            }
            else
            {
                try
                {
                    _result = _proc->endCall();
                    reply(ios);
                }
                catch (const RemoteException& e)
                {
                    ios.buffer().discard();
                    replyError(ios, e.what(), e.rc());
                }
                catch (const std::exception& e)
                {
                    ios.buffer().discard();
                    replyError(ios, e.what(), 0);
                }
            }

            _serviceRegistry.releaseProcedure(_proc);
            _proc = 0;
            _args = 0;
            _result = 0;
            _state = State::header;
            _failed = false;
            _errorMessage.clear();
            _deserializer.begin();
            _headerParser.reset();

            return true;
        }
    }

    return false;
}

bool Responder::advance(std::streambuf& in)
{
    std::streambuf::int_type chi;
    while ((chi = in.sgetc()) != std::streambuf::traits_type::eof())
    {
        char ch = std::streambuf::traits_type::to_char_type(chi);
        switch (_state)
        {
            case State::header:
                if (_headerParser.advance(in))
                {
                    log_info("rpc method \"" << _headerParser.method() << '"');

                    _proc = _serviceRegistry.getProcedure(
                            _headerParser.domain().empty()
                                ? _headerParser.method()
                                : _headerParser.domain() + '\0' + _headerParser.method());

                    if (_proc)
                    {
                        _args = _proc->beginCall();
                        _state = State::params;
                    }
                    else
                    {
                        _failed = true;
                        _errorMessage = "unknown method \"" + _headerParser.method() + '"';
                        _state = State::params_skip;
                    }
                }
                break;

            case State::params:
                if (ch == '\xff')
                {
                    if (_args && _args->needMore())
                    {
                        _failed = true;
                        _errorMessage = "argument expected";
                    }

                    in.sbumpc();
                    return true;
                }
                else
                {
                    if (_args == 0 || !_args->needMore())
                    {
                        _failed = true;
                        _errorMessage = "too many arguments";
                        _state = State::params_skip;
                    }
                    else
                    {
                        _deserializer.begin(false);
                        _state = State::param;
                    }
                }
                break;

            case State::params_skip:
                if (ch == '\xff')
                {
                    in.sbumpc();
                    return true;
                }
                else
                {
                    _deserializer.skip();
                    _state = State::param_skip;
                }

                break;

            case State::param:
                if (_deserializer.advance(in))
                {
                    try
                    {
                        _args->get()->fixup(_deserializer.si());
                        _state = State::params;
                    }
                    catch (const std::exception& e)
                    {
                        _failed = true;
                        _errorMessage = e.what();
                        _state = State::params_skip;
                    }
                }
                break;

            case State::param_skip:
                if (_deserializer.advance(in))
                    _state = State::params_skip;

                break;
        }
    }

    return false;
}

}
}
