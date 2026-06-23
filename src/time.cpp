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
#include "cxxtools/serializationinfo.h"
#include "dateutils.h"
#include <stdexcept>
#include <sstream>
#include <cctype>

namespace cxxtools
{

static void throwInvalidTime(const std::string& str, std::string::const_iterator p, const std::string& fmt)
{
    throw InvalidTime("string <" + std::string(str.begin(), p) + "(*)" + std::string(p, str.end()) + "> does not match time format <" + fmt + '>');
}

InvalidTime::InvalidTime()
: std::invalid_argument("Invalid time")
{ }

Time::Time(const std::string& str, const std::string& fmt)
: _usecs(0)
{
  unsigned hours = 0;
  unsigned minutes = 0;
  unsigned seconds = 0;
  unsigned useconds = 0;
  bool am = true;

  enum class State {
    null,
    format,
    two
  } state = State::null;

  std::string::const_iterator dit = str.begin();
  try
  {
    for (auto it = fmt.begin(); it != fmt.end(); ++it)
    {
      char ch = *it;
      switch (state)
      {
        case State::null:
          if (ch == '%')
            state = State::format;
          else
          {
            if (ch == '*')
              skipNonDigit(dit, str.end());
            else if (ch == '#')
              skipWord(dit, str.end());
            else if (dit == str.end() || (*dit != ch && ch != '?'))
              throwInvalidTime(str, dit, fmt);
            else
              ++dit;
          }
          break;

        case State::format:
          state = State::null;
          switch (ch)
          {
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
                throwInvalidTime(str, dit, fmt);
              }

              am = (*dit == 'A' || *dit == 'a');
              dit += 2;
              break;

            case '2':
              state = State::two;
              break;

            default:
              throw InvalidTime("invalid time format <" + fmt + '>');
          }

          break;

        case State::two:
          state = State::null;
          switch (ch)
          {
            case 'H':
            case 'I': hours = getUnsignedF(dit, str.end(), 2); break;
            case 'M': minutes = getUnsignedF(dit, str.end(), 2); break;
            case 'S': seconds = getUnsignedF(dit, str.end(), 2); break;
            default:
              throw InvalidTime("invalid time format <" + fmt + '>');
          }

      }
    }

    if (state != State::null || dit != str.end())
      throwInvalidTime(str, dit, fmt);

    unsigned normalized_hours = hours;
    if (am && hours == 12)
        normalized_hours = 0;       // 12 AM → midnight
    else if (!am && hours != 12)
        normalized_hours = hours + 12;  // 1–11 PM → 13–23
    set(normalized_hours, minutes, seconds, 0, useconds);
  }
  catch (const std::invalid_argument&)
  {
    throwInvalidTime(str, dit, fmt);
  }
}

std::string Time::toString(const std::string& fmt) const
{
  unsigned hours, minutes, seconds, mseconds, useconds;

  get(hours, minutes, seconds, mseconds, useconds);

  std::string str;

  enum class State {
    null,
    format,
    one
  } state = State::null;

  for (std::string::const_iterator it = fmt.begin(); it != fmt.end(); ++it)
  {
    switch (state)
    {
      case State::null:
        if (*it == '%')
          state = State::format;
        else
          str += *it;
        break;

      case State::format:
        if (*it != '%')
          state = State::null;

        switch (*it)
        {
          case 'H': appendDn(str, 2, hours); break;
          case 'I': appendDn(str, 2, hours % 12); break;
          case 'M': appendDn(str, 2, minutes); break;
          case 'S': appendDn(str, 2, seconds); break;
          case 'j': if (useconds != 0)
                    {
                      auto u = useconds;
                      str += '.';
                      str += (u / 100000 + '0');
                      u %= 100000;
                      for (unsigned e = 10000; e > 0 && u > 0; e /= 10)
                      {
                        str += (u / e + '0');
                        u %= e;
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

          case '1': state = State::one; break;

          default:
            str += '%';
            str += *it;
        }

        break;

      case State::one:
        state = State::null;
        switch (*it)
        {
          case 'H': appendDn(str, hours < 10 ? 1 : 2, hours); break;
          case 'I': appendDn(str, hours%12 < 10 ? 1 : 2, hours%12); break;
          case 'M': appendDn(str, minutes < 10 ? 1 : 2, minutes); break;
          case 'S': appendDn(str, seconds < 10 ? 1 : 2, seconds); break;

          default:  str += "%1";
                    str += *it;
                    if (*it == '%')
                      state = State::format;
                    break;
        }

        break;
    }
  }

  if (state == State::format)
    str += '%';

  return str;
}

Time& Time::operator+=(const Timespan& ts)
{
    auto microsecs = _usecs + ts.totalUSecs();
    if (microsecs < 0 || microsecs > USecsPerDay)
    {
        std::ostringstream s;
        s << "cannot add " << Microseconds(ts) << " to " << toString();
        throw std::overflow_error(s.str());
    }

    _usecs = static_cast<uint64_t>(microsecs);
    return *this;
}

Time& Time::operator-=(const Timespan& ts)
{
    auto microsecs = _usecs + ts.totalUSecs();
    if (microsecs < 0 || microsecs > USecsPerDay)
    {
        std::ostringstream s;
        s << "cannot subtract " << Microseconds(ts) << " from " << toString();
        throw std::overflow_error(s.str());
    }

    _usecs = static_cast<uint64_t>(microsecs);
    return *this;
}

void operator >>=(const SerializationInfo& si, Time& time)
{
    if (si.category() == cxxtools::SerializationInfo::Object)
    {
        unsigned short hour, min, sec, msec, usec;

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

        if ((p = si.findMember("microsecond")) != 0
            || (p = si.findMember("usec")) != 0)
            *p >>= usec;
        else
            usec = 0;

        time.set(hour, min, sec, msec, usec);
    }
    else
    {
        std::string s;
        si.getValue(s);
        time = Time(s);
    }
}

void operator <<=(SerializationInfo& si, const Time& time)
{
    si.setValue(time.toString());
    si.setTypeName("Time");
}

} // namespace cxxtools
