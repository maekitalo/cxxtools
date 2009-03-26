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

#ifndef cxxtools_Net_HttpMessageHeader_h
#define cxxtools_Net_HttpMessageHeader_h

#include <cxxtools/api.h>
#include <string>
#include <map>

namespace cxxtools {

namespace net {

class CXXTOOLS_API HttpMessageHeader
{
        class CXXTOOLS_API StringLessIgnoreCase
        {
            public:
                bool operator()(const std::string& s1, const std::string& s2) const;
        };

        typedef std::map<std::string, std::string, StringLessIgnoreCase> Headers;
        Headers _headers;
        unsigned _httpVersionMajor;
        unsigned _httpVersionMinor;

    public:
        typedef Headers::const_iterator const_iterator;

        HttpMessageHeader()
            : _httpVersionMajor(1)
            , _httpVersionMinor(1)
            { }

        virtual ~HttpMessageHeader()  {}

        void clear()
        {
            _headers.clear();
            _httpVersionMajor = 1;
            _httpVersionMinor = 1;
        }

        void setHeader(const std::string& key, const std::string& value)
        {
            _headers[key] = value;
        }

        void addHeader(const std::string& key, const std::string& value)
        {
            Headers::iterator it = _headers.find(key);
            if (it == _headers.end())
                _headers[key] = value;
            else
            {
                it->second += ',';
                it->second += value;
            }
        }

        void removeHeader(const std::string& key)
        {
            _headers.erase(key);
        }

        std::string getHeader(const std::string& key) const
        {
            Headers::const_iterator it = _headers.find(key);
            return it == _headers.end() ? std::string() : it->second;
        }

        bool hasHeader(const std::string& key) const
        { return _headers.find(key) != _headers.end(); }

        unsigned httpVersionMajor() const
        { return _httpVersionMajor; }

        unsigned httpVersionMinor() const
        { return _httpVersionMinor; }

        void httpVersion(unsigned major, unsigned minor)
        {
            _httpVersionMajor = major;
            _httpVersionMinor = minor;
        }

        std::size_t contentLength() const;

        const_iterator begin() const
        { return _headers.begin(); }

        const_iterator end() const
        { return _headers.end(); }

        bool keepAlive() const;

        /// Returns a properly formatted current time-string, as needed in http.
        static std::string htdateCurrent();

};

} // namespace net

} // namespace cxxtools

#endif
