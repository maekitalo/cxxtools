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
#include "cxxtools/clock.h"
#include "cxxtools/serializationinfo.h"
#include "dateutils.h"
#include <stdexcept>
#include <cctype>

namespace cxxtools
{

static void throwInvalidDate(const std::string& str, std::string::const_iterator p, const std::string& fmt)
{
    throw InvalidDate("string <" + std::string(str.begin(), p) + "(*)" + std::string(p, str.end()) + "> does not match datetime format <" + fmt + '>');
}

DateTime::DateTime(const std::string& str, const std::string& fmt)
{
  unsigned year = 0;
  unsigned month = 1;
  unsigned day = 1;
  unsigned hours = 0;
  unsigned minutes = 0;
  unsigned seconds = 0;
  unsigned useconds = 0;
  bool am = true;

  enum {
    state_0,
    state_fmt,
    state_two
  } state = state_0;

  std::string::const_iterator dit = str.begin();
  try
  {
    std::string::const_iterator it;
    for (it = fmt.begin(); it != fmt.end(); ++it)
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
            else if (ch == '#')
              skipWord(dit, str.end());
            else if (dit == str.end() || (*dit != ch && ch != '?'))
              throwInvalidDate(str, dit, fmt);
            else
              ++dit;
          }
          break;

        case state_fmt:
          state = state_0;
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

            case 'O':
              month = getMonthFromName(dit, str.end());
              break;

            case 'd':
              day = getUnsigned(dit, str.end(), 2);
              break;

            case 'H':
            case 'I':
              hours = getUnsigned(dit, str.end(), 2);
              break;

            case 'M':
              minutes = getUnsigned(dit, str.end(), 2);
              break;

            case 'S':
              seconds = getUnsigned(dit, str.end(), 2);
              break;

            case 'j':
              if (dit != str.end() && *dit == '.')
                ++dit;
              useconds = getMicroseconds(dit, str.end(), 6);
              break;

            case 'J':
            case 'U':
              if (dit != str.end() && *dit == '.')
              {
                ++dit;
                useconds = getMicroseconds(dit, str.end(), 6);
              }
              break;

            case 'K':
              if (dit != str.end() && *dit == '.')
              {
                ++dit;
                useconds = getMicroseconds(dit, str.end(), 3);
              }
              break;

            case 'k':
              useconds = getMicroseconds(dit, str.end(), 3);
              break;

            case 'u':
              useconds = getMicroseconds(dit, str.end(), 6);
              break;

            case 'p':
              if (dit == str.end()
                || dit + 1 == str.end()
                || ((*dit != 'A'
                  && *dit != 'a'
                  && *dit != 'P'
                  && *dit != 'p')
                || (*(dit + 1) != 'M'
                  &&  *(dit + 1) != 'm')))
              {
                throwInvalidDate(str, dit, fmt);
              }

              am = (*dit == 'A' || *dit == 'a');
              dit += 2;
              break;

            case '2':
              state = state_two;
              break;

            default:
              throw InvalidDate("invalid datetime format <" + fmt + '>');
          }

          break;

        case state_two:
          state = state_0;
          switch (ch)
          {
            case 'm': month = getUnsignedF(dit, str.end(), 2); break;
            case 'd': day = getUnsignedF(dit, str.end(), 2); break;
            case 'H':
            case 'I': hours = getUnsignedF(dit, str.end(), 2); break;
            case 'M': minutes = getUnsignedF(dit, str.end(), 2); break;
            case 'S': seconds = getUnsignedF(dit, str.end(), 2); break;
            default:
              throw InvalidDate("invalid datetime format <" + fmt + '>');
          }
      }
    }

    if (it != fmt.end() || dit != str.end())
      throwInvalidDate(str, dit, fmt);

    set(year, month, day, am ? hours : hours + 12, minutes, seconds, 0, useconds);
  }
  catch (const std::invalid_argument&)
  {
    throwInvalidDate(str, dit, fmt);
  }
}

UtcDateTime DateTime::fromMSecsSinceEpoch(cxxtools::Milliseconds sinceEpoch)
{
    static const LocalDateTime dt(1970, 1, 1, 0, 0, 0);
    return UtcDateTime(dt + sinceEpoch);
}

UtcDateTime DateTime::gmtime()
{
    return Clock::getSystemTime();
}

LocalDateTime DateTime::localtime()
{
    return Clock::getLocalTime();
}

Milliseconds DateTime::msecsSinceEpoch() const
{
    static const DateTime dt(1970, 1, 1, 0, 0, 0);
    return *this - dt;
}

