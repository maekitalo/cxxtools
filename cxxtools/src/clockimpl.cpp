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

namespace cxxtools {

ClockImpl::ClockImpl()
{}


ClockImpl::~ClockImpl()
{}


void ClockImpl::start()
{
   gettimeofday( &_startTime, 0 );
}


Timespan ClockImpl::stop()
{
    gettimeofday( &_stopTime, 0 );

    time_t secs = _stopTime.tv_sec - _startTime.tv_sec;
    suseconds_t usecs = _stopTime.tv_usec - _startTime.tv_usec;

    return Timespan(secs, usecs);
}


DateTime ClockImpl::getSystemTime()
{
    struct ::tm tim;
    struct timeval tod;

    gettimeofday(&tod, NULL);

    gmtime_r(&tod.tv_sec, &tim);

    return DateTime( tim.tm_year + 1900,
                     tim.tm_mon + 1,
                     tim.tm_mday,
                     tim.tm_hour,
                     tim.tm_min,
                     tim.tm_sec,
                     tod.tv_usec / 1000 );
}


DateTime ClockImpl::getLocalTime()
{
    struct timeval timeValue;
    gettimeofday(&timeValue, NULL);

    struct tm* currentTimeStruct;
    time_t time = timeValue.tv_sec;
    currentTimeStruct = localtime(&time);

    return DateTime( currentTimeStruct->tm_year + 1900,
                     currentTimeStruct->tm_mon + 1,
                     currentTimeStruct->tm_mday,
                     currentTimeStruct->tm_hour,
                     currentTimeStruct->tm_min,
                     currentTimeStruct->tm_sec,
                     timeValue.tv_usec / 1000 );
}


Timespan ClockImpl::getSystemTicks()
{
    struct timeval tv;
    gettimeofday( &tv, 0 );

    return Timespan(tv.tv_sec, tv.tv_usec);
}

} // namespace System



