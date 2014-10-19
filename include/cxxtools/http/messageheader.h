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

#ifndef cxxtools_Http_MessageHeader_h
#define cxxtools_Http_MessageHeader_h

#include <string>
#include <cstring>
#include <utility>

namespace cxxtools
{

namespace http
{

class MessageHeader
{
    public:
        static const unsigned MAXHEADERSIZE = 4096;

    private:
        char _rawdata[MAXHEADERSIZE];  // key_1\0value_1\0key_2\0value_2\0...key_n\0value_n\0\0
        unsigned _endOffset;
        char* eptr() { return _rawdata + _endOffset; }
        unsigned _httpVersionMajor;
        unsigned _httpVersionMinor;

    public:
        typedef std::pair<const char*, const char*> value_type;
        class const_iterator
            : public std::iterator<std::forward_iterator_tag, value_type>
        {
            friend class MessageHeader;

            value_type current_value;

            void fixup()
            {
                if (*current_value.first)
                    current_value.second = current_value.first + std::strlen(current_value.first) + 1;
                else
                    current_value.first = current_value.second = 0;
            }

            void moveForward()
            {
                current_value.first = current_value.second + std::strlen(current_value.second) + 1;
                fixup();
            }

          public:
            const_iterator()
                : current_value(0, 0)
            { }

            explicit const_iterator(const char* p)
                : current_value(p, p)
            {
                fixup();
            }

            bool operator== (const const_iterator& it) const
            { return current_value.first == it.current_value.first; }

            bool operator!= (const const_iterator& it) const
            { return current_value.first != it.current_value.first; }

            const_iterator& operator++()
            {
                moveForward();
                return *this;
            }

            const_iterator operator++(int)
            {
                const_iterator ret = *this;
                moveForward();
                return ret;
            }

            const value_type& operator* () const   { return current_value; }
            const value_type* operator-> () const  { return &current_value; }
        };


        MessageHeader()
            : _endOffset(0),
              _httpVersionMajor(1),
              _httpVersionMinor(1)
        {
            _rawdata[0] = _rawdata[1] = '\0';
        }

        virtual ~MessageHeader()  {}

        void clear();

        void setHeader(const char* key, const char* value, bool replace = true);

        void addHeader(const char* key, const char* value)
        { setHeader(key, value, false); }

        void removeHeader(const char* key);

        const char* getHeader(const char* key) const;

        bool hasHeader(const char* key) const
        { return getHeader(key) != 0; }

        bool isHeaderValue(const char* key, const char* value) const;

        const_iterator begin() const
        { return const_iterator(_rawdata); }

        const_iterator end() const
        { return const_iterator(); }

        unsigned httpVersionMajor() const
        { return _httpVersionMajor; }

        unsigned httpVersionMinor() const
        { return _httpVersionMinor; }

        void httpVersion(unsigned major, unsigned minor)
        {
            _httpVersionMajor = major;
            _httpVersionMinor = minor;
        }

        bool chunkedTransferEncoding() const;

        std::size_t contentLength() const;

        bool keepAlive() const;

        /// Returns a properly formatted current time-string, as needed in http.
        /// The buffer must have at least 30 bytes.
        static char* htdateCurrent(char* buffer);

};

} // namespace http

} // namespace cxxtools

#endif
