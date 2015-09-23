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
#include "selectorimpl.h"
#include "cxxtools/selector.h"
#include "cxxtools/timer.h"
#include "cxxtools/timespan.h"
#include "cxxtools/log.h"

log_define("cxxtools.selector")

namespace cxxtools {

const int SelectorBase::WaitInfinite;

SelectorBase::~SelectorBase()
{
    while( _timers.size() )
    {
       Timer* timer = _timers.begin()->second;
       timer->setSelector(0);
    }
}


void SelectorBase::add(Selectable& s)
{
    s.setSelector(this);
}


void SelectorBase::remove(Selectable& s)
{
    if(s.selector() == this)
        s.setSelector(0);
}


void SelectorBase::add(Timer& timer)
{
    timer.setSelector(this);
}


void SelectorBase::remove( Timer& timer )
{
    if(timer.selector() == this)
        timer.setSelector(0);
}


void SelectorBase::onAddTimer(Timer& timer)
{
    if( timer.active() )
    {
        TimerMap::value_type elem(timer.finished(), &timer);
        _timers.insert(elem);
        //_timers.insert( std::make_pair(timer.finished(), &timer) );
    }
}


void SelectorBase::onRemoveTimer( Timer& timer )
{
    std::multimap<Timespan, Timer*>::iterator it;
    for(it = _timers.begin(); it != _timers.end(); ++it)
    {
        if(it->second == &timer)
        {
            _timers.erase(it);
            return;
        }
    }
}


void SelectorBase::onTimerChanged(Timer& timer)
{
    if( timer.active() )
    {
        TimerMap::value_type elem(timer.finished(), &timer);
        _timers.insert(elem);
        //_timers.insert( std::make_pair(timer.finished(), &timer) );
    }
    else
    {
        SelectorBase::onRemoveTimer(timer);
    }
}


bool SelectorBase::updateTimer(Timespan& lowestTimeout)
{
    if( _timers.empty() )
        return false;

    Timespan now = Timespan::gettimeofday();
    Timer* timer = _timers.begin()->second;
    bool timerActive = now >= timer->finished();

    while( ! _timers.empty() )
    {
        timer = _timers.begin()->second;

        if ( now < timer->finished() )
        {
            lowestTimeout = timer->finished();
            log_debug("lowestTimeout => " << lowestTimeout);
            break;
        }

        timer->update(now);

        if( ! _timers.empty() )
        {
            timer = _timers.begin()->second;
            _timers.erase( _timers.begin() );
            TimerMap::value_type elem(timer->finished(), timer);
            _timers.insert(elem);
            //_timers.insert( std::make_pair(timer->finished(), timer) );
        }
    }

    return timerActive;
}


bool SelectorBase::wait(Milliseconds msecs)
{
    if (msecs <= Timespan(0))
        return waitUntil(msecs);
    else
        return waitUntil(Timespan::gettimeofday() + msecs);
}

bool SelectorBase::waitUntil(Timespan t)
{
    log_debug("waitUntil(" << t << ')');

    Timespan timerTimeout = Timespan(Selector::WaitInfinite);

    // If a timer is immediately ready, still check for an
    // active selectable to avoid timer preemption
    if ( updateTimer(timerTimeout) )
    {
        onWaitUntil(Timespan(0));
        return true;
    }

    // This handles the case when no timer will become
    // active in the given timeout. The result of the
    // wait call indicates activity
    if (timerTimeout < Timespan(0) || (t >= Timespan(0) && t < timerTimeout))
    {
        return onWaitUntil(t);
    }

    // A timer will become active before the timeout expires
    while(true)
    {
        log_debug("wait(" << timerTimeout << ')');

        if (onWaitUntil(timerTimeout))
            return true;

        if (updateTimer(timerTimeout))
            return true;
    }
}


Timespan SelectorBase::waitTimer()
{
    log_debug("waitTimer()");

    while (true)
    {
        Timespan timerTimeout = Timespan(Selector::WaitInfinite);
        updateTimer(timerTimeout);
        onWaitUntil(Timespan(0));

        if (timerTimeout < Timespan(0))
            return timerTimeout;

        Timespan now = Timespan::gettimeofday();
        if (timerTimeout > now)
            return timerTimeout - now;
    }
}


void SelectorBase::wake()
{ 
    onWake();
}


SelectorBase::SelectorBase()
{}


Selector::Selector()
: _impl( 0 )
{
    _impl = new SelectorImpl();
}


Selector::~Selector()
{
    delete _impl;
}


void Selector::onAdd( Selectable& selectable )
{
    _impl->add(selectable);
}


void Selector::onRemove( Selectable& selectable )
{
    _impl->remove(selectable);
}


void Selector::onChanged(Selectable& s)
{
    _impl->changed(s);
}


void Selector::onReinit(Selectable& /*s*/)
{
}


bool Selector::onWaitUntil(Timespan timeout)
{
    return _impl->waitUntil(timeout);
}


void Selector::onWake()
{
    _impl->wake();
}


SelectorImpl& Selector::impl()
{
    return *_impl;
}

}//namespace cxxtools
