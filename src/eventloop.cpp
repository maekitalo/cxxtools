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
#include "cxxtools/log.h"
#include <deque>

log_define("cxxtools.eventloop")

namespace cxxtools
{
namespace
{
    struct EvPtr
    {
        Event* ev;
        explicit EvPtr(Event* ev_)
            : ev(ev_)
        { }

        ~EvPtr()
        { if (ev) ev->destroy(); }

    };
}

class EventLoop::Impl
{
public:
    Impl()
        : _exitLoop(false),
          _selector(new SelectorImpl()),
          _eventsPerLoop(16)
        { }
    ~Impl();

    bool eventQueueEmpty()
    { return _eventQueue.empty() && _priorityEventQueue.empty() && _activeEventQueue.empty(); }

    Event* front()
    {
        if (_priorityEventQueue.empty())
            return _eventQueue.front();
        else
            return _priorityEventQueue.front();
    }

    void pop_front()
    {
        if (_priorityEventQueue.empty())
            _eventQueue.pop_front();
        else
            _priorityEventQueue.pop_front();
    }

    bool _exitLoop;
    SelectorImpl* _selector;
    std::deque<Event*> _eventQueue;
    std::deque<Event*> _priorityEventQueue;
    std::deque<Event*> _activeEventQueue;
    bool _activeEventQueueIsPriority;
    Mutex _queueMutex;
    unsigned _eventsPerLoop;
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

        while ( ! _priorityEventQueue.empty() )
        {
            Event* ev = _priorityEventQueue.front();
            _priorityEventQueue.pop_front();
            ev->destroy();
        }

        while ( ! _activeEventQueue.empty() )
        {
            Event* ev = _activeEventQueue.front();
            _activeEventQueue.pop_front();
            if (ev)
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

unsigned EventLoop::eventsPerLoop()
{
    return _impl->_eventsPerLoop;
}

void EventLoop::eventsPerLoop(unsigned n)
{
    _impl->_eventsPerLoop = n;
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
    started();

    while (true)
    {
        MutexLock lock(_impl->_queueMutex);

        if (_impl->_exitLoop)
        {
            _impl->_exitLoop = false;
            break;
        }

        bool eventQueueEmpty = _impl->eventQueueEmpty();
        if (!eventQueueEmpty)
        {
            lock.unlock();
            processEvents(_impl->_eventsPerLoop);
            eventQueueEmpty = _impl->eventQueueEmpty();
        }

        lock.unlock();

        if (eventQueueEmpty)
        {
            idle();

            bool active = wait(idleTimeout());
            if (!active)
                timeout.send();
        }
        else
        {
            // process I/O events
            wait(0);
        }
    }

    exited();
}


bool EventLoop::onWaitUntil(Timespan timeout)
{
    if (_impl->_selector->waitUntil(timeout))
    {
        MutexLock lock(_impl->_queueMutex);

        if (!_impl->eventQueueEmpty())
        {
            lock.unlock();
            processEvents(_impl->_eventsPerLoop);
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
    log_debug("exit loop");

    {
        MutexLock lock(_impl->_queueMutex);
        _impl->_exitLoop = true;
    }

    wake();
}

void EventLoop::onQueueEvent(const Event& ev, bool priority)
{
    log_debug("queue event");

    MutexLock lock( _impl->_queueMutex );

    EvPtr cloned(ev.clone());

    if (priority)
        _impl->_priorityEventQueue.push_back(cloned.ev);
    else
        _impl->_eventQueue.push_back(cloned.ev);

    cloned.ev = 0;
}


void EventLoop::onCommitEvent(const Event& ev, bool priority)
{
    onQueueEvent(ev, priority);
    _impl->_selector->wake();
}


void EventLoop::onProcessEvents(unsigned max)
{
    unsigned count = 0;

    auto& exitLoop = _impl->_exitLoop;
    auto& eventQueue = _impl->_eventQueue;
    auto& priorityEventQueue = _impl->_priorityEventQueue;
    auto& activeEventQueue = _impl->_activeEventQueue;
    auto& activeEventQueueIsPriority = _impl->_activeEventQueueIsPriority;
    auto& queueMutex = _impl->_queueMutex;

    log_debug("processEvents(max:" << max << ") normal/priority/active(priority): " << eventQueue.size() << '/' << priorityEventQueue.size() << '/' << activeEventQueue.size() << '(' << activeEventQueueIsPriority << ')');

    if (!activeEventQueue.empty() && !activeEventQueueIsPriority)
    {
        MutexLock lock(queueMutex);
        if (!priorityEventQueue.empty())
        {
            log_debug("priority events bypass active events");

            if (eventQueue.empty())
            {
                eventQueue.swap(activeEventQueue);
            }
            else
            {
                eventQueue.insert(eventQueue.begin(), activeEventQueue.begin(), activeEventQueue.end()); 
                activeEventQueue.clear();
            }

            priorityEventQueue.swap(activeEventQueue);
            activeEventQueueIsPriority = true;
        }
    }

    while (!exitLoop)
    {
        if (activeEventQueue.empty())
        {
            MutexLock lock(queueMutex);
            if (!priorityEventQueue.empty())
            {
                log_debug("move " << priorityEventQueue.size() << " priority events to active event queue");
                activeEventQueueIsPriority = true;
                activeEventQueue.swap(priorityEventQueue);
            }
            else if (!eventQueue.empty())
            {
                log_debug("move " << eventQueue.size() << " events to active event queue");
                activeEventQueueIsPriority = false;
                activeEventQueue.swap(eventQueue);
            }
        }

        if (exitLoop || activeEventQueue.empty())
        {
            log_debug_if(activeEventQueue.empty(), "no events to process");
            break;
        }

        EvPtr ev(activeEventQueue.front());
        activeEventQueue.pop_front();

        ++count;

        log_debug("send event " << count);
        event.send(*ev.ev);

        if (max != 0 && count >= max)
        {
            log_debug("maximum number of events per loop " << max << " reached");
            break;
        }
    }
}

} // namespace cxxtools
