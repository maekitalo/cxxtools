/***************************************************************************
 *   Copyright (C) 2004-2008 Marc Boris Duerner                            *
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
#include "cxxtools/date.h"
#include "cxxtools/convert.h"
//#include "cxxtools/serializationinfo.h" // coming soon
#include <cctype>

namespace cxxtools {

InvalidDate::InvalidDate(const SourceInfo& si)
: std::invalid_argument("Invalid date" + si)
{
}


void greg2jul(unsigned& jd, int y, int m, int d)
{
    if( ! Date::isValid(y, m, d) )
    {
        throw InvalidDate(CXXTOOLS_SOURCEINFO);
    }

    jd=(1461*(y+4800+(m-14)/12))/4+(367*(m-2-12*((m-14)/12)))/12-(3*((y+4900+(m-14)/12)/100))/4+d-32075;
}


void jul2greg(unsigned jd, int& y, int& m, int& d)
{
  register int l,n,i,j;
  l=jd+68569;
  n=(4*l)/146097;
  l=l-(146097*n+3)/4;
  i=(4000*(l+1))/1461001;
  l=l-(1461*i)/4+31;
  j=(80*l)/2447;
  d=l-(2447*j)/80;
  l=j/11;
  m=j+2-(12*l);
  y=100*(n-49)+i+l;
}


inline unsigned short getNumber2(const char* s)
{
    if (!std::isdigit(s[0]) || !std::isdigit(s[1]))
    throw ConversionError( CXXTOOLS_ERROR_MSG("Invalid date format.") );
    return (s[0] - '0') * 10
        + (s[1] - '0');
}


inline unsigned short getNumber4(const char* s)
{
    if ( ! std::isdigit(s[0]) || !std::isdigit(s[1])
        || !std::isdigit(s[2])  || !std::isdigit(s[3]))
    throw ConversionError( CXXTOOLS_ERROR_MSG("Invalid date format.") );

    return (s[0] - '0') * 1000
        + (s[1] - '0') * 100
        + (s[2] - '0') * 10
        + (s[3] - '0');
}


void convert(std::string& str, const Date& date)
{
    // format YYYY-MM-DD
    //        0....+....1

    int year, month, day;
    jul2greg(date.julian(), year, month, day);

    char ret[10];
    unsigned short n = year;

    ret[3] = '0' + n % 10;
    n /= 10;
    ret[2] = '0' + n % 10;
    n /= 10;
    ret[1] = '0' + n % 10;
    n /= 10;
    ret[0] = '0' + n % 10;
    ret[4] = '-';
    ret[5] = '0' + month / 10;
    ret[6] = '0' + month % 10;
    ret[7] = '-';
    ret[8] = '0' + day / 10;
    ret[9] = '0' + day % 10;

    str.assign(ret, 10);
}


void convert(Date& date, const std::string& s)
{
    if (s.size() < 10 || s.at(4) != '-' || s.at(7) != '-')
        throw ConversionError( CXXTOOLS_ERROR_MSG("Illegal date format") );

    const char* d = s.data();
    date= Date(getNumber4(d), getNumber2(d + 5), getNumber2(d + 8));
}


/*
void operator>>=(const SerializationInfo& si, Date& date)
{
    std::string s = si.toValue<std::string>();
    convert(date, s);
}


void operator<<=(SerializationInfo& si, const Date& date)
{
    std::string s;
    convert(s, date);
    si.setValue(s);
    si.setTypeName("Date");
}
*/
}

