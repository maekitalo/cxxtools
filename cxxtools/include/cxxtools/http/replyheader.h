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

#ifndef cxxtools_Http_ReplyHeader_h
#define cxxtools_Http_ReplyHeader_h

#include <cxxtools/http/api.h>
#include <cxxtools/http/messageheader.h>

namespace cxxtools {

namespace http {

class RequestHeader;

class ReplyHeader : public MessageHeader
{
        unsigned _httpReturnCode;
        std::string _httpReturnText;

    public:
        ReplyHeader()
            : _httpReturnCode(200),
              _httpReturnText("OK")
            { }

        void clear()
        {
            MessageHeader::clear();
            _httpReturnCode = 200;
            _httpReturnText = "OK";
        }

        unsigned httpReturnCode() const
        { return _httpReturnCode; }

        const std::string& httpReturnText() const
        { return _httpReturnText; }

        void httpReturn(unsigned c, const std::string& t)
        {
            _httpReturnCode = c;
            _httpReturnText = t;
        }

};

} // namespace http

} // namespace cxxtools

#endif
