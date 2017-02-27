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
#ifndef CXXTOOLS_EVENTSOURCE_H
#define CXXTOOLS_EVENTSOURCE_H

#include <cxxtools/event.h>
#include <cxxtools/signal.h>
#include <cxxtools/mutex.h>
#include <cxxtools/eventsink.h>
#include <map>
#include <typeinfo>

namespace cxxtools {

class EventSink;

/** @brief Sends Events to receivers in other threads

    The Signal class is not thread-safe and can only be used for
    intra-thread communication. To pass Events between different threads
    use an %EventSource instead. Thread-safety only refers to the usage
    of the %EventSource itself (connection, disconnecting...) and not the
    slot.
    Construction and destruction must always occur in isolation.
*/
class EventSource
{
#if __cplusplus >= 201103L
        EventSource(const EventSource&) = delete;
        EventSource& operator=(const EventSource&) = delete;
#else
        EventSource(const EventSource&) { }
        EventSource& operator=(const EventSource&) { return *this; }
#endif
        friend class EventSink;

    public:
        EventSource();

        ~EventSource();

        void send(const cxxtools::Event& ev);

        void connect(EventSink& sink);

        void disconnect(EventSink& sink);

        template <typename EventT>
        void subscribe(EventSink& sink)
        {
            subscribe( sink, typeid(EventT) );
        }

        template <typename EventT>
        void unsubscribe(EventSink& sink)
        {
            unsubscribe( sink, typeid(EventT) );
        }

    private:
        bool tryDisconnect(EventSink& sink);

        void subscribe(EventSink& sink, const std::type_info& ti);

        void unsubscribe(EventSink& sink, const std::type_info& ti);

    private:
        struct Sentry;

        typedef std::multimap< const std::type_info*,
                               EventSink*,
                               CompareEventTypeInfo > SinkMap;

        mutable RecursiveMutex _mutex;
        mutable RecursiveMutex* _dmutex;
        mutable SinkMap _sinks;
        mutable Sentry* _sentry;
        mutable bool _dirty;
};

} // namespace cxxtools

#endif

