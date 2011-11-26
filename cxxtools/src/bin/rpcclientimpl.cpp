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

#include "rpcclientimpl.h"
#include <cxxtools/log.h>
#include <cxxtools/remoteprocedure.h>
#include <cxxtools/bin/rpcclient.h>

log_define("cxxtools.bin.rpcclient.impl")

namespace cxxtools
{
namespace bin
{
RpcClientImpl::RpcClientImpl(RpcClient* client, SelectorBase* selector, const std::string& addr, unsigned short port)
    : _client(client),
      _proc(0),
      _stream(_socket, 8192, true),
      _formatter(_stream)
{
    setSelector(selector);
    connect(addr, port);
}

RpcClientImpl::~RpcClientImpl()
{
}

void RpcClientImpl::connect(const std::string& addr, unsigned short port)
{
    _addr = addr;
    _port = port;
}

void RpcClientImpl::close()
{
    _socket.close();
}

void RpcClientImpl::beginCall(IDeserializer& r, IRemoteProcedure& method, ISerializer** argv, unsigned argc)
{
    _proc = &method;

    prepareRequest(method.name(), argv, argc);

    if (_socket.isConnected())
    {
        try
        {
            _stream.buffer().beginWrite();
            _reconnectOnError = true;
        }
        catch (const IOError&)
        {
            log_debug("first write failed, connection is not active any more");
            _stream.clear();
            _stream.buffer().discard();
            _socket.beginConnect(_addr, _port);
            _reconnectOnError = true;
        }
    }
    else
    {
        log_debug("not yet connected - do it now");
        _socket.beginConnect(_addr, _port);
        _reconnectOnError = false;
    }

}

void RpcClientImpl::endCall()
{
    _proc = 0;

    if (_exceptionPending)
    {
        _exceptionPending = false;
        throw;
    }
}

void RpcClientImpl::call(IDeserializer& r, IRemoteProcedure& method, ISerializer** argv, unsigned argc)
{
    _proc = &method;

    prepareRequest(_proc->name(), argv, argc);

    if (!_socket.isConnected())
        _socket.connect(_addr, _port);

    _stream.flush();

    _scanner.begin(&r);

    char ch;
    while (_stream.get(ch))
    {
        if (_scanner.advance(ch))
            break;
    }

    _proc = 0;

    if (!_stream)
        throw std::runtime_error("reading result failed");
}

void RpcClientImpl::cancel()
{
    _socket.close();
    _stream.clear();
    _stream.buffer().discard();
}

void RpcClientImpl::prepareRequest(const String& name, ISerializer** argv, unsigned argc)
{
    _stream << '\x40' << name << '\0';

    for(unsigned n = 0; n < argc; ++n)
    {
        argv[n]->format(_formatter);
    }

    _stream << '\xff';
}

void RpcClientImpl::onConnect(net::TcpSocket& socket)
{
    try
    {
        log_trace("onConnect");

        _exceptionPending = false;
        socket.endConnect();

        _stream.buffer().beginWrite();
    }
    catch (const std::exception& )
    {
        _exceptionPending = true;
        _proc->onFinished();

        if (_exceptionPending)
            throw;
    }
}

}
}
