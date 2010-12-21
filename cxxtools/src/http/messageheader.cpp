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

#include <cxxtools/http/messageheader.h>
#include <cxxtools/clock.h>
#include <cxxtools/log.h>
#include <cctype>
#include <sstream>
#include <stdio.h>
#include <string.h>

log_define("cxxtools.http.messageheader")

namespace cxxtools
{

namespace http
{

namespace
{

int compareIgnoreCase(const char* s1, const char* s2)
{
    const char* it1 = s1;
    const char* it2 = s2;
    while (*it1 && *it2)
    {
        if (*it1 != *it2)
        {
            char c1 = std::toupper(*it1);
            char c2 = std::toupper(*it2);
            if (c1 < c2)
                return -1;
            else if (c2 < c1)
                return 1;
        }
        ++it1;
        ++it2;
    }

    return *it1 ? 1
                : *it2 ? -1 : 0;
}
} 
const unsigned MessageHeader::MAXHEADERSIZE;

const char* MessageHeader::getHeader(const char* key) const
{
    for (const_iterator it = begin(); it != end(); ++it)
    {
        if (compareIgnoreCase(key, it->first) == 0)
            return it->second;
    }

    return 0;
}

bool MessageHeader::isHeaderValue(const char* key, const char* value) const
{
    const char* h = getHeader(key);
    if (h == 0)
        return false;
    return compareIgnoreCase(h, value) == 0;
}

void MessageHeader::clear()
{
    _rawdata[0] = _rawdata[1] = '\0';
    _endOffset = 0;
    _httpVersionMajor = 1;
    _httpVersionMinor = 1;
}

void MessageHeader::setHeader(const char* key, const char* value, bool replace)
{
    log_debug("setHeader(\"" << key << "\", \"" << value << "\", " << replace << ')');

    if (!*key)
        throw std::runtime_error("empty key not allowed in messageheader");

    if (replace)
        removeHeader(key);

    char* p = eptr();

    size_t lk = strlen(key);     // length of key
    size_t lv = strlen(value);   // length of value

    if (p - _rawdata + lk + lv + 2 > MAXHEADERSIZE)
        throw std::runtime_error("message header too big");

    std::strcpy(p, key);   // copy key
    p += lk + 1;
    std::strcpy(p, value); // copy value
    p[lv + 1] = '\0';      // put new message end marker in place

    _endOffset = (p + lv + 1) - _rawdata;
}

void MessageHeader::removeHeader(const char* key)
{
    if (!*key)
        throw std::runtime_error("empty key not allowed in messageheader");

    char* p = eptr();

    const_iterator it = begin();
    while (it != end())
    {
        if (compareIgnoreCase(key, it->first) == 0)
        {
            unsigned slen = it->second - it->first + std::strlen(it->second) + 1;

            std::memcpy(
                const_cast<char*>(it->first),
                it->first + slen,
                p - it->first + slen);

            p -= slen;

            it.fixup();
        }
        else
            ++it;
    }

    _endOffset = p - _rawdata;
}

bool MessageHeader::chunkedTransferEncoding() const
{
    return isHeaderValue("Transfer-Encoding", "chunked");
}

std::size_t MessageHeader::contentLength() const
{
    const char* s = getHeader("Content-Length");
    if (s == 0)
        return 0;

    std::size_t size = 0;
    while (*s >= '0' && *s <= '9')
        size = size * 10 + (*s++ - '0');

    return size;
}

bool MessageHeader::keepAlive() const
{
    const char* ch = getHeader("Connection");

    if (ch == 0)
        return httpVersionMajor() == 1
            && httpVersionMinor() >= 1;
    else
        return compareIgnoreCase(ch, "keep-alive") == 0;
}

char* MessageHeader::htdateCurrent(char* buffer)
{
    int year = 0;
    unsigned month = 0;
    unsigned day = 0;
    unsigned hour = 0;
    unsigned min = 0;
    unsigned sec = 0;
    unsigned msec = 0;

    cxxtools::DateTime dt = cxxtools::Clock::getSystemTime();
    dt.get(year, month, day, hour, min, sec, msec);
    unsigned dayOfWeek = dt.date().dayOfWeek();

    static const char* wday[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
    static const char* monthn[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun",
                                   "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
    sprintf(buffer, "%s, %02d %s %d %02d:%02d:%02d GMT",
                    wday[dayOfWeek], day, monthn[month-1], year, hour, min, sec);

    return buffer;
}

} // namespace http

} // namespace cxxtools
