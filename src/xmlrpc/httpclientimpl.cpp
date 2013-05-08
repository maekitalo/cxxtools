/*
 * Copyright (C) 2009 by Tommi Meakitalo
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

#include "httpclientimpl.h"
#include "cxxtools/log.h"
#include "cxxtools/http/replyheader.h"
#include "cxxtools/ioerror.h"
#include "cxxtools/remoteclient.h"
#include "cxxtools/clock.h"
#include <string.h>

log_define("cxxtools.xmlrpc.httpclient.impl")

namespace cxxtools {

namespace xmlrpc {

HttpClientImpl::HttpClientImpl()
{
    _request.method("POST");
    cxxtools::connect(_client.headerReceived, *this, &HttpClientImpl::onReplyHeader);
    cxxtools::connect(_client.bodyAvailable, *this, &HttpClientImpl::onReplyBody);
    cxxtools::connect(_client.replyFinished, *this, &HttpClientImpl::onReplyFinished);
}

HttpClientImpl::HttpClientImpl(SelectorBase& selector, const std::string& addr,
       unsigned short port, const std::string& url)
: _client(selector, addr, port)
, _request(url)
{
    _request.method("POST");
    cxxtools::connect(_client.headerReceived, *this, &HttpClientImpl::onReplyHeader);
    cxxtools::connect(_client.bodyAvailable, *this, &HttpClientImpl::onReplyBody);
    cxxtools::connect(_client.replyFinished, *this, &HttpClientImpl::onReplyFinished);
}

HttpClientImpl::HttpClientImpl(const std::string& addr, unsigned short port, const std::string& url)
: _client(addr, port)
, _request(url)
{
    _request.method("POST");
    cxxtools::connect(_client.headerReceived, *this, &HttpClientImpl::onReplyHeader);
    cxxtools::connect(_client.bodyAvailable, *this, &HttpClientImpl::onReplyBody);
    cxxtools::connect(_client.replyFinished, *this, &HttpClientImpl::onReplyFinished);
}

std::string HttpClientImpl::url() const
{
    std::ostringstream s;
    s << "http://"
      << _client.host()
      << ':'
      << _client.port()
      << _request.url();

    return s.str();
}

void HttpClientImpl::onReplyHeader(http::Client& client)
{
    log_debug("httpReturnCode=" << client.header().httpReturnCode()
        << " content-type=" << client.header().getHeader("Content-Type"));

    verifyHeader(client.header());

    ClientImpl::onReadReplyBegin(client.in());
}

std::size_t HttpClientImpl::onReplyBody(http::Client& client)
{
    return ClientImpl::onReadReply();
}

void HttpClientImpl::onReplyFinished(http::Client& client)
{
    ClientImpl::onReplyFinished();
}

void HttpClientImpl::beginExecute()
{
    _client.beginExecute(_request);
}

void HttpClientImpl::endExecute()
{
    if (_errorPending)
    {
        _errorPending = false;
        throw;
    }

    _client.endExecute();
}

std::string HttpClientImpl::execute()
{
    _client.execute(_request, timeout());

    std::string body;

    try
    {
        verifyHeader(_client.header());
        _client.readBody(body);
    }
    catch (...)
    {
        _client.cancel();
        throw;
    }

    return body;
}

std::ostream& HttpClientImpl::prepareRequest()
{
    _request.clear();
    _request.setHeader("Content-Type", "text/xml");
    _request.method("POST");
    return _request.body();
}


void HttpClientImpl::cancel()
{
    _client.cancel();
    ClientImpl::cancel();
}

void HttpClientImpl::wait(std::size_t msecs)
{
    if (!_client.selector())
        throw std::logic_error("cannot run async rpc request without a selector");

    Clock clock;
    if (msecs != RemoteClient::WaitInfinite)
        clock.start();

    std::size_t remaining = msecs;

    while (activeProcedure() != 0)
    {
        if (_client.selector()->wait(remaining) == false)
            throw IOTimeout();

        if (msecs != RemoteClient::WaitInfinite)
        {
            std::size_t diff = static_cast<std::size_t>(clock.stop().totalMSecs());
            remaining = diff >= msecs ? 0 : msecs - diff;
        }
    }
}

void HttpClientImpl::verifyHeader(const http::ReplyHeader& header)
{
    if (header.httpReturnCode() != 200)
    {
        std::ostringstream msg;
        msg << "invalid http return code "
            << header.httpReturnCode()
            << ": "
            << header.httpReturnText();
        throw std::runtime_error(msg.str());
    }

    const char* contentType = header.getHeader("Content-Type");
    if (contentType == 0)
        throw std::runtime_error("missing content type header");

    if (::strncasecmp(contentType, "text/xml", 8) != 0)
    {
        std::ostringstream msg;
        msg << "invalid content type " << contentType;
        throw std::runtime_error(msg.str());
    }

}


}

}
