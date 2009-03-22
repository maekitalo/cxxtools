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

#ifndef cxxtools_Net_HttpRequestHeader_h
#define cxxtools_Net_HttpRequestHeader_h

#include <cxxtools/api.h>
#include <cxxtools/httpmessageheader.h>
#include <string>

namespace cxxtools {

namespace net {

class HttpRequestHeader : public HttpMessageHeader
{
        std::string _url;
        std::string _method;
        std::string _qparams;

    public:
        explicit HttpRequestHeader(const std::string& url = std::string())
        : _url(url),
          _method("GET")
        { }

        virtual ~HttpRequestHeader()  {}

        void clear()
        {
            HttpMessageHeader::clear();
            _method = "GET";
            _qparams.clear();
        }

        const std::string& url() const
        { return _url; }

        void url(const std::string& u)
        { _url = u; }

        const std::string& method() const
        { return _method; }

        void method(const std::string& m)
        { _method = m; }

        const std::string& qparams() const
        { return _qparams; }

        void qparams(const std::string& q)
        { _qparams = q; }

};

} // namespace net

} // namespace cxxtools

#endif
