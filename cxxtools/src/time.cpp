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
#include "cxxtools/time.h"
#include "cxxtools/convert.h"
#include "cxxtools/serializationinfo.h"
#include <sstream>
#include <cctype>

namespace cxxtools
{

InvalidTime::InvalidTime(const SourceInfo& si)
: std::invalid_argument("Invalid time" + si)
{ }


inline unsigned short getNumber2(const char* s)
{
    if ( ! std::isdigit(s[0]) || ! std::isdigit(s[1]) )
        throw ConversionError( CXXTOOLS_ERROR_MSG("Invalid Time format") );

    return (s[0] - '0') * 10 + (s[1] - '0');
}


inline unsigned short getNumber3(const char* s)
{
    if( ! std::isdigit(s[0]) || ! std::isdigit(s[1]) || ! std::isdigit(s[2]) )
       throw ConversionError( CXXTOOLS_ERROR_MSG("Invalid Time format") );

    return ( s[0] - '0') * 100 + (s[1] - '0') * 10 + (s[2] - '0' );
}


void convert(Time& time, const std::string& s)
{
    unsigned hour = 0, min = 0, sec = 0, msec = 0;

    if( s.size() < 11 || s.at(2) != ':' || s.at(5) != ':' || s.at(8) != '.')
        throw ConversionError( CXXTOOLS_ERROR_MSG("Invalid Time format") );

	const char* d = s.data();
	hour = getNumber2(d);
	min = getNumber2(d + 3);
	sec = getNumber2(d + 6);
	msec = getNumber3(d + 9);

	time.set(hour, min, sec, msec);
}


void convert(std::string& str, const Time& time)
{
    unsigned hour = 0, minute = 0, second = 0, msec = 0;
    time.get(hour, minute, second, msec);

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

    str.assign(ret, 12);
}

void operator >>=(const SerializationInfo& si, Time& time)
{
    if (si.category() == cxxtools::SerializationInfo::Object)
    {
        unsigned short hour, min, sec, msec;

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

        time.set(hour, min, sec, msec);
    }
    else
    {
        std::string s;
        si.getValue(s);
        convert(time, s);
    }
}


void operator <<=(SerializationInfo& si, const Time& time)
{
    std::string s;
    convert(s, time);
    si.setValue(s);
    si.setTypeName("Date");
}

} // namespace cxxtools
