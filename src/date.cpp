/*
 * Copyright (C) 2004-2008 Marc Boris Duerner
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
#include "cxxtools/date.h"
#include "cxxtools/convert.h"
#include "cxxtools/serializationinfo.h"
#include "dateutils.h"
#include <cctype>

namespace cxxtools
{

InvalidDate::InvalidDate()
: std::invalid_argument("Invalid date")
{
}


void greg2jul(unsigned& jd, int y, int m, int d)
{
    if( ! Date::isValid(y, m, d) )
    {
        throw InvalidDate();
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



Date::Date(const std::string& str, const std::string& fmt)
{
  unsigned year = 0;
  unsigned month = 0;
  unsigned day = 0;

  enum {
    state_0,
    state_fmt
  } state = state_0;

  std::string::const_iterator dit = str.begin();
  std::string::const_iterator it;
  for (it = fmt.begin(); it != fmt.end() && dit != str.end(); ++it)
  {
    char ch = *it;
    switch (state)
    {
      case state_0:
        if (ch == '%')
          state = state_fmt;
        else
        {
          if (ch == '*')
            skipNonDigit(dit, str.end());
          else if (*dit != ch && ch != '?')
            throw std::runtime_error("string <" + str + "> does not match date format <" + fmt + '>');
          else
            ++dit;
        }
        break;

      case state_fmt:
        switch (ch)
        {
          case 'Y':
            year = getInt(dit, str.end(), 4);
            break;

          case 'y':
            year = getInt(dit, str.end(), 2);
            year += (year < 50 ? 2000 : 1900);
            break;

          case 'm':
            month = getUnsigned(dit, str.end(), 2);
            break;

          case 'd':
            day = getUnsigned(dit, str.end(), 2);
            break;

          default:
            throw std::runtime_error("invalid date format <" + fmt + '>');
        }

        state = state_0;
        break;
    }
  }

  if (it != fmt.end() || dit != str.end())
    throw std::runtime_error("string <" + str + "> does not match date format <" + fmt + '>');

  set(year, month, day);
}

std::string Date::toString(const std::string& fmt) const
{
  int year;
  unsigned month;
  unsigned day;

  get(year, month, day);

  std::string str;

  enum {
    state_0,
    state_fmt
  } state = state_0;

  for (std::string::const_iterator it = fmt.begin(); it != fmt.end(); ++it)
  {
    switch (state)
    {
      case state_0:
        if (*it == '%')
          state = state_fmt;
        else
          str += *it;
        break;

      case state_fmt:
        switch (*it)
        {
          case 'Y': appendDn(str, 4, year); break;
          case 'y': appendDn(str, 2, year % 100); break;
          case 'm': appendDn(str, 2, month); break;
          case 'd': appendDn(str, 2, day); break;
          case 'w': appendDn(str, 1, dayOfWeek()); break;
          case 'W': { int dow = dayOfWeek(); appendDn(str, 1, dow == 0 ? 7 : dow); } break;
          default:
            str += '%';
        }

        if (*it != '%')
          state = state_0;
        break;
    }
  }

  if (state == state_fmt)
    str += '%';

  return str;
}

void operator>>=(const SerializationInfo& si, Date& date)
{
    if (si.category() == cxxtools::SerializationInfo::Object)
    {
        unsigned short year, month, day;
        si.getMember("year") >>= year;
        si.getMember("month") >>= month;
        si.getMember("day") >>= day;
        date.set(year, month, day);
    }
    else
    {
        std::string s;
        si.getValue(s);
        date = Date(s);
    }
}

void operator<<=(SerializationInfo& si, const Date& date)
{
    si.setValue(date.toString());
    si.setTypeName("Date");
}

}
