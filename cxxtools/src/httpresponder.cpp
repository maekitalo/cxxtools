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

#include <cxxtools/httpresponder.h>
#include <cxxtools/httpreply.h>
#include <cxxtools/log.h>

log_define("cxxtools.net.http.responder")

namespace cxxtools {

namespace net {

void HttpResponder::beginRequest(std::istream& in, HttpRequest& request)
{
}

std::size_t HttpResponder::readBody(std::istream& in)
{
    std::streambuf* sb = in.rdbuf();

    std::size_t ret = 0;
    while (sb->in_avail() > 0)
    {
        sb->sbumpc();
        ++ret;
    }

    return ret;
}

void HttpResponder::replyError(std::ostream& out, HttpRequest& request, HttpReply& reply, const std::exception& ex)
{
    reply.httpReturn(500, "internal server error");
    reply.setHeader("Content-Type", "text/plain");
    out << ex.what();
}

void HttpNotFoundResponder::reply(std::ostream& out, HttpRequest& request, HttpReply& reply)
{
    reply.httpReturn(404, "Not found");
}

HttpResponder* HttpNotFoundService::createResponder(const HttpRequest&)
{
    return &_responder;
}

void HttpNotFoundService::releaseResponder(HttpResponder*)
{ }


} // namespace net

} // namespace cxxtools
