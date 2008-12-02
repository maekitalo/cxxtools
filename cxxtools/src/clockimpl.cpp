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



