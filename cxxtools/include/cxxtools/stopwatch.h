/* cxxtools/stopwatch.h
 *
 * cxxtools - general purpose C++-toolbox
 * Copyright (C) 2003 Tommi Maekitalo
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

#include <sys/time.h>

#ifndef STOPWATCH_H
#define STOPWATCH_H

class Stopwatch
{
    static long tv2msec(const struct timeval& tv)
    {
      return tv.tv_sec * 1000 + tv.tv_usec / 1000;
    }

    struct timeval m_start;
    struct timeval m_stop;
    bool running;

  public:
    Stopwatch()
      : running(true)
    {
      gettimeofday(&m_start, 0);
    }

    void start()
    {
      gettimeofday(&m_start, 0);
      running = true;
    }

    void stop()
    {
      gettimeofday(&m_stop, 0);
      running = false;
    }

    long getStartMs() const
    {
      return tv2msec(m_start);
    }

    long getStopMs() const
    {
      return tv2msec(m_stop);
    }

    long getMs() const
    {
      if (running)
      {
        struct timeval tv;
        gettimeofday(&tv, 0);
        return (tv.tv_sec  - m_start.tv_sec) * 1000
          + (tv.tv_usec - m_start.tv_usec) / 1000;
      }
      return (m_stop.tv_sec  - m_start.tv_sec) * 1000
        + (m_stop.tv_usec - m_start.tv_usec) / 1000;
    }
};

#endif // STOPWATCH_H
