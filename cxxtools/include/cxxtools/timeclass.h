/* cxxtools/timeclass.h
   Copyright (C) 2003 Tommi Mäkitalo

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
along with cxxtools; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330,
Boston, MA  02111-1307  USA
*/

#ifndef TIMECLASS_H
#define TIMECLASS_H

#include <sys/time.h>
#include <iostream>
#include <string>

////////////////////////////////////////////////////////////////////////
// TimeClass
//
class TimeClass
{
    friend std::ostream& operator<< (std::ostream& out, const TimeClass& t);
    friend std::istream& operator>> (std::istream& in, TimeClass& t);

  public:
    TimeClass()
    { }
    TimeClass(double t);
    TimeClass(long sec, long usec)
    {
      tv.tv_sec = sec;
      tv.tv_usec = usec;
    }
    TimeClass(std::string s);

    operator double() const
    { return (double)tv.tv_sec + (double)tv.tv_usec / 1e6; }

    TimeClass& setNow();
    static TimeClass Now()
    { TimeClass t; t.setNow(); return t; }

    bool operator<  (const TimeClass& t) const
    { return tv.tv_sec < t.tv.tv_sec
          || tv.tv_sec == t.tv.tv_sec && tv.tv_usec < t.tv.tv_usec; }
    bool operator<= (const TimeClass& t) const
    { return tv.tv_sec < t.tv.tv_sec
          || tv.tv_sec == t.tv.tv_sec && tv.tv_usec <= t.tv.tv_usec; }
    bool operator== (const TimeClass& t) const
    { return tv.tv_sec == t.tv.tv_sec && tv.tv_usec == t.tv.tv_usec; }

    TimeClass  operator-  (const TimeClass& t) const;
    TimeClass& operator-= (const TimeClass& t);
    TimeClass  operator+  (const TimeClass& t) const;
    TimeClass& operator+= (const TimeClass& t);

    timeval*    getTimevalPtr() { return &tv; }
    int         getMs() const   { return tv.tv_sec * 1000 + tv.tv_usec / 1000; }
    std::string Hms() const;
    void        Hms(char* buffer) const;  // Format: hh:mm:ss
    std::string Hmsh() const;
    void        Hmsh(char* buffer) const;  // Format: hh:mm:ss,hh

    std::string hms() const;
    void        hms(char* buffer) const;  // Format: hhmmss
    std::string hmsh() const;
    void        hmsh(char* buffer) const;  // Format: hhmmsshh

  private:
    struct timeval tv;
};

std::ostream& operator<< (std::ostream& out, const TimeClass& t);
std::istream& operator>> (std::istream& in, TimeClass& t);

#endif

