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

#ifndef cxxtools_Http_Reply_h
#define cxxtools_Http_Reply_h

#include <cxxtools/http/replyheader.h>
#include <string>
#include <sstream>

namespace cxxtools {

namespace http {

class Request;

class Reply
{
        ReplyHeader _header;
        std::stringstream _body;

    public:
        Reply()
            { }

        ReplyHeader& header()
        { return _header; }

        const ReplyHeader& header() const
        { return _header; }

        void setHeader(const char* key, const char* value)
        {
            _header.setHeader(key, value);
        }

        void addHeader(const char* key, const char* value)
        {
            _header.addHeader(key, value);
        }

        void removeHeader(const char* key)
        {
            _header.removeHeader(key);
        }

        const char* getHeader(const char* key) const
        {
            return _header.getHeader(key);
        }

        bool hasHeader(const char* key) const
        {
            return _header.hasHeader(key);
        }

        void clear()
        {
            _header.clear();
            _body.clear();
            _body.str(std::string());
        }

        unsigned httpReturnCode() const
        { return _header.httpReturnCode(); }

        const std::string& httpReturnText() const
        { return _header.httpReturnText(); }

        void httpReturn(unsigned c, const std::string& t)
        { _header.httpReturn(c, t); }

        std::string body() const
        { return _body.str(); }

        std::stringstream& bodyStream()
        { return _body; }

        std::size_t bodySize() const
        { return _body.str().size(); }

        void sendBody(std::ostream& out) const
        { out << _body.str(); }

        operator std::string() const
        { return _body.str(); }

};

} // namespace http

} // namespace cxxtools

#endif
