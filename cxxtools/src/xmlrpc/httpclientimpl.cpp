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

namespace cxxtools {

namespace xmlrpc {

HttpClientImpl::HttpClientImpl()
{
    _request.method("POST");
    cxxtools::connect(_client.headerReceived, *this, &HttpClientImpl::onReplyHeader);
    cxxtools::connect(_client.bodyAvailable, *this, &HttpClientImpl::onReplyBody);
    cxxtools::connect(_client.replyFinished, *this, &HttpClientImpl::onReplyFinished);
    cxxtools::connect(_client.errorOccured, *this, &HttpClientImpl::onErrorOccured);
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
    cxxtools::connect(_client.errorOccured, *this, &HttpClientImpl::onErrorOccured);
}

HttpClientImpl::HttpClientImpl(const std::string& addr, unsigned short port, const std::string& url)
: _client(addr, port)
, _request(url)
{
    _request.method("POST");
    cxxtools::connect(_client.headerReceived, *this, &HttpClientImpl::onReplyHeader);
    cxxtools::connect(_client.bodyAvailable, *this, &HttpClientImpl::onReplyBody);
    cxxtools::connect(_client.replyFinished, *this, &HttpClientImpl::onReplyFinished);
    cxxtools::connect(_client.errorOccured, *this, &HttpClientImpl::onErrorOccured);
}

std::string HttpClientImpl::url() const
{
    std::ostringstream s;
    s << "http://"
      << _client.server()
      << ':'
      << _client.port()
      << _request.url();

    return s.str();
}

void HttpClientImpl::onReplyHeader(http::Client& client)
{
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

void HttpClientImpl::onErrorOccured(http::Client& client, const std::exception& e)
{
    ClientImpl::onErrorOccured(e);
}

void HttpClientImpl::beginExecute()
{
    _client.beginExecute(_request);
}

std::string HttpClientImpl::execute()
{
    _client.execute(_request, timeout());
    std::string body;
    _client.readBody(body);
    return body;
}

std::ostream& HttpClientImpl::prepareRequest()
{
    _request.clear();
    _request.setHeader("Content-Type", "text/xml");
    _request.method("POST");
    return _request.body();
}


}

}
