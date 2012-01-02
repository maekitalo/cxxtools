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

#include "httpresponder.h"
#include <cxxtools/http/reply.h>
#include <cxxtools/json/httpservice.h>
#include <cxxtools/log.h>

log_define("cxxtools.json.httpresponder")

namespace cxxtools
{
namespace json
{

HttpResponder::HttpResponder(HttpService& service)
    : Responder(service),
      _responder(service)
{
}

HttpResponder::~HttpResponder()
{
}

void HttpResponder::beginRequest(std::istream& in, http::Request& request)
{
    log_debug("begin request");
    _responder.begin();
}

std::size_t HttpResponder::readBody(std::istream& is)
{
    log_debug("begin request");
    std::size_t n = 0;

    char ch;
    while (is.get(ch))
    {
        ++n;
        if (_responder.advance(ch))
            break;
    }

    log_debug(n << " bytes processed");
    return n;
}

void HttpResponder::reply(std::ostream& os, http::Request& request, http::Reply& reply)
{
    reply.setHeader("Content-Type", "application/jsonrpc");
    _responder.finalize(os);
}

}
}
