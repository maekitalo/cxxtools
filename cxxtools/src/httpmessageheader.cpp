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

#include <cxxtools/httpmessageheader.h>
#include <cxxtools/clock.h>
#include <cctype>
#include <sstream>

namespace cxxtools {

namespace net {

bool HttpMessageHeader::StringLessIgnoreCase::operator()
    (const std::string& s1, const std::string& s2) const
{
    std::string::const_iterator it1 = s1.begin();
    std::string::const_iterator it2 = s2.begin();
    while (it1 != s1.end() && it2 != s2.end())
    {
        if (*it1 != *it2)
        {
            char c1 = std::toupper(*it1);
            char c2 = std::toupper(*it2);
            if (c1 < c2)
                return true;
            else if (c2 < c1)
                return false;
        }
        ++it1;
        ++it2;
    }
    return it1 == s1.end() ? (it2 != s2.end()) : (it2 == s2.end());
}

std::size_t HttpMessageHeader::contentLength() const
{
    std::string s = getHeader("Content-Length");
    if (s.empty())
        return 0;

    std::istringstream ss(s);
    std::size_t size = 0;
    ss >> size;
    return size;
}

bool HttpMessageHeader::keepAlive() const
{
    std::string ch = getHeader("Connection");

    for (std::string::iterator c = ch.begin(); c != ch.end(); ++c)
        *c = std::tolower(*c);

    return ch == "keep-alive" ||
           (ch.empty()
                && httpVersionMajor() == 1
                && httpVersionMinor() >= 1);
}

std::string HttpMessageHeader::htdateCurrent()
{
    int year = 0;
    unsigned month = 0;
    unsigned day = 0;
    unsigned hour = 0;
    unsigned min = 0;
    unsigned sec = 0;
    unsigned msec = 0;

    Pt::DateTime dt = Pt::System::Clock::getSystemTime();
    dt.get(year, month, day,hour, min, sec, msec);

    static const char* wday[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
    static const char* monthn[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun",
                                   "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
    char buffer[80];
    sprintf(buffer, "%s, %02d %s %d %02d:%02d:%02d GMT",
                    wday[day], day, monthn[month], year, hour, min, sec);

    return std::string(buffer);

    //TODO: use System::Clock
}

} // namespace net

} // namespace cxxtools
