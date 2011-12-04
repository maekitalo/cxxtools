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

#include "rpcclientimpl.h"
#include <cxxtools/log.h>
#include <cxxtools/remoteprocedure.h>
#include <cxxtools/bin/rpcclient.h>
#include <stdexcept>

log_define("cxxtools.bin.rpcclient.impl")

namespace cxxtools
{
namespace bin
{

RpcClientImpl::RpcClientImpl(RpcClient* client, SelectorBase& selector, const std::string& addr, unsigned short port)
    : _client(client),
      _proc(0),
      _stream(_socket, 8192, true),
      _formatter(_stream)
{
    setSelector(selector);
    connect(addr, port);

    cxxtools::connect(_socket.connected, *this, &RpcClientImpl::onConnect);
    cxxtools::connect(_stream.buffer().outputReady, *this, &RpcClientImpl::onOutput);
    cxxtools::connect(_stream.buffer().inputReady, *this, &RpcClientImpl::onInput);

}

RpcClientImpl::RpcClientImpl(RpcClient* client, const std::string& addr, unsigned short port)
    : _client(client),
      _proc(0),
      _stream(_socket, 8192, true),
      _formatter(_stream)
{
    connect(addr, port);

    cxxtools::connect(_socket.connected, *this, &RpcClientImpl::onConnect);
    cxxtools::connect(_stream.buffer().outputReady, *this, &RpcClientImpl::onOutput);
    cxxtools::connect(_stream.buffer().inputReady, *this, &RpcClientImpl::onInput);

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

void RpcClientImpl::beginCall(IComposer& r, IRemoteProcedure& method, IDecomposer** argv, unsigned argc)
{
    if (_socket.selector() == 0)
        throw std::logic_error("cannot run async rpc request without a selector");

    _proc = &method;

    prepareRequest(method.name(), argv, argc);

    if (_socket.isConnected())
    {
        try
        {
            _stream.buffer().beginWrite();
        }
        catch (const IOError&)
        {
            log_debug("write failed, connection is not active any more");
            _socket.beginConnect(_addr, _port);
        }
    }
    else
    {
        log_debug("not yet connected - do it now");
        _socket.beginConnect(_addr, _port);
    }

    _scanner.begin(&r);
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

void RpcClientImpl::call(IComposer& r, IRemoteProcedure& method, IDecomposer** argv, unsigned argc)
{
    _proc = &method;

    prepareRequest(_proc->name(), argv, argc);

    if (!_socket.isConnected())
        _socket.connect(_addr, _port);

    try
    {
        _stream.flush();

        _scanner.begin(&r);

        char ch;
        while (_stream.get(ch))
        {
            if (_scanner.advance(ch))
            {
                _scanner.checkException();
                break;
            }
        }
    }
    catch (const std::exception& e)
    {
        cancel();
        throw;
    }

    _proc = 0;

    if (!_stream)
    {
        cancel();
        throw std::runtime_error("reading result failed");
    }
}

void RpcClientImpl::cancel()
{
    _socket.close();
    _stream.clear();
    _stream.buffer().discard();
    _proc = 0;
}

void RpcClientImpl::prepareRequest(const String& name, IDecomposer** argv, unsigned argc)
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
        IRemoteProcedure* proc = _proc;
        cancel();

        if (!proc)
            throw;

        _exceptionPending = true;
        proc->onFinished();

        if (_exceptionPending)
            throw;
    }
}

void RpcClientImpl::onOutput(StreamBuffer& sb)
{
    try
    {
        _exceptionPending = false;
        sb.endWrite();
        if (sb.out_avail() > 0)
            sb.beginWrite();
        else
            sb.beginRead();
    }
    catch (const std::exception&)
    {
        IRemoteProcedure* proc = _proc;
        cancel();

        if (!proc)
            throw;

        _exceptionPending = true;
        proc->onFinished();

        if (_exceptionPending)
            throw;
    }
}

void RpcClientImpl::onInput(StreamBuffer& sb)
{
    try
    {
        _exceptionPending = false;
        sb.endRead();

        char ch;
        while (_stream.buffer().in_avail() && _stream.get(ch))
        {
            if (_scanner.advance(ch))
            {
                _scanner.checkException();
                IRemoteProcedure* proc = _proc;
                _proc = 0;
                proc->onFinished();
                return;
            }
        }

        if (!_stream)
        {
            close();
            throw std::runtime_error("reading result failed");
        }

        sb.beginRead();
    }
    catch (const std::exception&)
    {
        IRemoteProcedure* proc = _proc;
        cancel();

        if (!proc)
            throw;

        _exceptionPending = true;
        proc->onFinished();

        if (_exceptionPending)
            throw;
    }
}

}
}
