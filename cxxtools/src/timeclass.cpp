/* timeclass.cpp
   Copyright (C) 2003 Tommi Maekitalo

This file is part of cxxtools.

Cxxtools is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

Cxxtools is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Cxxtools; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330,
Boston, MA  02111-1307  USA
*/

#include "cxxtools/timeclass.h"
#include <iomanip>
#include <stdio.h>
#if __GNUC__ <= 2
# include <strstream>
#else
#endif
# include <sstream>
#include <stdexcept>

using namespace std;

////////////////////////////////////////////////////////////////////////
// TimeClass
//
TimeClass::TimeClass(double t)
{
  tv.tv_sec = (long)t;
  tv.tv_usec = (long)((t - tv.tv_sec) * 1e6);
}

TimeClass::TimeClass(std::string s)
{
#if __GNUC__ <= 2
  istrstream ss(s.c_str());
#else
  istringstream ss(s);
#endif
  ss >> *this;

  if (!ss)
    throw runtime_error("error extracting time from string");
}

TimeClass& TimeClass::setNow()
{
  gettimeofday(&tv, 0);
  tv.tv_sec %= 24*60*60;
  return *this;
}

ostream& operator<< (ostream& out, const TimeClass& t)
{
  unsigned h, m, s;
  char buffer[20];

  h = t.tv.tv_sec / 3600;
  m = (t.tv.tv_sec - h * 3600) / 60;
  s = t.tv.tv_sec - h * 3600 - m * 60;

  sprintf(buffer, "%02u:%02u:%02u.%06u", h, m, s, (unsigned)t.tv.tv_usec);
  return out << buffer;
}

istream& operator>> (istream& in, TimeClass& t)
{
  long h;
  unsigned m, s, u;
  long ls;
  int ch;

  // lese Stunde
  in >> h;
  ch = in.peek();

  if (ch == ':')
  {
    // lese Minute

    in.get();  // ':' überspringen

    in >> m;
    ch = in.peek();
    if (in && ch != ':')
    {
      s  = 0;
      ls = 0;
      ch = '\0';
    }
    else
    {
      in.get();

      // lese Sekunde
      in >> s;
      ch = in.peek();

      ls = (long)s;
    }
  }
  else
  {
    ls = (long)h;
    m  = 0;
    s  = 0;
    h  = 0;
  }

  u = 0;
  if (in && ch == '.')
  {
    // lese Mikrosekunde
    in.get();
    unsigned f = (unsigned)1e5;
    while(in && f > 0)
    {
      ch = in.peek();
      if (isdigit(ch))
      {
        in.get();
        u += (ch - '0') * f;
        f /= 10;
      }
      else
      {
        f = 0;
      }
    }

    if (isdigit(in.peek()) && in.get() >= '5')
    {
      if(++u >= 1e6)
      {
        u = 0;
        ++ls;
      }
    }

    while (isdigit(in.peek()))
      in.get();
  }

  if (!in.fail())
  {
    t.tv.tv_sec = h * 3600 + m * 60 + ls;
    t.tv.tv_usec = u;
  }

  return in;
}

TimeClass TimeClass::operator- (const TimeClass& t) const
{
  TimeClass ret(*this);
  ret -= t;

  return ret;
}

TimeClass& TimeClass::operator-= (const TimeClass& t)
{
  tv.tv_sec -= t.tv.tv_sec;
  if (tv.tv_usec >= t.tv.tv_usec)
    tv.tv_usec -= t.tv.tv_usec;
  else
  {
    tv.tv_usec = tv.tv_usec + (long)1e6 - t.tv.tv_usec;
    --tv.tv_sec;
  }

  return *this;
}

TimeClass TimeClass::operator+ (const TimeClass& t) const
{
  TimeClass ret(*this);
  ret += t;
  return ret;
}

TimeClass& TimeClass::operator+= (const TimeClass& t)
{
  tv.tv_sec += t.tv.tv_sec;
  tv.tv_usec += t.tv.tv_usec;
  if (tv.tv_usec >= (long)1e6)
  {
    ++tv.tv_sec;
    tv.tv_usec -= (long)1e6;
  }
  return *this;
}

string TimeClass::Hms() const
{
  char buffer[9];
  Hms(buffer);
  return buffer;
}

void TimeClass::Hms(char* buffer) const
{
  long sec = tv.tv_sec;
  long std = sec / 3600;
  sec %= 3600;
  long min = sec / 60;
  sec %= 60;
  buffer[0] = (char)(std / 10) + '0';
  buffer[1] = (char)(std % 10) + '0';
  buffer[2] = ':';
  buffer[3] = (char)(min / 10) + '0';
  buffer[4] = (char)(min % 10) + '0';
  buffer[5] = ':';
  buffer[6] = (char)(sec / 10) + '0';
  buffer[7] = (char)(sec % 10) + '0';
  buffer[8] = '\0';
}

string TimeClass::Hmsh() const
{
  char buffer[12];
  Hmsh(buffer);
  return buffer;
}

void TimeClass::Hmsh(char* buffer) const
{
  long sec = tv.tv_sec;
  long std = sec / 3600;
  sec %= 3600;
  long min = sec / 60;
  sec %= 60;
  long hsec = tv.tv_usec / (long)1e4;

  buffer[0] = (char)(std / 10) + '0';
  buffer[1] = (char)(std % 10) + '0';
  buffer[2] = ':';
  buffer[3] = (char)(min / 10) + '0';
  buffer[4] = (char)(min % 10) + '0';
  buffer[5] = ':';
  buffer[6] = (char)(sec / 10) + '0';
  buffer[7] = (char)(sec % 10) + '0';
  buffer[8] = ',';
  buffer[9] = (char)(hsec / 10) + '0';
  buffer[10] = (char)(hsec % 10) + '0';
  buffer[11] = '\0';
}

string TimeClass::hms() const
{
  char buffer[7];
  hms(buffer);
  return buffer;
}

void TimeClass::hms(char* buffer) const
{
  long sec = tv.tv_sec;
  long std = sec / 3600;
  sec %= 3600;
  long min = sec / 60;
  sec %= 60;
  buffer[0] = (char)(std / 10) + '0';
  buffer[1] = (char)(std % 10) + '0';
  buffer[2] = (char)(min / 10) + '0';
  buffer[3] = (char)(min % 10) + '0';
  buffer[4] = (char)(sec / 10) + '0';
  buffer[5] = (char)(sec % 10) + '0';
  buffer[6] = '\0';
}

string TimeClass::hmsh() const
{
  char buffer[9];
  hmsh(buffer);
  return buffer;
}

void TimeClass::hmsh(char* buffer) const
{
  long sec = tv.tv_sec;
  long std = sec / 3600;
  sec %= 3600;
  long min = sec / 60;
  sec %= 60;
  long hsec = tv.tv_usec / (long)1e4;

  buffer[0] = (char)(std / 10) + '0';
  buffer[1] = (char)(std % 10) + '0';
  buffer[2] = (char)(min / 10) + '0';
  buffer[3] = (char)(min % 10) + '0';
  buffer[4] = (char)(sec / 10) + '0';
  buffer[5] = (char)(sec % 10) + '0';
  buffer[6] = (char)(hsec / 10) + '0';
  buffer[7] = (char)(hsec % 10) + '0';
  buffer[8] = '\0';
}
