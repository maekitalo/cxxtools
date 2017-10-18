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
#include <cxxtools/net/uri.h>
#include "parser.h"
#include <cxxtools/ioerror.h>
#include <cxxtools/textstream.h>
#include <cxxtools/base64codec.h>
#include <sstream>
#include <algorithm>
#include "config.h"

#include <cxxtools/log.h>

log_define("cxxtools.http.client.impl")

namespace cxxtools {

namespace http {

void ClientImpl::ParseEvent::onHttpReturn(unsigned ret, const std::string& text)
{
    _replyHeader.httpReturn(ret, text);
}

ClientImpl::ClientImpl(Client* client)
: _client(client),
  _request(0),
  _parseEvent(_reply.header()),
  _parser(_parseEvent, true),
#ifdef WITH_SSL
  _ssl(false),
#endif
  _stream(8192, true),
  _chunkedIStream(_stream.rdbuf()),
  _bodyStream(_stream.rdbuf()),
  _readHeader(true),
  _chunkedEncoding(false),
  _reconnectOnError(false),
  _errorPending(false)
{
    _stream.attachDevice(_socket);
    cxxtools::connect(_socket.connected, *this, &ClientImpl::onConnect);
#ifdef WITH_SSL
    cxxtools::connect(_socket.sslConnected, *this, &ClientImpl::onSslConnect);
#endif
    cxxtools::connect(_stream.buffer().outputReady, *this, &ClientImpl::onOutput);
    cxxtools::connect(_stream.buffer().inputReady, *this, &ClientImpl::onInput);
}


void ClientImpl::prepareConnect(const net::AddrInfo& addrinfo, const std::string& sslCertificate)
{
#ifdef WITH_SSL
    if (addrinfo != _addrInfo || sslCertificate != _sslCertificate || _ssl != _sslCertificate.empty())
    {
        _addrInfo = addrinfo;
        _sslCertificate = sslCertificate;
        _ssl = !sslCertificate.empty();
        _socket.close();
    }
#else
    if (addrinfo != _addrInfo)
    {
        _addrInfo = addrinfo;
        _socket.close();
    }
#endif
}

void ClientImpl::reexecute(const Request& request)
{
    log_debug("reexecute");

    _stream.clear();
    _stream.buffer().discard();

    _socket.connect(_addrInfo);
#ifdef WITH_SSL
    if (_ssl)
        _socket.sslConnect();
#endif

    sendRequest(request);
    _stream.flush();
}

void ClientImpl::reexecuteBegin(const Request& /*request*/)
{
    log_debug("reexecuteBegin");

    _stream.clear();
    _stream.buffer().discard();

    _socket.beginConnect(_addrInfo);
    _reconnectOnError = false;
}

void ClientImpl::doparse()
{
    char ch;
    while (!_parser.end() && _stream.get(ch))
        _parser.parse(ch);

}

const ReplyHeader& ClientImpl::execute(const Request& request, Timespan timeout, Timespan connectTimeout)
{
    log_trace("execute request " << request.url());

    if (_chunkedEncoding)
    {
        while (_chunkedIStream)
            _chunkedIStream.get();
    }
    else
    {
        while (_bodyStream)
            _bodyStream.get();
    }

    if (connectTimeout < Timespan(0))
        connectTimeout = timeout;

    _reply.clear();

    _socket.setTimeout(connectTimeout);

    bool shouldReconnect = _socket.isConnected();
    if (!shouldReconnect)
    {
        log_debug("connect");
        _socket.connect(_addrInfo);

#ifdef WITH_SSL
        if (_ssl)
        {
            log_debug("ssl connect");
            _socket.sslConnect();
        }
#endif
    }

    _socket.setTimeout(timeout);

    log_debug("send request");
    sendRequest(request);
    _stream.flush();

    if (!_stream && shouldReconnect)
    {
        // sending failed and we were not connected before, so try again
        reexecute(request);
        shouldReconnect = false;
    }

    if (!_stream)
        throw IOError("error sending HTTP request");

    log_debug("read reply");

    _parser.reset(true);
    _readHeader = true;
    doparse();

    if (_parser.begin() && shouldReconnect)
    {
        // reading failed and we were not connected before, so try again
        reexecute(request);

        if (!_stream)
            throw IOError("error sending HTTP request");

        doparse();
    }

    log_debug("reply ready");

    if (_stream.fail())
        throw IOError("failed to read HTTP reply");

    if (_parser.fail())
        throw IOError("invalid HTTP reply");

    if (!_parser.end())
        throw IOError("incomplete HTTP reply header");

    _chunkedEncoding = _reply.header().chunkedTransferEncoding();

    if (_chunkedEncoding)
    {
        _chunkedIStream.reset();
    }
    else
    {
        std::size_t n = _reply.header().contentLength();
        _bodyStream.clear();
        _bodyStream.icount(n);

        log_debug("content length " << n);

    }

    return _reply.header();
}


void ClientImpl::readBody()
{
    if (_chunkedEncoding)
    {
        log_debug("read body with chunked encoding");

        _reply.bodyStream() << _chunkedIStream.rdbuf();

        if (!_chunkedIStream.eod())
        {
            _chunkedIStream.setstate(std::ios::failbit);
            throw IOError("error reading HTTP reply body: incomplete chunked data stream");
        }
    }
    else if (_bodyStream.icount() > 0)
    {
        _reply.bodyStream() << _bodyStream.rdbuf();

        if (_bodyStream.icount() > 0 || !_reply.bodyStream())
        {
            _stream.setstate(std::ios::failbit);
            throw IOError("error reading HTTP reply body");
        }

    }

    if (!_reply.header().keepAlive())
    {
        log_debug("close socket - no keep alive");
        _socket.close();
    }
    else
    {
        log_debug("do not close socket - keep alive");
    }
}


void ClientImpl::beginExecute(const Request& request)
{
    if (_socket.selector() == 0)
        throw std::logic_error("cannot run async http request without a selector");

    log_trace("beginExecute");

    _errorPending = false;
    _request = &request;
    _reply.clear();
    if (_socket.isConnected())
    {
        log_debug("we are connected already");
        sendRequest(*_request);
        try
        {
            _stream.buffer().beginWrite();
            _reconnectOnError = true;
        }
        catch (const IOError&)
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


void ClientImpl::endExecute()
{
    if (_errorPending)
    {
        _errorPending = false;
        throw;
    }
}


bool ClientImpl::wait(std::size_t msecs)
{
    return _socket.wait(msecs);
}


SelectorBase* ClientImpl::selector()
{
    return _socket.selector();
}


void ClientImpl::sendRequest(const Request& request)
{
    log_debug("send request " << request.url());

    static const char* contentLength = "Content-Length";
    static const char* connection = "Connection";
    static const char* date = "Date";
    static const char* host = "Host";
    static const char* authorization = "Authorization";
    static const char* userAgent = "User-Agent";

    _stream << request.method() << ' '
            << request.url();

    if (!request.qparams().empty() && request.method() == "GET")
        _stream << '?' << request.qparams();

    _stream << " HTTP/"
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
        char buffer[50];
        _stream << "Date: " << MessageHeader::htdateCurrent(buffer) << "\r\n";
    }

    if (!request.header().hasHeader(host))
    {
        _stream << "Host: " << _addrInfo.host();
        unsigned short port = _addrInfo.port();
        if (port != 80)
            _stream << ':' << port;
        _stream << "\r\n";
    }

    if (!request.header().hasHeader(userAgent))
    {
        _stream << "User-Agent: " PACKAGE_STRING " http client\r\n";
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

        _errorPending = false;
        socket.endConnect();
#ifdef WITH_SSL
        if (_ssl)
        {
            socket.beginSslConnect();
            return;
        }
#endif

        sendRequest(*_request);

        log_debug("request sent - begin write");
        _stream.buffer().beginWrite();
    }
    catch (const std::exception& )
    {
        _errorPending = true;
        _client->replyFinished(*_client);

        if (_errorPending)
            throw;
    }
}

#ifdef WITH_SSL
void ClientImpl::onSslConnect(net::TcpSocket& socket)
{
    try
    {
        log_trace("onSslConnect");

        _errorPending = false;
        socket.endSslConnect();
        sendRequest(*_request);

        log_debug("request sent - begin write");
        _stream.buffer().beginWrite();
    }
    catch (const std::exception& )
    {
        _errorPending = true;
        _client->replyFinished(*_client);

        if (_errorPending)
            throw;
    }
}
#endif

void ClientImpl::onOutput(StreamBuffer& sb)
{
    log_trace("ClientImpl::onOutput; out_avail=" << sb.out_avail());

    try
    {
        try
        {
            _errorPending = false;

            sb.endWrite();

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
        catch (const IOError& e)
        {
            if (_reconnectOnError && _request != 0)
            {
                log_debug("reconnect on error");
                _socket.close();
                _reconnectOnError = false;
                reexecuteBegin(*_request);
                return;
            }

            throw;
        }
    }
    catch (const std::exception& e)
    {
        log_warn("error of type " << typeid(e).name() << " occured: " << e.what());

        _errorPending = true;

        _client->replyFinished(*_client);

        if (_errorPending)
            throw;
    }
}

void ClientImpl::onInput(StreamBuffer& sb)
{
    try
    {
        try
        {
            log_trace("ClientImpl::onInput; readHeader=" << _readHeader);

            _errorPending = false;

            sb.endRead();

            if (sb.device()->eof())
                throw IOError("end of input");

            _reconnectOnError = false;

            if (_readHeader)
            {
                processHeaderAvailable(sb);
            }
            else
            {
                processBodyAvailable(sb);
            }
        }
        catch (const IOError& e)
        {
            // after writing the request, the first read request may
            // detect, that the server has already closed the connection,
            // so check it here
            if (_readHeader && _reconnectOnError && _request != 0)
            {
                log_debug("reconnect on error");
                _socket.close();
                _reconnectOnError = false;
                reexecuteBegin(*_request);
                return;
            }

            throw;
        }
    }
    catch (const std::exception& e)
    {
        _errorPending = true;
        _client->replyFinished(*_client);

        if (_errorPending)
            throw;
    }
}

void ClientImpl::processHeaderAvailable(StreamBuffer& sb)
{
    _parser.advance(sb);

    if (_parser.fail())
        throw std::runtime_error("http parser failed"); // TODO define exception class

    if( _parser.end() )
    {
        _chunkedEncoding = _reply.header().chunkedTransferEncoding();

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
            _bodyStream.clear();
            _bodyStream.icount(_reply.header().contentLength());

            log_debug("header received - content-length=" << _bodyStream.icount());

            if (_bodyStream.icount() > 0)
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
                if (!_reply.header().keepAlive())
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

                    if (!_reply.header().keepAlive())
                    {
                        log_debug("close socket - no keep alive");
                        _socket.close();
                    }

                    _client->replyFinished(*_client);
                }
            }

            if (_chunkedIStream.fail())
                throw IOError("error reading HTTP reply body");
        }
        else if( _chunkedIStream.eod() )
        {
            if( _reply.header().hasHeader("Trailer") )
                _parser.readHeader();
            else
                _client->replyFinished(*_client);
        }

        if (_socket.enabled())
        {
            if ((!_chunkedIStream.eod() || !_parser.end()))
            {
                log_debug("call beginRead");
                sb.beginRead();
            }
        }
        else
        {
            cancel();
        }
    }
    else
    {
        log_debug("content-length(pre)=" << _bodyStream.icount());

        while (_stream.good() && _bodyStream.good() && _bodyStream.rdbuf()->in_avail() > 0)
        {
            _client->bodyAvailable(*_client); // TODO: may throw exception
            log_debug("content-length(post)=" << _bodyStream.icount());
        }

        if (_stream.fail() || _bodyStream.fail())
            throw IOError("error reading HTTP reply body");

        if( _bodyStream.icount() <= 0 )
        {
            log_debug("reply finished");

            if (!_reply.header().keepAlive())
            {
                log_debug("close socket - no keep alive");
                _socket.close();
            }

            _client->replyFinished(*_client);
        }
        else if (_socket.enabled() && _stream.good())
        {
            sb.beginRead();
        }
        else
        {
            cancel();
        }
    }
}

void ClientImpl::cancel()
{
    _socket.close();
    _stream.clear();
    _stream.buffer().discard();

    _chunkedIStream.reset();
}


} // namespace http

} // namespace cxxtools
