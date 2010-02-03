/*
 * Copyright (C) 2009 by Marc Boris Duerner, Tommi Maekitalo
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

#include "clientimpl.h"
#include <cxxtools/http/client.h>
#include "parser.h"
#include <cxxtools/ioerror.h>
#include <cxxtools/textstream.h>
#include <cxxtools/base64codec.h>
#include <sstream>

#include <cxxtools/log.h>

log_define("cxxtools.http.client.impl")

namespace cxxtools {

namespace http {

void ClientImpl::ParseEvent::onHttpReturn(unsigned ret, const std::string& text)
{
    _replyHeader.httpReturn(ret, text);
}

ClientImpl::ClientImpl(Client* client)
: _client(client)
, _parseEvent(_replyHeader)
, _parser(_parseEvent, true)
, _request(0)
, _stream(8192, true)
, _chunkedIStream(_stream.rdbuf())
, _contentLength(0)
, _readHeader(true)
, _chunkedEncoding(false)
, _reconnectOnError(false)
{
    _stream.attachDevice(_socket);
    _stream.exceptions(std::ios::failbit | std::ios::badbit);     
    _chunkedIStream.exceptions(std::ios::failbit | std::ios::badbit);
    cxxtools::connect(_socket.connected, *this, &ClientImpl::onConnect);
    cxxtools::connect(_socket.errorOccured, *this, &ClientImpl::onErrorOccured);
    cxxtools::connect(_stream.buffer().outputReady, *this, &ClientImpl::onOutput);
    cxxtools::connect(_stream.buffer().inputReady, *this, &ClientImpl::onInput);
}


ClientImpl::ClientImpl(Client* client, const net::AddrInfo& addrinfo)
: _client(client)
, _parseEvent(_replyHeader)
, _parser(_parseEvent, true)
, _request(0)
, _addrInfo(addrinfo)
, _stream(8192, true)
, _chunkedIStream(_stream.rdbuf())
, _contentLength(0)
, _readHeader(true)
, _chunkedEncoding(false)
, _reconnectOnError(false)
{
    _stream.attachDevice(_socket);
    _stream.exceptions(std::ios::failbit | std::ios::badbit);                     
    _chunkedIStream.exceptions(std::ios::failbit | std::ios::badbit);             
    cxxtools::connect(_socket.connected, *this, &ClientImpl::onConnect);
    cxxtools::connect(_socket.errorOccured, *this, &ClientImpl::onErrorOccured);
    cxxtools::connect(_stream.buffer().outputReady, *this, &ClientImpl::onOutput);
    cxxtools::connect(_stream.buffer().inputReady, *this, &ClientImpl::onInput);
}

ClientImpl::ClientImpl(Client* client, SelectorBase& selector, const net::AddrInfo& addrinfo)
: _client(client)
, _parseEvent(_replyHeader)
, _parser(_parseEvent, true)
, _request(0)
, _addrInfo(addrinfo)
, _stream(8192, true)
, _chunkedIStream(_stream.rdbuf())
, _contentLength(0)
, _readHeader(true)
, _chunkedEncoding(false)
, _reconnectOnError(false)
{
    _stream.attachDevice(_socket);
    _stream.exceptions(std::ios::failbit | std::ios::badbit);                     
    _chunkedIStream.exceptions(std::ios::failbit | std::ios::badbit);             
    cxxtools::connect(_socket.connected, *this, &ClientImpl::onConnect);
    cxxtools::connect(_socket.errorOccured, *this, &ClientImpl::onErrorOccured);
    cxxtools::connect(_stream.buffer().outputReady, *this, &ClientImpl::onOutput);
    cxxtools::connect(_stream.buffer().inputReady, *this, &ClientImpl::onInput);
    setSelector(selector);
}


void ClientImpl::setSelector(SelectorBase& selector)
{
    selector.add(_socket);
}

void ClientImpl::reexecute(const Request& request)
{
    log_debug("reexecute");

    _stream.clear();
    _stream.buffer().discard();

    _socket.connect(_addrInfo);

    sendRequest(request);
    _stream.flush();
}

void ClientImpl::doparse()
{
    char ch;
    while (!_parser.end() && _stream.get(ch))
        _parser.parse(ch);

}

const ReplyHeader& ClientImpl::execute(const Request& request, std::size_t timeout)
{
    log_trace("execute request " << request.url());

    _replyHeader.clear();

    _socket.setTimeout(timeout);

    bool shouldReconnect = _socket.isConnected();
    if (!shouldReconnect)
    {
        log_debug("connect");
        _socket.connect(_addrInfo);
    }

    log_debug("send request");
    sendRequest(request);
    log_debug("flush stream");
    _stream.flush();

    log_debug("flush stream ready");
    if (!_stream && shouldReconnect)
    {
        // sending failed and we were not connected before, so try again
        reexecute(request);
        shouldReconnect = false;
    }

    if (!_stream)
        throw IOError( CXXTOOLS_ERROR_MSG("error sending HTTP request") );

    log_debug("read reply");

    _parser.reset(true);
    _readHeader = true;
    doparse();

    if (_parser.begin() && shouldReconnect)
    {
        // reading failed and we were not connected before, so try again
        reexecute(request);

        if (!_stream)
            throw IOError( CXXTOOLS_ERROR_MSG("error sending HTTP request") );

        doparse();
    }

    log_debug("reply ready");

    if (_stream.fail())
        throw IOError( CXXTOOLS_ERROR_MSG("failed to read HTTP reply") );

    if (_parser.fail())
        throw IOError( CXXTOOLS_ERROR_MSG("invalid HTTP reply") );

    if (!_parser.end())
        throw IOError( CXXTOOLS_ERROR_MSG("incomplete HTTP reply header") );

    return _replyHeader;
}


void ClientImpl::readBody(std::string& s)
{
    s.clear();

    _chunkedEncoding = _replyHeader.chunkedTransferEncoding();
    _chunkedIStream.reset();

    if (_chunkedEncoding)
    {
        log_debug("read body with chunked encoding");

        char ch;
        while (_chunkedIStream.get(ch))
            s += ch;

        log_debug("eod=" << _chunkedIStream.eod());

        if (_chunkedIStream.eod())
        {
            _parser.readHeader();
            doparse();

            if (_parser.fail())
                throw std::runtime_error("http parser failed"); // TODO define exception class
        }
        else
            throw std::runtime_error("chunked stream not complete");
    }
    else
    {
        unsigned n = _replyHeader.contentLength();

        log_debug("read body; content-size: " << n);

        s.reserve(n);

        char ch;
        while (n-- && _stream.get(ch))
            s += ch;

        if (_stream.fail())
            throw IOError( CXXTOOLS_ERROR_MSG("error reading HTTP reply body") );

        //log_debug("body read: \"" << s << '"');
    }

    if (!_replyHeader.keepAlive())
    {
        log_debug("close socket - no keep alive");
        _socket.close();
    }
    else
    {
        log_debug("do not close socket - keep alive");
    }
}


std::string ClientImpl::get(const std::string& url, std::size_t timeout)
{
    Request request(url);
    execute(request, timeout);
    return readBody();
}


void ClientImpl::beginExecute(const Request& request)
{
    log_trace("beginExecute");

    _request = &request;
    _replyHeader.clear();
    if (_socket.isConnected())
    {
        log_debug("we are connected already");
        sendRequest(*_request);
        try
        {
            _stream.buffer().beginWrite();
            _reconnectOnError = true;
        }
        catch (const cxxtools::IOError&)
        {
            log_debug("first write failed, so connection is not active any more");

            _stream.clear();
            _stream.buffer().discard();
            _socket.beginConnect(_addrInfo);
            _reconnectOnError = false;
        }
    }
    else
    {
        log_debug("not yet connected - do it now");
        _socket.beginConnect(_addrInfo);
        _reconnectOnError = false;
    }
}


void ClientImpl::wait(std::size_t msecs)
{
    _socket.wait(msecs);
}


void ClientImpl::sendRequest(const Request& request)
{
    log_debug("send request " << request.url());

    static const std::string contentLength = "Content-Length";
    static const std::string connection = "Connection";
    static const std::string date = "Date";
    static const std::string host = "Host";
    static const std::string authorization = "Authorization";

    _stream << request.method() << ' '
            << request.url() << " HTTP/"
            << request.header().httpVersionMajor() << '.'
            << request.header().httpVersionMinor() << "\r\n";

    for (RequestHeader::const_iterator it = request.header().begin();
        it != request.header().end(); ++it)
    {
        _stream << it->first << ": " << it->second << "\r\n";
    }

   if (!request.header().hasHeader(contentLength))
    {
        _stream << "Content-Length: " << request.bodySize() << "\r\n";
    }

    if (!request.header().hasHeader(connection))
    {
        _stream << "Connection: keep-alive\r\n";
    }

    if (!request.header().hasHeader(date))
    {
        _stream << "Date: " << MessageHeader::htdateCurrent() << "\r\n";
    }

    if (!request.header().hasHeader(host))
    {
        _stream << "Host: " << _addrInfo.host();
        unsigned short port = _addrInfo.port();
        if (port != 80)
            _stream << ':' << port;
        _stream << "\r\n";
    }

    if (!_username.empty() && !request.header().hasHeader(authorization))
    {
        std::ostringstream d;
        BasicTextOStream<char, char> b(d, new Base64Codec());
        b << _username
          << ':'
          << _password;
        b.terminate();
        log_debug("set Authorization to " << d.str());
        _stream << "Authorization: Basic " << d.str() << "\r\n";
    }

    _stream << "\r\n";

    log_debug("send body; " << request.bodySize() << " bytes");

    request.sendBody(_stream);
}

void ClientImpl::onConnect(net::TcpSocket& socket)
{
    try
    {
        log_trace("onConnect");
        socket.endConnect();
        sendRequest(*_request);

        log_debug("request sent - begin write");
        _stream.buffer().beginWrite();
    }
    catch (const std::exception& e)
    {
        log_debug("error occured");

        _socket.close();

        if (_reconnectOnError && _request != 0)
        {
            log_debug("reconnect on error");
            _reconnectOnError = false;
            reexecute(*_request);
        }
        else
        {
            if (_client->errorOccured.connectionCount() == 0)
                throw;

            _client->errorOccured(*_client, e);
        }
    }
}

void ClientImpl::onErrorOccured(IODevice& socket)
{
    log_trace("error occured");
    try
    {
        if (_reconnectOnError && _request != 0)
        {
            log_debug("reconnect on error");
            _reconnectOnError = false;
            reexecute(*_request);
        }
        else
        {
            throw IOError( CXXTOOLS_ERROR_MSG("error occured in i/o-device") );
        }
    }
    catch (const std::exception& e)
    {
        _socket.close();

        if (_client->errorOccured.connectionCount() == 0)
            throw;

        _client->errorOccured(*_client, e);
    }
}

void ClientImpl::onOutput(StreamBuffer& sb)
{
    log_trace("ClientImpl::onOutput; out_avail=" << sb.out_avail());

    if( sb.out_avail() > 0 )
    {
        sb.beginWrite();
    }
    else
    {
        sb.beginRead();
        _client->requestSent(*_client);
        _parser.reset(true);
        _readHeader = true;
    }
}


void ClientImpl::onInput(StreamBuffer& sb)
{
    try
    {
        log_trace("ClientImpl::onInput; readHeader=" << _readHeader);

        if (_readHeader)
        {
            processHeaderAvailable(sb);
        }
        else
        {
            processBodyAvailable(sb);
        }
    }
    catch (const std::exception& e)
    {
        log_warn("error of type " << typeid(e).name() << " occured: " << e.what());

        _socket.close();

        if (_client->errorOccured.connectionCount() == 0)
            throw;

        _client->errorOccured(*_client, e);
    }
}

void ClientImpl::processHeaderAvailable(StreamBuffer& sb)
{
    _parser.advance(sb);

    if (_parser.fail())
        throw std::runtime_error("http parser failed"); // TODO define exception class

    if( _parser.end() )
    {
        _chunkedEncoding = _replyHeader.chunkedTransferEncoding();

        _client->headerReceived(*_client);
        _readHeader = false;

        if (_chunkedEncoding)
        {
            log_debug("chunked transfer encoding used");

            _chunkedIStream.reset();

            if( sb.in_avail() > 0 )
            {
                processBodyAvailable(sb);
            }
            else
            {
                sb.beginRead();
            }
        }
        else
        {
            _contentLength = _replyHeader.contentLength();
            log_debug("header received - content-length=" << _contentLength);

            if (_contentLength > 0)
            {
                if( sb.in_avail() > 0 )
                {
                    processBodyAvailable(sb);
                }
                else
                {
                    sb.beginRead();
                }
            }
            else
            {
                if (!_replyHeader.keepAlive())
                {
                    log_debug("close socket - no keep alive");
                    _socket.close();
                }

                _client->replyFinished(*_client);
            }
        }
    }
    else
    {
        sb.beginRead();
    }
}

void ClientImpl::processBodyAvailable(StreamBuffer& sb)
{
    log_trace("processBodyAvailable");

    if (_chunkedEncoding)
    {
        if (_chunkedIStream.rdbuf()->in_avail() > 0)
        {
            if (!_chunkedIStream.eod())
            {
                log_debug("read chunked encoding body");

                while (_chunkedIStream.good()
                    && _chunkedIStream.rdbuf()->in_avail() > 0
                    && !_chunkedIStream.eod())
                {
                    log_debug("bodyAvailable");
                    _client->bodyAvailable(*_client);
                }

                log_debug("in_avail=" << _chunkedIStream.rdbuf()->in_avail() << " eod=" << _chunkedIStream.eod());
                if (_chunkedIStream.eod())
                {
                    _parser.readHeader();
                }
            }

            if (_chunkedIStream.eod() && sb.in_avail() > 0)
            {
                log_debug("read chunked encoding post headers");

                _parser.advance(sb);
                if (_parser.fail())
                    throw std::runtime_error("http parser failed"); // TODO define exception class

                if( _parser.end() )
                {
                    log_debug("reply finished");

                    if (!_replyHeader.keepAlive())
                    {
                        log_debug("close socket - no keep alive");
                        _socket.close();
                    }

                    _client->replyFinished(*_client);
                }
            }
        }

        if (!_chunkedIStream.eod() || !_parser.end())
        {
            log_debug("call beginRead");
            sb.beginRead();
        }
    }
    else
    {
        log_debug("content-length(pre)=" << _contentLength);

        while (_stream.good() && _contentLength > 0 && sb.in_avail() > 0)                                            
        {
            _contentLength -= _client->bodyAvailable(*_client); // TODO: may throw exception
            log_debug("content-length(post)=" << _contentLength);
        }

        if( _contentLength <= 0 )
        {
            log_debug("reply finished");

            if (!_replyHeader.keepAlive())
            {
                log_debug("close socket - no keep alive");
                _socket.close();
            }

            _client->replyFinished(*_client);
        }
        else
        {
            sb.beginRead();
        }
    }
}

void ClientImpl::cancel()
{
    _socket.close();
    _stream.clear();
    _stream.buffer().discard();
}


} // namespace http

} // namespace cxxtools
