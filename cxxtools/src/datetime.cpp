/*
 * Copyright (C) 2006 by Tommi Maekitalo
 * Copyright (C) 2006 by Marc Boris Duerner
 * Copyright (C) 2006 by Stefan Bueder
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
#include "cxxtools/datetime.h"
#include "cxxtools/convert.h"
#include "cxxtools/serializationinfo.h"
#include <algorithm>
#include <sstream>
#include <stdexcept>
#include <cctype>
#include <cmath>
#include <cassert>

namespace cxxtools
{

namespace
{

unsigned short getNumber2(const char* s)
{
    if( ! std::isdigit(s[0]) || !std::isdigit(s[1]) )
        throw ConversionError( CXXTOOLS_ERROR_MSG("Invalid DateTime format") );

    return (s[0] - '0') * 10 + (s[1] - '0');
}

unsigned short getNumber3(const char* s)
{
    if (!std::isdigit(s[0]) || !std::isdigit(s[1]) || !std::isdigit(s[2]))
        throw ConversionError( CXXTOOLS_ERROR_MSG("Invalid DateTime format") );

    return (s[0] - '0') * 100
        + (s[1] - '0') * 10
        + (s[2] - '0');
}

unsigned short getNumber4(const char* s)
{
    if( ! std::isdigit(s[0]) || ! std::isdigit(s[1]) ||
        ! std::isdigit(s[2]) || ! std::isdigit(s[3]) )
        throw ConversionError( CXXTOOLS_ERROR_MSG("Invalid DateTime format") );

    return (s[0] - '0') * 1000
        + (s[1] - '0') * 100
        + (s[2] - '0') * 10
        + (s[3] - '0');
}

}

cxxtools::int64_t DateTime::msecsSinceEpoch() const
{
    static const DateTime dt(1970, 1, 1);
    return (*this - dt).totalMSecs();
}

DateTime& DateTime::operator+=(const Timespan& ts)
{
    cxxtools::int64_t totalMSecs = ts.totalMSecs();
    cxxtools::int64_t days = totalMSecs / Time::MSecsPerDay;
    cxxtools::int64_t overrun = totalMSecs % Time::MSecsPerDay;

    if( (-overrun) > _time.totalMSecs()  )
    {
        days -= 1;
    }
    else if( overrun + _time.totalMSecs() > Time::MSecsPerDay)
    {
        days += 1;
    }

    _date += static_cast<int>(days);
    _time += Timespan(overrun * 1000);
    return *this;
}

DateTime& DateTime::operator-=(const Timespan& ts)
{
    cxxtools::int64_t totalMSecs = ts.totalMSecs();
    cxxtools::int64_t days = totalMSecs / Time::MSecsPerDay;
    cxxtools::int64_t overrun = totalMSecs % Time::MSecsPerDay;

    if( overrun > _time.totalMSecs() )
    {
        days += 1;
    }
    else if(_time.totalMSecs() - overrun > Time::MSecsPerDay)
    {
        days -= 1;
    }

    _date -= static_cast<int>(days);
    _time -= Timespan( overrun * 1000 );
    return *this;
}

Timespan operator-(const DateTime& first, const DateTime& second)
{
    cxxtools::int64_t dayDiff      = cxxtools::int64_t( first.date().julian() ) -
                               cxxtools::int64_t( second.date().julian() );

    cxxtools::int64_t milliSecDiff = cxxtools::int64_t( first.time().totalMSecs() ) -
                               cxxtools::int64_t( second.time().totalMSecs() );

    cxxtools::int64_t result = (dayDiff * Time::MSecsPerDay + milliSecDiff) * 1000;

    return result;
}

DateTime operator+(const DateTime& dt, const Timespan& ts)
{
    DateTime tmp = dt;
    tmp += ts;
    return tmp;
}

DateTime operator-(const DateTime& dt, const Timespan& ts)
{
    DateTime tmp = dt;
    tmp -= ts;
    return tmp;
}

void convert(DateTime& dt, const std::string& s)
{
    if (s.size() < 23
        || s.at(4) != '-'
        || s.at(7) != '-'
        || s.at(10) != ' '
        || s.at(13) != ':'
        || s.at(16) != ':'
        || s.at(19) != '.')
        throw ConversionError( CXXTOOLS_ERROR_MSG("Invalid DateTime format") );

    const char* d = s.data();

    dt= DateTime( getNumber4(d),
                  getNumber2(d + 5),
                  getNumber2(d + 8),
                  getNumber2(d + 11),
                  getNumber2(d + 14),
                  getNumber2(d + 17),
                  getNumber3(d + 20) );
}


void convert(std::string& str, const DateTime& dt)
{
    // format YYYY-MM-DD hh:mm:ss.sss
    //        0....+....1....+....2....+
    char ret[24];
    unsigned short n = dt.date().year();
    ret[3] = '0' + n % 10;
    n /= 10;
    ret[2] = '0' + n % 10;
    n /= 10;
    ret[1] = '0' + n % 10;
    n /= 10;
    ret[0] = '0' + n % 10;
    ret[4] = '-';
    ret[5] = '0' + dt.date().month() / 10;
    ret[6] = '0' + dt.date().month() % 10;
    ret[7] = '-';
    ret[8] = '0' + dt.date().day() / 10;
    ret[9] = '0' + dt.date().day() % 10;
    ret[10] = ' ';
    ret[11] = '0' + dt.time().hour() / 10;
    ret[12] = '0' + dt.time().hour() % 10;
    ret[13] = ':';
    ret[14] = '0' + dt.time().minute() / 10;
    ret[15] = '0' + dt.time().minute() % 10;
    ret[16] = ':';
    ret[17] = '0' + dt.time().second() / 10;
    ret[18] = '0' + dt.time().second() % 10;
    ret[19] = '.';
    n = dt.time().msec();
    ret[22] = '0' + n % 10;
    n /= 10;
    ret[21] = '0' + n % 10;
    n /= 10;
    ret[20] = '0' + n % 10;

    str.assign(ret, 23);
}

void operator >>=(const SerializationInfo& si, DateTime& datetime)
{
    if (si.category() == cxxtools::SerializationInfo::Object)
    {
        unsigned short year, month, day, hour, min, sec, msec;
        si.getMember("year") >>= year;
        si.getMember("month") >>= month;
        si.getMember("day") >>= day;
        si.getMember("hour") >>= hour;

        const cxxtools::SerializationInfo* p;

        if ((p = si.findMember("minute")) != 0)
            *p >>= min;
        else
            si.getMember("min") >>= min;

        if ((p = si.findMember("second")) != 0)
            *p >>= sec;
        else
            si.getMember("sec") >>= sec;

        if ((p = si.findMember("millisecond")) != 0
            || (p = si.findMember("msec")) != 0)
            *p >>= msec;
        else
            msec = 0;

        datetime.set(year, month, day, hour, min, sec, msec);
    }
    else
    {
        std::string s;
        si.getValue(s);
        convert(datetime, s);
    }
}

void operator <<=(SerializationInfo& si, const DateTime& datetime)
{
    std::string s;
    convert(s, datetime);
    si.setValue(s);
    si.setTypeName( "DateTime");
}

}
