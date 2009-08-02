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

#include <cxxtools/http/client.h>
#include <cxxtools/http/parser.h>
#include <cxxtools/ioerror.h>
#include <cxxtools/base64stream.h>
#include <sstream>

#include <cxxtools/log.h>

log_define("cxxtools.http.client")

namespace cxxtools {

namespace http {

void Client::ParseEvent::onHttpReturn(unsigned ret, const std::string& text)
{
    _replyHeader.httpReturn(ret, text);
}

Client::Client()
: _parseEvent(_replyHeader)
, _parser(_parseEvent, true)
, _request(0)
, _server(std::string())
, _port(0)
, _stream(8192, true)
, _readHeader(true)
, _contentLength(0)
{
    _stream.attachDevice(_socket);
    cxxtools::connect(_socket.connected, *this, &Client::onConnect);
    cxxtools::connect(_stream.buffer().outputReady, *this, &Client::onOutput);
    cxxtools::connect(_stream.buffer().inputReady, *this, &Client::onInput);
}


Client::Client(const std::string& server, unsigned short int port)
: _parseEvent(_replyHeader)
, _parser(_parseEvent, true)
, _request(0)
, _server(server)
, _port(port)
, _stream(8192, true)
, _readHeader(true)
, _contentLength(0)
{
    _stream.attachDevice(_socket);
    cxxtools::connect(_socket.connected, *this, &Client::onConnect);
    cxxtools::connect(_stream.buffer().outputReady, *this, &Client::onOutput);
    cxxtools::connect(_stream.buffer().inputReady, *this, &Client::onInput);
}


Client::Client(SelectorBase& selector, const std::string& server, unsigned short int port)
: _parseEvent(_replyHeader)
, _parser(_parseEvent, true)
, _request(0)
, _server(server)
, _port(port)
, _stream(8192, true)
, _readHeader(true)
, _contentLength(0)
{
    _stream.attachDevice(_socket);
    cxxtools::connect(_socket.connected, *this, &Client::onConnect);
    cxxtools::connect(_stream.buffer().outputReady, *this, &Client::onOutput);
    cxxtools::connect(_stream.buffer().inputReady, *this, &Client::onInput);
    setSelector(selector);
}


void Client::setSelector(SelectorBase& selector)
{
    selector.add(_socket);
}

void Client::reexecute(const Request& request)
{
    log_debug("reconnect");
    _socket.connect(_server, _port);

    _stream.clear();
    _stream.buffer().discard();
    sendRequest(request);
    _stream.flush();
}

void Client::doparse()
{
    char ch;
    while (!_parser.end() && _stream.get(ch))
        _parser.parse(ch);

}

const ReplyHeader& Client::execute(const Request& request, std::size_t timeout)
{
    _replyHeader.clear();

    _socket.setTimeout(timeout);

    bool shouldReconnect = _socket.isConnected();
    if (!shouldReconnect)
    {
        log_debug("connect");
        _socket.connect(_server, _port);
    }

    sendRequest(request);
    _stream.flush();

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


void Client::readBody(std::string& s)
{
    unsigned n = _replyHeader.contentLength();

    log_debug("read body; content-size: " << n);

    s.clear();
    s.reserve(n);

    char ch;
    while (n-- && _stream.get(ch))
        s += ch;

    if (_stream.fail())
        throw IOError( CXXTOOLS_ERROR_MSG("error reading HTTP reply body") );

    //log_debug("body read: \"" << s << '"');

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


std::string Client::get(const std::string& url, std::size_t timeout)
{
    Request request(url);
    execute(request, timeout);
    return readBody();
}


void Client::beginExecute(const Request& request)
{
    _request = &request;
    _replyHeader.clear();
    if (_socket.isConnected())
    {
        sendRequest(*_request);
        try
        {
            _stream.buffer().beginWrite();
        }

        catch (const cxxtools::IOError&)
        {
            log_debug("first write failed, so connection is not active any more");

            _stream.clear();
            _stream.buffer().discard();
            _socket.beginConnect(_server, _port);
        }
    }
    else
    {
        _socket.beginConnect(_server, _port);
    }
}


void Client::wait(std::size_t msecs)
{
    _socket.wait(msecs);
}


void Client::sendRequest(const Request& request)
{
    log_debug("send request " << request.url());

    const std::string contentLength = "Content-Length";
    const std::string connection = "Connection";
    const std::string date = "Date";
    const std::string host = "Host";
    const std::string authorization = "Authorization";

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
        _stream << "Host: " << _server;
        if (_port != 80)
            _stream << ':' << _port;
        _stream << "\r\n";
    }

    if (!_username.empty() && !request.header().hasHeader(authorization))
    {
        std::ostringstream d;
        Base64ostream b(d);
        b << _username
          << ':'
          << _password;
        b.end();
        log_debug("set Authorization to " << d.str());
        _stream << "Authorization: Basic " << d.str() << "\r\n";
    }

    _stream << "\r\n";

    log_debug("send body");

    request.sendBody(_stream);
}

void Client::onConnect(net::TcpSocket& socket)
{
    try
    {
        socket.endConnect();
        sendRequest(*_request);
        _stream.buffer().beginWrite();
    }
    catch (const std::exception& e)
    {
        errorOccured(*this, e);
    }
}

void Client::onOutput(StreamBuffer& sb)
{
    log_trace("Client::onOutput; out_avail=" << sb.out_avail());

    if( sb.out_avail() > 0 )
    {
        sb.beginWrite();
    }
    else
    {
        sb.beginRead();
        requestSent(*this);
        _parser.reset(true);
        _readHeader = true;
    }
}


void Client::onInput(StreamBuffer& sb)
{
    try
    {
        log_trace("Client::onInput; readHeader=" << _readHeader);

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

        // TODO propagate exception if signal errorOccured is not connected
        errorOccured(*this, e);
    }
}

void Client::processHeaderAvailable(StreamBuffer& sb)
{
    _parser.advance(sb);

    if (_parser.fail())
        throw std::runtime_error("http parser failed"); // TODO define exception class

    if( _parser.end() )
    {
        _contentLength = _replyHeader.contentLength();
        log_debug("header received - content-length=" << _contentLength);

        headerReceived(*this);
        _readHeader = false;

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
            replyFinished(*this);
        }
    }
    else
    {
        sb.beginRead();
    }
}

void Client::processBodyAvailable(StreamBuffer& sb)
{
    log_trace("processBodyAvailable");

    log_debug("content-length(pre)=" << _contentLength);

    _contentLength -= bodyAvailable(*this); // TODO: may throw exception

    log_debug("content-length(post)=" << _contentLength);

    if( _contentLength <= 0 )
    {
        log_debug("reply finished");
        replyFinished(*this);
    }
    else
    {
        sb.beginRead();
    }
}

} // namespace http

} // namespace cxxtools
