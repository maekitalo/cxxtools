/*
 * Copyright (C) 2008 Marc Boris Duerner
 * 
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
#include "cxxtools/eventsource.h"
#include "cxxtools/eventsink.h"

namespace cxxtools {

RecursiveMutex dmx;

struct EventSource::Sentry
{
    Sentry(const EventSource* es)
    : _es(es)
    {
        _es->_sentry = this;
        _es->_dirty = false;
    }

    ~Sentry()
    {
        if( _es )
            this->detach();
    }

    void detach()
    {
        if( _es->_dirty )
        {
            SinkMap::iterator it = _es->_sinks.begin();
            while( it != _es->_sinks.end() )
            {
                if( it->second )
                {
                    ++it;
                }
                else
                {
                    _es->_sinks.erase(it++);
                }
            }
        }

        _es->_dirty = false;
        _es->_sentry = 0;
        _es = 0;
    }

    bool operator!() const
    {
        return _es == 0;
    }

    const EventSource* _es;
};


EventSource::EventSource()
: _dmutex(&dmx)
, _sentry(0)
, _dirty(false)
{ }


EventSource::~EventSource()
{
    RecursiveLock dlock(*_dmutex);

    while( true )
    {
        RecursiveLock lock( _mutex );

        if(_sentry)
            _sentry->detach();

        if( _sinks.empty() )
            return;

        EventSink* sink = _sinks.begin()->second;

        if( ! this->tryDisconnect(*sink) )
        {
            lock.unlock();
            Thread::yield();
        }
    }
}


void EventSource::connect(EventSink& sink)
{
    RecursiveLock lock( _mutex );

    sink.onConnect(*this);

    const std::type_info* ti = 0;
    _sinks.insert( std::make_pair(ti, &sink) );
}


void EventSource::disconnect(EventSink& sink)
{
    RecursiveLock lock( _mutex );

    sink.onDisconnect(*this);

    SinkMap::iterator it = _sinks.begin();
    while( it != _sinks.end() )
    {
        if(it->second == &sink)
        {
            if(_sentry)
            {
                _dirty = true;
                it->second = 0;
            }
            else
            {
                _sinks.erase(it++);
                continue;
            }
        }

        ++it;
    }
}


bool EventSource::tryDisconnect(EventSink& sink)
{
    if( _dmutex->tryLock() )
    {
        this->disconnect(sink);
        _dmutex->unlock();
        return true;
    }

    return false;
}


void EventSource::subscribe(EventSink& sink, const std::type_info& ti)
{
    RecursiveLock lock( _mutex );

    sink.onConnect(*this);
    _sinks.insert( std::make_pair(&ti, &sink) );
}


void EventSource::unsubscribe(EventSink& sink, const std::type_info& ti)
{
    RecursiveLock lock( _mutex );

    sink.onUnsubscribe(*this);

    SinkMap::iterator it = _sinks.lower_bound(&ti);
    while( it != _sinks.end() && *(it->first) == ti )
    {
        if(it->second == &sink)
        {
            if(_sentry)
            {
                _dirty = true;
                it->second = 0;
            }
            else
            {
                _sinks.erase(it++);
                continue;
            }
        }

        ++it;
    }
}


void EventSource::send(const cxxtools::Event& ev)
{
    RecursiveLock lock(_mutex);
    EventSource::Sentry sentry(this);

    SinkMap::iterator it;
    for(it = _sinks.begin(); it != _sinks.end(); ++it)
    {
        EventSink* sink = it->second;

        if(sink)
            sink->commitEvent(ev);

        if( ! sentry )
            return;
    }
}

} // namespace cxxtools
