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
#include "selectorimpl.h"
#include "cxxtools/eventloop.h"
#include "cxxtools/mutex.h"
#include <deque>

namespace cxxtools
{

class EventLoop::Impl
{
public:
    Impl()
        : _exitLoop(false),
          _selector(new SelectorImpl())
        { }
    ~Impl();

    bool _exitLoop;
    SelectorImpl* _selector;
    std::deque<Event* > _eventQueue;
    RecursiveMutex _queueMutex;
};

EventLoop::Impl::~Impl()
{
    try
    {
        while ( ! _eventQueue.empty() )
        {
            Event* ev = _eventQueue.front();
            _eventQueue.pop_front();
            ev->destroy();
        }
    }
    catch(...)
    {}

    delete _selector;
}

EventLoop::EventLoop()
: _impl(new Impl())
{
}


EventLoop::~EventLoop()
{
    delete _impl;
}


void EventLoop::onAdd( Selectable& s )
{
    return _impl->_selector->add( s );
}


void EventLoop::onRemove( Selectable& s )
{
    _impl->_selector->remove( s );
}


void EventLoop::onReinit(Selectable& /*s*/)
{
}


void EventLoop::onChanged(Selectable& s)
{
    _impl->_selector->changed(s);
}


void EventLoop::onRun()
{
    while( true )
    {
        RecursiveLock lock(_impl->_queueMutex);

        if(_impl->_exitLoop)
        {
            _impl->_exitLoop = false;
            break;
        }

        if( !_impl->_eventQueue.empty() )
        {
            lock.unlock();
            this->processEvents();
        }

        lock.unlock();

        bool active = this->wait( this->idleTimeout() );
        if( ! active )
            timeout.send();
    }

    exited();
}


bool EventLoop::onWaitUntil(Timespan timeout)
{
    if( _impl->_selector->waitUntil(timeout) )
    {
        RecursiveLock lock(_impl->_queueMutex);

        if( !_impl->_eventQueue.empty() )
        {
            lock.unlock();
            this->processEvents();
        }

        return true;
    }

    return false;
}


void EventLoop::onWake()
{
    _impl->_selector->wake();
}


void EventLoop::onExit()
{
    RecursiveLock lock(_impl->_queueMutex);
    _impl->_exitLoop = true;
    lock.unlock();

    this->wake();
}


void EventLoop::onQueueEvent(const Event& ev)
{
    RecursiveLock lock( _impl->_queueMutex );

    Event* clonedEvent = ev.clone();

    try
    {
        _impl->_eventQueue.push_back(clonedEvent);
    }
    catch(...)
    {
        clonedEvent->destroy();
        throw;
    }
}


void EventLoop::onCommitEvent(const Event& ev)
{
    onQueueEvent(ev);
    _impl->_selector->wake();
}


void EventLoop::onProcessEvents()
{
    while( !_impl->_exitLoop )
    {
        RecursiveLock lock(_impl->_queueMutex);

        if ( _impl->_eventQueue.empty() || _impl->_exitLoop )
            break;

        Event* ev = _impl->_eventQueue.front();
        _impl->_eventQueue.pop_front();

        try
        {
            lock.unlock();
            event.send(*ev);
        }
        catch(...)
        {
            ev->destroy();
            throw;
        }

        ev->destroy();
    }
}

} // namespace cxxtools
