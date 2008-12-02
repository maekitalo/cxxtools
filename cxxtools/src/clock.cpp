#include "clockimpl.h"
#include "cxxtools/clock.h"

namespace cxxtools {

Clock::Clock()
{
    _impl = new ClockImpl();
}

Clock::~Clock()
{
    delete _impl;
}

void Clock::start()
{
    _impl->start();
}

Timespan Clock::stop()
{
    return _impl->stop();
}

DateTime Clock::getLocalTime()
{
    return ClockImpl::getLocalTime();
}

Timespan Clock::getSystemTicks()
{
    return ClockImpl::getSystemTicks();
}

} //namespace cxxtools
