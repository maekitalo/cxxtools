////////////////////////////////////////////////////////////////////////
// Stopuhr
//
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
