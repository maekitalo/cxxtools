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
#include "cxxtools/time.h"
#include "cxxtools/convert.h"
// #include "cxxtools/SerializationInfo.h" coming soon
#include <sstream>
#include <cctype>

namespace cxxtools {

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

/*
void operator >>=(const SerializationInfo& si, Time& time)
{
    std::string s = si.toValue<std::string>();
    convert(time, s);;
}


void operator <<=(SerializationInfo& si, const Time& time)
{
    std::string s;
    convert(s, time);
    si.setValue(s);
    si.setTypeName("Date");
}
*/
} // namespace cxxtools
