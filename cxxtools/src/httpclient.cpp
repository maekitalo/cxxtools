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

#include <cxxtools/httpclient.h>
#include <cxxtools/httpparser.h>
#include <cxxtools/ioerror.h>
#include <cxxtools/log.h>

log_define("cxxtools.net.httpclient")

namespace cxxtools {

namespace net {

void HttpClient::ParseEvent::onHttpReturn(unsigned ret, const std::string& text)
{
    _replyHeader.httpReturn(ret, text);
}

HttpClient::HttpClient(const std::string& server, unsigned short int port)
: _parseEvent(_replyHeader)
, _parser(_parseEvent, true)
, _request(0)
, _server(server)
, _port(port)
, _readHeader(true)
, _contentLength(0)
{
    _stream.attachDevice(_socket);
    connect(_socket.connected, *this, &HttpClient::onConnect);
    connect(_stream.buffer().outputReady, *this, &HttpClient::onOutput);
    connect(_stream.buffer().inputReady, *this, &HttpClient::onInput);
}


HttpClient::HttpClient(const std::string& server, unsigned short int port,
    SelectorBase& selector)
: _parseEvent(_replyHeader)
, _parser(_parseEvent, true)
, _request(0)
, _server(server)
, _port(port)
, _readHeader(true)
, _contentLength(0)
{
    _stream.attachDevice(_socket);
    connect(_socket.connected, *this, &HttpClient::onConnect);
    connect(_stream.buffer().outputReady, *this, &HttpClient::onOutput);
    connect(_stream.buffer().inputReady, *this, &HttpClient::onInput);
    setSelector(selector);
}


void HttpClient::setSelector(SelectorBase& selector)
{
    selector.add(_socket);
}

const HttpReplyHeader& HttpClient::execute(const HttpRequest& request, std::size_t timeout)
{
    bool connected = _socket.isConnected();
    if (!connected)
    {
        log_debug("connect");
        _socket.connect(_server, _port);
    }

    sendRequest(request);
    _stream.flush();

    if (!_stream && !connected)
    {
        // sending failed and we were not connected before, so try now
        log_debug("connect");
        _socket.connect(_server, _port);

        _stream.clear();
        _stream.buffer().discard();
        sendRequest(request);
        _stream.flush();
    }

    if (!_stream)
        throw IOError( CXXTOOLS_ERROR_MSG("error sending HTTP request") );

    _parser.reset(true);

    log_debug("read reply");

    char ch;
    while (!_parser.end() && _stream.get(ch))
    {
        _parser.parse(ch);
    }

    log_debug("reply ready");

    if (_parser.fail())
        throw IOError( CXXTOOLS_ERROR_MSG("invalid HTTP reply") );

    return _replyHeader;
}


void HttpClient::readBody(std::string& s)
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

    if (!_replyHeader.keepAlive())
        _socket.close();
}


std::string HttpClient::get(const std::string& url)
{
    HttpRequest request(url);
    execute(request);
    return readBody();
}


void HttpClient::beginExecute(const HttpRequest& request)
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
            // first write failed, so connection is not active any more

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


void HttpClient::wait(std::size_t msecs)
{
    _socket.wait(msecs);
}


void HttpClient::sendRequest(const HttpRequest& request)
{
    log_debug("send request " << request.url());

    const std::string contentLength = "Content-Length";
    const std::string server = "Server";
    const std::string connection = "Connection";
    const std::string date = "Date";

    _stream << request.method() << ' '
            << request.url() << " HTTP/"
            << request.header().httpVersionMajor() << '.'
            << request.header().httpVersionMinor() << "\r\n";

    for (HttpRequestHeader::const_iterator it = request.header().begin();
        it != request.header().end(); ++it)
    {
        _stream << it->first << ": " << it->second << "\r\n";
    }

   if (!request.header().hasHeader(contentLength))
    {
        _stream << "Content-Length: " << request.bodySize() << "\r\n";
    }

    if (!request.header().hasHeader(server))
    {
        _stream << "Server: cxxtools-Net-HttpServer\r\n";
    }

    if (!request.header().hasHeader(connection))
    {
        _stream << "Connection: keep-alive\r\n";
    }

    if (!request.header().hasHeader(date))
    {
        _stream << "Date: " << HttpMessageHeader::htdateCurrent() << "\r\n";
    }

    _stream << "\r\n";

    log_debug("send body");

    request.sendBody(_stream);

}

void HttpClient::onConnect(TcpSocket& socket)
{
    socket.endConnect();
    sendRequest(*_request);
    _stream.buffer().beginWrite();
}

void HttpClient::onOutput(StreamBuffer& sb)
{
    if( sb.out_avail() > 0 )
    {
        sb.beginWrite();
    }
    else
    {
        sb.beginRead();
        requestSent(*this);
    }
}


void HttpClient::onInput(StreamBuffer& sb)
{
    try
    {
        if (_readHeader)
        {
            _parser.advance(sb);

            if (_parser.fail())
                throw std::runtime_error("http parser failed"); // TODO define exception class

            if( _parser.end() )
            {
                _contentLength = _replyHeader.contentLength();
                headerReceived(*this);
                _readHeader = false;

                if (_contentLength > 0)
                {
                    if( sb.in_avail() > 0 )
                    {
                        processBodyAvailable();
                    }
                }
                else
                {
                    replyFinished(*this);
                }
            }
        }
        else
        {
            processBodyAvailable();
        }
    }
    catch (const std::exception& e)
    {
        _socket.close();

        // TODO propagate exception if signal errorOccured is not connected
        errorOccured(*this, e);
    }
}

void HttpClient::processBodyAvailable()
{
    _contentLength -= bodyAvailable(*this); // TODO: may throw exception
    if( _contentLength <= 0 )
        replyFinished(*this);
}

} // namespace net

} // namespace cxxtools
