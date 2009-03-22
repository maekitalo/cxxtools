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

#ifndef cxxtools_Net_HttpMessage_h
#define cxxtools_Net_HttpMessage_h

#include <cxxtools/api.h>
#include <cxxtools/httpmessagehead.h>
#include <string>
#include <sstream>
#include <cctype>

namespace cxxtools {

namespace net {

class HttpMessage
{
        HttpMessageHead _head;

        std::stringstream _body;

    public:
        HttpMessage()
            { }

        virtual ~HttpMessage()  {}

        void clear()
        {
          _head.clear();
          _body.str(std::string());
          _httpVersionMajor = 1;
          _httpVersionMinor = 1;
        }

        void setHeader(const std::string& key, const std::string& value)
        {
            _head.setHeader(key, value);
        }

        void addHeader(const std::string& key, const std::string& value)
        {
            _head.addHeader(key, value);
        }

        std::string getHeader(const std::string& key) const
        {
            return _head.getHeader(key);
        }

        bool hasHeader(const std::string& key) const
        {
            return _head.hasHeader(key);
        }

        std::string bodyStr() const
        { return _body.str(); }

        std::iostream& body()
        { return _body; }

        unsigned httpVersionMajor() const
        {
            return _head.httpVersionMajor();
        }

        unsigned httpVersionMinor() const
        {
            return _head.httpVersionMinor();
        }

        void httpVersion(unsigned major, unsigned minor)
        {
            _head.httpVersion(major, minor);
        }

        std::size_t contentSize() const;

        HttpMessageHead& header()
        { return _head; }

        const HttpMessageHead& header() const
        { return _head; }

        bool keepAlive() const
        { return _head.keepAlive(); }

};

} // namespace net

} // namespace cxxtools

#endif
