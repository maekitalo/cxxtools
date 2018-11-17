/*
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
#include "clockimpl.h"
#include <sys/time.h>
#include <time.h>

namespace cxxtools
{

Timespan ClockImpl::start()
{
#ifdef HAVE_CLOCK_GETTIME
    clock_gettime(CLOCK_MONOTONIC, &_startTime);
    return Timespan(_startTime.tv_sec, _startTime.tv_nsec / 1000);
#else
    gettimeofday( &_startTime, 0 );
    return Timespan(_startTime.tv_sec, _startTime.tv_usec);
#endif
}


Timespan ClockImpl::stop() const
{
#ifdef HAVE_CLOCK_GETTIME
    struct timespec stopTime;

    clock_gettime(CLOCK_MONOTONIC, &stopTime);

    time_t secs = stopTime.tv_sec - _startTime.tv_sec;
    suseconds_t usecs = (stopTime.tv_nsec - _startTime.tv_nsec) / 1000;
#else
    struct timeval stopTime;

    gettimeofday( &stopTime, 0 );

    time_t secs = stopTime.tv_sec - _startTime.tv_sec;
    suseconds_t usecs = stopTime.tv_usec - _startTime.tv_usec;
#endif

    return Timespan(secs, usecs);
}


UtcDateTime ClockImpl::getSystemTime()
{
    struct timeval tod;
    gettimeofday(&tod, NULL);

    struct tm tim;
    time_t sec = tod.tv_sec;
    gmtime_r(&sec, &tim);

    return UtcDateTime( tim.tm_year + 1900,
                        tim.tm_mon + 1,
                        tim.tm_mday,
                        tim.tm_hour,
                        tim.tm_min,
                        tim.tm_sec,
                        0,
                        tod.tv_usec);
}


LocalDateTime ClockImpl::getLocalTime()
{
    struct timeval tod;
    gettimeofday(&tod, NULL);

    struct tm tim;
    time_t sec = tod.tv_sec;
    localtime_r(&sec, &tim);

    return LocalDateTime( tim.tm_year + 1900,
                          tim.tm_mon + 1,
                          tim.tm_mday,
                          tim.tm_hour,
                          tim.tm_min,
                          tim.tm_sec,
                          0,
                          tod.tv_usec);
}


Timespan ClockImpl::getSystemTicks()
{
    struct timeval tv;
    gettimeofday( &tv, 0 );

    return Timespan(tv.tv_sec, tv.tv_usec);
}

} // namespace System



