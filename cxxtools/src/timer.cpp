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
#include "cxxtools/timer.h"
#include "cxxtools/clock.h"
#include "cxxtools/selector.h"
#include <limits>

namespace cxxtools {

Timer::Timer()
: _selector(0)
, _active(false)
, _interval(0)
, _remaining(0)
, _finished(0)
{ }


Timer::~Timer()
{
    try
    {
        if(_selector)
            _selector->remove(*this);
    }
    catch(...) {}
}


bool Timer::active() const
{
    return _active;
}


std::size_t Timer::interval() const
{
    return _interval;
}


void Timer::start(std::size_t interval)
{
    if( _active)
        stop();
    
    _active = true;
    _interval = interval;
    _remaining = cxxtools::int64_t(_interval) * 1000;
    _finished = Clock::getSystemTicks() + _remaining;

    if(_selector)
        _selector->onTimerChanged(*this);
}


void Timer::stop()
{
    _active = false;
    _remaining = 0;
    _finished = 0;

    if(_selector)
        _selector->onTimerChanged(*this);
}


bool Timer::update()
{
    if(_active == false)
        return false;

    Timespan now = Clock::getSystemTicks();
    return this->update(now);
}


bool Timer::update(const Timespan& now)
{
    if(_active == false)
        return false;

    bool hasElapsed = now >= _finished;

    while( _active && now >= _finished )
    {
        _finished += (_interval * 1000);
        timeout.send();
    }

    _remaining = _finished - now;
    return hasElapsed;
}


void Timer::setSelector(SelectorBase* selector)
{
    if(_selector == selector)
        return;

    if(_selector)
    {
        _selector->onRemoveTimer(*this);
    }

    if(selector)
    {
        selector->onAddTimer(*this);
    }

    _selector = selector;
}

}
