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
//#include "cxxtools/serializationinfo.h" // coming soon
#include <algorithm>
#include <sstream>
#include <stdexcept>
#include <cctype>
#include <cmath>
#include <cassert>

namespace cxxtools {

inline unsigned short getNumber2(const char* s)
{
    if( ! std::isdigit(s[0]) || !std::isdigit(s[1]) )
        throw ConversionError( CXXTOOLS_ERROR_MSG("Invalid DateTime format") );

    return (s[0] - '0') * 10 + (s[1] - '0');
}

inline unsigned short getNumber3(const char* s)
{
    if (!std::isdigit(s[0]) || !std::isdigit(s[1]) || !std::isdigit(s[2]))
        throw ConversionError( CXXTOOLS_ERROR_MSG("Invalid DateTime format") );

    return (s[0] - '0') * 100
        + (s[1] - '0') * 10
        + (s[2] - '0');
}


inline unsigned short getNumber4(const char* s)
{
    if( ! std::isdigit(s[0]) || ! std::isdigit(s[1]) ||
        ! std::isdigit(s[2]) || ! std::isdigit(s[3]) )
        throw ConversionError( CXXTOOLS_ERROR_MSG("Invalid DateTime format") );

    return (s[0] - '0') * 1000
        + (s[1] - '0') * 100
        + (s[2] - '0') * 10
        + (s[3] - '0');
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
    // format YYYY-MM-DD hh:mm:ss.sssss
    //        0....+....1....+....2....+
    char ret[25];
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

/*
void operator >>=(const SerializationInfo& si, DateTime& datetime)
{
    std::string s = si.toValue<std::string>();
    convert(datetime, s);

    //Date date(1,1,1);
    //si.getMember("date") >>= date;
    //datetime.setDate(date);

    //Time time;
    //si.getMember("time") >>= time;
    //datetime.setTime(time);
}


void operator <<=(SerializationInfo& si, const DateTime& datetime)
{
    std::string s;
    convert(s, datetime);
    si.setValue(s);
    si.setTypeName( "DateTime");

    //si.setTypeName("DateTime");
    //SerializationInfo& date = si.addMember("date");

    //date <<= datetime.date();
    //SerializationInfo& time  = si.addMember("time");
    //time <<= datetime.time();
}
*/

}
