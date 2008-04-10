/***************************************************************************
 *   Copyright (C) 2006 by Tommi Maekitalo                                 *
 *   Copyright (C) 2006 by Marc Boris Duerner                              *
 *   Copyright (C) 2006 by Stefan Bueder                                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this program; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "cxxtools/xtime.h"
#include <sstream>
#include <cctype>

namespace cxxtools {

InvalidTime::InvalidTime(const SourceInfo& si)
: std::invalid_argument("Invalid time" + si)
{
}


InvalidTime::~InvalidTime() throw()
{
}


Time::Time(unsigned h, unsigned m, unsigned s, unsigned ms)
{
    set(h, m, s, ms);
}


unsigned Time::hour() const
{
    return _msecs / MSecsPerHour;
}


unsigned Time::minute() const
{
    return (_msecs % MSecsPerHour) / MSecsPerMinute;
}


unsigned Time::second() const
{
    return (_msecs / 1000) % SecondsPerMinute;
}


unsigned Time::msec() const
{
    return _msecs % 1000;
}


void Time::set(unsigned h, unsigned m, unsigned s, unsigned ms)
{
    if ( !isValid(h, m, s ,ms) )
    {
        throw InvalidTime(CXXTOOLS_SOURCEINFO);
    }

    _msecs = (h*SecondsPerHour + m*SecondsPerMinute + s) * 1000 + ms;
}


void Time::get(unsigned& h, unsigned& m, unsigned& s, unsigned& ms) const
{
    h = hour();
    m = minute();
    s = second();
    ms = msec();
}


Time Time::addSecs(int secs) const
{
    return addMSecs(secs * 1000);
}


int Time::secsTo(const Time &t) const
{
    return (t._msecs - _msecs) / 1000;
}


Time Time::addMSecs(cxxtools::int64_t ms) const
{
    Time t;
    if (ms < 0)
    {
        cxxtools::int64_t negdays = (MSecsPerDay - ms) / MSecsPerDay;
        t._msecs = static_cast<unsigned>((_msecs + ms + negdays * MSecsPerDay) % MSecsPerDay);
    }
    else
    {
        t._msecs = static_cast<unsigned>((_msecs + ms) % MSecsPerDay);
    }

    return t;
}


cxxtools::int64_t Time::msecsTo(const Time &t) const
{
    return t._msecs - _msecs;
}


bool Time::isValid(unsigned h, unsigned m, unsigned s, unsigned ms)
{
    return h < 24 && m < 60 && s < 60 && ms < 1000;
}


std::string Time::toIsoString() const
{
    unsigned hour, minute, second, msec;
    this->get(hour, minute, second, msec);

    // format hh:mm:ss.sssss
    //        0....+....1....+
    char ret[14];
    ret[0] = '0' + hour / 10;
    ret[1] = '0' + hour % 10;
    ret[2] = ':';
    ret[3] = '0' + minute / 10;
    ret[4] = '0' + minute % 10;
    ret[5] = ':';
    ret[6] = '0' + second / 10;
    ret[7] = '0' + second % 10;
    ret[8] = '.';
    unsigned short n = msec;
    ret[11] = '0' + n % 10;
    n /= 10;
    ret[10] = '0' + n % 10;
    n /= 10;
    ret[9] = '0' + n % 10;

    return std::string(ret, 12);
}


inline unsigned short getNumber2(const char* s)
{
    if ( !std::isdigit(s[0]) || !std::isdigit(s[1]) )
        throw InvalidTime(CXXTOOLS_SOURCEINFO);

    return (s[0] - '0') * 10 + (s[1] - '0');
}


inline unsigned short getNumber3(const char* s)
{
    if( !std::isdigit(s[0]) || !std::isdigit(s[1]) || !std::isdigit(s[2]) )
        throw InvalidTime(CXXTOOLS_SOURCEINFO);

    return ( s[0] - '0') * 100
             + (s[1] - '0') * 10
             + (s[2] - '0' );
}


Time Time::fromIsoString(const std::string& s)
{
    if( s.size() < 11 || s.at(2) != ':'
        || s.at(5) != ':' || s.at(8) != '.')
    {
        throw InvalidTime(CXXTOOLS_SOURCEINFO);
    }

    const char* d = s.data();
    return Time(getNumber2(d), getNumber2(d + 3), getNumber2(d + 6),
                getNumber3(d + 9));
}

} // namespace cxxtools

