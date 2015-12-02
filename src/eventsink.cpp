/*
 * Copyright (C) 2007 Marc Boris Duerner
 * Copyright (C) 2007 Laurentiu-Gheorghe Crisan
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
#include "cxxtools/eventsink.h"
#include "cxxtools/eventsource.h"

namespace cxxtools {

EventSink::EventSink()
{ }


EventSink::~EventSink()
{
    while( true )
    {
        RecursiveLock lock( _mutex );

        if( _sources.empty() )
            return;

        EventSource* source = _sources.front();
        if( ! source->tryDisconnect(*this) )
        {
            lock.unlock();
            Thread::yield();
        }
    }
}


void EventSink::queueEvent(const Event& event)
{
    this->onQueueEvent(event, false);
}


void EventSink::commitEvent(const Event& event)
{
    this->onCommitEvent(event, false);
}


void EventSink::queuePriorityEvent(const Event& event)
{
    this->onQueueEvent(event, true);
}


void EventSink::commitPriorityEvent(const Event& event)
{
    this->onCommitEvent(event, true);
}


void EventSink::onConnect(EventSource& source)
{
    RecursiveLock lock1( _mutex );

    _sources.push_back(&source);
}


void EventSink::onDisconnect(EventSource& source)
{
    RecursiveLock lock1( _mutex );

    _sources.remove(&source);
}


void EventSink::onUnsubscribe(EventSource& source)
{
    RecursiveLock lock1( _mutex );

    std::list<EventSource*>::iterator it;
    for(it = _sources.begin(); it != _sources.end(); ++it)
    {
        if(&source == *it)
        {
            _sources.erase(it);
            return;
        }
    }
}

} // namespace cxxtools