std::string DateTime::toString(const std::string& fmt) const
{
  int year;
  unsigned month, day, hours, minutes, seconds, mseconds, useconds;

  get(year, month, day, hours, minutes, seconds, mseconds, useconds);

  std::string str;

  enum {
    state_0,
    state_fmt,
    state_one
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
        if (*it != '%')
          state = state_0;

        switch (*it)
        {
          case 'Y': appendDn(str, 4, year); break;
          case 'y': appendDn(str, 2, year % 100); break;
          case 'm': appendDn(str, 2, month); break;
          case 'O': str += monthnames[month-1]; break;
          case 'd': appendDn(str, 2, day); break;
          case 'w': appendDn(str, 1, dayOfWeek()); break;
          case 'W': { int dow = dayOfWeek(); appendDn(str, 1, dow == 0 ? 7 : dow); } break;
          case 'N': str += weekdaynames[dayOfWeek()]; break;
          case 'H': appendDn(str, 2, hours); break;
          case 'I': appendDn(str, 2, hours % 12); break;
          case 'M': appendDn(str, 2, minutes); break;
          case 'S': appendDn(str, 2, seconds); break;
          case 'j': if (useconds != 0)
                    {
                      str += '.';
                      str += (useconds / 100000 + '0');
                      useconds %= 100000;
                      for (unsigned e = 10000; e > 0 && useconds > 0; e /= 10)
                      {
                        str += (useconds / e + '0');
                        useconds %= e;
                      }
                    }
                    break;

          case 'J': str += '.';
                    str += (useconds / 100000 + '0');
                    useconds %= 100000;
                    for (unsigned e = 10000; e > 0 && useconds > 0; e /= 10)
                    {
                      str += (useconds / e + '0');
                      useconds %= e;
                    }
                    break;

          case 'k': appendDn(str, 3, mseconds);
                    break;

          case 'K': str += '.';
                    appendDn(str, 3, mseconds);
                    break;

          case 'u': appendDn(str, 6, useconds);
                    break;

          case 'U': str += '.';
                    appendDn(str, 6, useconds);
                    break;

          case 'p': str += (hours < 12 ? "am" : "pm"); break;
          case 'P': str += (hours < 12 ? "AM" : "PM"); break;

          case '1': state = state_one; break;

          default:
            str += '%';
            str += *it;
        }

        break;

      case state_one:
        state = state_0;
        switch (*it)
        {
          case 'd': appendDn(str, day < 10 ? 1 : 2, day); break;
          case 'm': appendDn(str, month < 10 ? 1 : 2, month); break;
          case 'H': appendDn(str, hours < 10 ? 1 : 2, hours); break;
          case 'I': appendDn(str, hours%12 < 10 ? 1 : 2, hours%12); break;
          case 'M': appendDn(str, minutes < 10 ? 1 : 2, minutes); break;
          case 'S': appendDn(str, seconds < 10 ? 1 : 2, seconds); break;

          default:  str += "%1";
                    str += *it;
                    if (*it == '%')
                      state = state_fmt;
                    break;
        }

        break;
    }
  }

  if (state == state_fmt)
    str += '%';

  return str;
}


DateTime& DateTime::operator+=(const Timespan& ts)
{
    int64_t totalUSecs = ts.totalUSecs();
    int64_t days = totalUSecs / static_cast<int64_t>(Time::USecsPerDay);
    int64_t overrun = totalUSecs % static_cast<int64_t>(Time::USecsPerDay);

    if ((-overrun) > static_cast<int64_t>(_time.totalUSecs()) )
    {
        days -= 1;
        overrun += Time::USecsPerDay;
    }
    else if (overrun + _time.totalUSecs() >= Time::USecsPerDay)
    {
        days += 1;
        overrun -= Time::USecsPerDay;
    }

    _date += static_cast<int>(days);
    _time += Microseconds(overrun);
    return *this;
}

Timespan operator-(const DateTime& first, const DateTime& second)
{
    int64_t dayDiff      = int64_t( first.date().julian() ) -
                               int64_t( second.date().julian() );

    int64_t microSecDiff = int64_t( first.time().totalUSecs() ) -
                               int64_t( second.time().totalUSecs() );

    int64_t result = (dayDiff * Time::USecsPerDay + microSecDiff);

    return Timespan(result);
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
        datetime = DateTime(s);
    }
}

void operator <<=(SerializationInfo& si, const DateTime& dt)
{
    si.setValue(dt.toString());
    si.setTypeName("DateTime");
}


}
