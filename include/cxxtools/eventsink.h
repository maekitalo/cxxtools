/*
 * Copyright (C) 2006-2007 Laurentiu-Gheorghe Crisan
 * Copyright (C) 2006-2008 Marc Boris Duerner
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
#ifndef CXXTOOLS_EVENTSINK_H
#define CXXTOOLS_EVENTSINK_H

#include <cxxtools/event.h>
#include <cxxtools/mutex.h>
#include <list>

namespace cxxtools {

    class EventSource;

    class EventSink
    {
#if __cplusplus >= 201103L
            EventSink(const EventSink&) = delete;
            EventSink& operator=(const EventSink&) = delete;
#else
            EventSink(const EventSink&) { }
            EventSink& operator=(const EventSink&) { return *this; }
#endif
        friend class EventSource;

        public:
            EventSink();

            virtual ~EventSink();

            /// Puts a event in the queue but does not wake the loop.
            void queueEvent(const Event& event);

            /// Puts a event in the queue and wakes the loop so that the event is processed.
            void commitEvent(const Event& event);

            /// Puts a priority event in the queue and wakes the loop so that the event is processed.
            /// Priority events are processed before any non priority event.
            void queuePriorityEvent(const Event& event);

            /// Puts a priority event in the queue and wakes the loop so that the event is processed.
            /// Priority events are processed before any non priority event.
            void commitPriorityEvent(const Event& event);

        protected:
            virtual void onQueueEvent(const Event& event, bool priority) = 0;
            virtual void onCommitEvent(const Event& event, bool priority) = 0;

        private:
            void onConnect(EventSource& source);
            void onDisconnect(EventSource& source);
            void onUnsubscribe(EventSource& source);

        private:
            mutable RecursiveMutex _mutex;
            std::list<EventSource*> _sources;
    };

} // namespace cxxtools

#endif
