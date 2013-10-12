/*
 * Copyright (C) 2011 by Tommi Meakitalo
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
#include "cxxtools/remoteprocedure.h"
#include "cxxtools/textstream.h"
#include "cxxtools/jsonformatter.h"
#include "cxxtools/http/replyheader.h"
#include "cxxtools/selectable.h"
#include "cxxtools/utf8codec.h"
#include "cxxtools/ioerror.h"
#include "cxxtools/clock.h"
#include "cxxtools/log.h"

#include <stdexcept>
#include <string.h>

log_define("cxxtools.json.client.impl")

namespace cxxtools
{

namespace json
{

HttpClientImpl::HttpClientImpl()
: _timeout(Selectable::WaitInfinite),
  _connectTimeoutSet(false),
  _connectTimeout(Selectable::WaitInfinite),
  _proc(0),
  _exceptionPending(false),
  _count(0)
{
    _request.method("POST");
    cxxtools::connect(_client.headerReceived, *this, &HttpClientImpl::onReplyHeader);
    cxxtools::connect(_client.bodyAvailable, *this, &HttpClientImpl::onReplyBody);
    cxxtools::connect(_client.replyFinished, *this, &HttpClientImpl::onReplyFinished);
}

void HttpClientImpl::beginCall(IComposer& r, IRemoteProcedure& method, IDecomposer** argv, unsigned argc)
{
    if (_client.selector() == 0)
        throw std::logic_error("cannot run async rpc request without a selector");

    if (_proc)
        throw std::logic_error("asyncronous request already running");

    _proc = &method;

    prepareRequest(method.name(), argv, argc);

    _client.beginExecute(_request);

    _scanner.begin(_deserializer, r);
}


void HttpClientImpl::endCall()
{
    log_debug("end call; errorPending=" << _exceptionPending);
    if (_exceptionPending)
    {
        _exceptionPending = false;
        throw;
    }

    _client.endExecute();
}


void HttpClientImpl::call(IComposer& r, IRemoteProcedure& method, IDecomposer** argv, unsigned argc)
{
    _proc = &method;

    prepareRequest(method.name(), argv, argc);

    _client.execute(_request, timeout(), connectTimeout());

    _scanner.begin(_deserializer, r);

    char ch;
    std::istream& is = _client.in();
    while (is.get(ch))
    {
        if (_scanner.advance(ch))
        {
            log_debug("scanner finished");
            _proc = 0;
            _scanner.finalizeReply();
            return;
        }
    }

    throw std::runtime_error("unexpected end of data");
}


const IRemoteProcedure* HttpClientImpl::activeProcedure() const
{
    return _proc;
}

void HttpClientImpl::cancel()
{
    _client.cancel();
    _proc = 0;
}

// private members

void HttpClientImpl::prepareRequest(const String& name, IDecomposer** argv, unsigned argc)
{
    _request.clear();
    _request.setHeader("Content-Type", "application/json");
    _request.method("POST");

    TextOStream ts(_request.body(), new Utf8Codec());
    JsonFormatter formatter;

    formatter.begin(ts);

    formatter.beginObject(std::string(), std::string());

    formatter.addValueStdString("jsonrpc", std::string(), "2.0");
    formatter.addValueString("method", std::string(), name);
    formatter.addValueInt("id", "int", ++_count);

    formatter.beginArray("params", std::string());

    for(unsigned n = 0; n < argc; ++n)
    {
        argv[n]->format(formatter);
    }

    formatter.finishArray();

    formatter.finishObject();

    formatter.finish();

    ts.flush();
}

void HttpClientImpl::onReplyHeader(http::Client& client)
{
    verifyHeader(client.header());
}

std::size_t HttpClientImpl::onReplyBody(http::Client& client)
{
    std::size_t count = 0;
    char ch;
    std::istream& is = client.in();
    while (is.rdbuf()->in_avail() && is.get(ch))
    {
        ++count;
        if (_scanner.advance(ch))
        {
            log_debug("scanner finished");
            try
            {
                _scanner.finalizeReply();
            }
            catch (const RemoteException& e)
            {
                _proc->setFault(e.rc(), e.text());
            }
            catch (const std::exception&)
            {
                log_warn("exception occured in finalizeReply");
                _exceptionPending = true;
                _proc->onFinished();
            }

            break;
        }
    }

    log_debug("no more data - " << count << " bytes consumed");

    return count;
}

void HttpClientImpl::onReplyFinished(http::Client& client)
{
    log_debug("onReplyFinished; method=" << static_cast<void*>(_proc));

    try
    {
        _exceptionPending = false;
        endCall();
    }
    catch (const std::exception& e)
    {
        if (!_proc)
            throw;

        _exceptionPending = true;

        IRemoteProcedure* proc = _proc;
        _proc = 0;
        proc->onFinished();
        if (_exceptionPending)
        {
            _exceptionPending = false;
            throw;
        }
        return;
    }

    IRemoteProcedure* proc = _proc;
    _proc = 0;
    proc->onFinished();
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

    if (::strncasecmp(contentType, "application/json", 16) != 0)
    {
        std::ostringstream msg;
        msg << "invalid content type " << contentType;
        throw std::runtime_error(msg.str());
    }

}

}

}
