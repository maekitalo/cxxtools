/* cxxtools/hirestime.h
 *
 * cxxtools - general purpose C++-toolbox
 * Copyright (C) 2005 Tommi Maekitalo
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#ifndef CXXTOOLS_HIRESTIME_H
#define CXXTOOLS_HIRESTIME_H

#include <sys/time.h>
#include <iosfwd>

namespace cxxtools
{
  class HiresTime
  {
      timeval tv;

    public:
      HiresTime()
      {
        tv.tv_sec = 0;
        tv.tv_usec = 0;
      }

      HiresTime(const timeval& tv_)
      {
        tv.tv_sec = tv_.tv_sec;
        tv.tv_usec = tv_.tv_usec;
      }

      HiresTime(time_t tv_sec, suseconds_t tv_usec)
      {
        tv.tv_sec = tv_sec;
        tv.tv_usec = tv_usec;
      }

      static HiresTime gettimeofday()
      {
        timeval tv;
        ::gettimeofday(&tv, 0);
        return tv;
      }

      time_t      getTvSec() const    { return tv.tv_sec; }
      suseconds_t getTvUsec() const   { return tv.tv_usec; }
      double      getSeconds() const  { return tv.tv_sec + tv.tv_usec / 1e6; }

      HiresTime& operator+= (const HiresTime& ht);

      HiresTime operator+ (const HiresTime& ht) const
      {
        HiresTime ret = *this;
        ret += ht;
        return ret;
      }

      HiresTime& operator-= (const HiresTime& ht);

      HiresTime operator- (const HiresTime& ht) const
      {
        HiresTime ret = *this;
        ret -= ht;
        return ret;
      }

      bool operator== (const HiresTime& ht) const
      {
        return tv.tv_sec == ht.tv.tv_sec
            && tv.tv_usec == ht.tv.tv_usec;
      }

      bool operator!= (const HiresTime& ht) const
      {
        return tv.tv_sec != ht.tv.tv_sec
            || tv.tv_usec != ht.tv.tv_usec;
      }

      bool operator< (const HiresTime& ht) const
      {
        return tv.tv_sec < ht.tv.tv_sec
            || tv.tv_sec == ht.tv.tv_sec
             && tv.tv_usec < ht.tv.tv_usec;
      }

      bool operator> (const HiresTime& ht) const
      {
        return ht < *this;
      }

      bool operator<= (const HiresTime& ht) const
      {
        return !(*this > ht);
      }

      bool operator>= (const HiresTime& ht) const
      {
        return !(*this < ht);
      }

      HiresTime operator/ (unsigned n) const
      {
        time_t sec = tv.tv_sec / n;
        time_t rem = tv.tv_sec % n;
        suseconds_t usec = (tv.tv_usec + 1000000 * rem) / n;
        return HiresTime(sec, usec);
      }
  };

  std::ostream& operator<< (std::ostream& out, const HiresTime& ht);
}

#endif // CXXTOOLS_HIRESTIME_H

