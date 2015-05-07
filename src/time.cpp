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
#include <cctype>

namespace cxxtools
{

InvalidTime::InvalidTime()
: std::invalid_argument("Invalid time")
{ }

Time::Time(const std::string& str, const std::string& fmt)
: _msecs(0)
{
  unsigned hours = 0;
  unsigned minutes = 0;
  unsigned seconds = 0;
  unsigned mseconds = 0;
  bool am = true;

  enum {
    state_0,
    state_fmt
  } state = state_0;

  std::string::const_iterator dit = str.begin();
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
          else if (dit == str.end() || (*dit != ch && ch != '?'))
            throw std::runtime_error("string <" + str + "> does not match time format <" + fmt + '>');
          else
            ++dit;
        }
        break;

      case state_fmt:
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
            mseconds = getMilliseconds(dit, str.end());
            break;

          case 'J':
          case 'K':
            if (dit != str.end() && *dit == '.')
            {
              ++dit;
              mseconds = getMilliseconds(dit, str.end());
            }
            break;

          case 'k':
            mseconds = getMilliseconds(dit, str.end());
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
                throw std::runtime_error("string <" + str + "> does not match time format <" + fmt + '>');
            }

            am = (*dit == 'A' || *dit == 'a');
            dit += 2;
            break;

          default:
            throw std::runtime_error("invalid time format <" + fmt + '>');
        }

        state = state_0;
        break;
    }
  }

  if (it != fmt.end() || dit != str.end())
    throw std::runtime_error("string <" + str + "> does not match time format <" + fmt + '>');

  set(am ? hours : hours + 12, minutes, seconds, mseconds);
}

std::string Time::toString(const std::string& fmt) const
{
  unsigned hours, minutes, seconds, mseconds;

  get(hours, minutes, seconds, mseconds);

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
          case 'H': appendD2(str, hours); break;
          case 'I': appendD2(str, hours % 12); break;
          case 'M': appendD2(str, minutes); break;
          case 'S': appendD2(str, seconds); break;
          case 'j': if (mseconds != 0)
                    {
                      str += '.';
                      str += (mseconds / 100 + '0');
                      if (mseconds % 100 != 0)
                      {
                        str += (mseconds / 10 % 10 + '0');
                        if (mseconds % 10 != 0)
                          str += (mseconds % 10 + '0');
                      }
                    }
                    break;

          case 'J': str += '.';
                    str += (mseconds / 100 + '0');
                    if (mseconds % 100 != 0)
                    {
                      str += (mseconds / 10 % 10 + '0');
                      if (mseconds % 10 != 0)
                        str += (mseconds % 10 + '0');
                    }
                    break;

          case 'k': appendD3(str, mseconds);
                    break;

          case 'K': str += '.';
                    appendD3(str, mseconds);
                    break;

          case 'p': str += (hours < 12 ? "am" : "pm"); break;
          case 'P': str += (hours < 12 ? "AM" : "PM"); break;
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
        time = Time(s);
    }
}

void operator <<=(SerializationInfo& si, const Time& time)
{
    si.setValue(time.toString());
    si.setTypeName("Time");
}

} // namespace cxxtools
