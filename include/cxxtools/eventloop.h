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
#ifndef CXXTOOLS_SYSTEM_EVENTLOOP_H
#define CXXTOOLS_SYSTEM_EVENTLOOP_H

#include <cxxtools/event.h>
#include <cxxtools/signal.h>
#include <cxxtools/api.h>
#include <cxxtools/mutex.h>
#include <cxxtools/selector.h>
#include <cxxtools/eventsink.h>
#include <cxxtools/timespan.h>
#include <deque>

namespace cxxtools {

    class Selectable;

    /** @brief Thread-safe event loop supporting I/O multiplexing and Timers.
    */
    class EventLoopBase : public SelectorBase
                        , public EventSink
    {
        public:
            /** @brief Destructs the EventLoop
            */
            virtual ~EventLoopBase()
            {}

            /** @brief Starts the event loop
            */
            void run()
            { this->onRun(); }

            /** @brief Processes all events which are currently in the event queue
            */
            void processEvents()
            { this->onProcessEvents(); }

            /** @brief Stops the %EventLoop.
            */
            void exit()
            { this->onExit(); }

            /** @brief Sets the idle timeout
            */
            void setIdleTimeout(Milliseconds msecs)
            { _timeout = msecs; }

            /** @brief Returns the idle timeout
            */
            Milliseconds idleTimeout() const
            { return _timeout; }

            /** @brief Notifies about wait timeouts
                This signal is send when the timeout given to a wait
                call of the selector expires and no activity occured.
            */
            Signal<> timeout;

            /** @brief Reports all events
            */
            Signal<const Event&> event;

            /** @brief Emited when the eventloop is exited
            */
            Signal<> exited;

        protected:
            /** @brief Constructs the EventLoop
            */
            EventLoopBase()
            : _timeout(WaitInfinite)
            {}

            virtual void onRun() = 0;

            virtual void onExit() = 0;

            virtual void onProcessEvents() = 0;

        private:
            Timespan _timeout;
    };

    /** @brief Thread-safe event loop supporting I/O multiplexing and Timers.

        The System EventLoop can be used as the central entity of a thread or
        process to dispatch application events and wait on multiple IODevices or
        Timers for activity.

        Events can be added to the internal event queue, even from other threads
        using the method EventLoop::commitEvent or EventLoop::queueEvent. The
        first method will add the event to the internal queue and wake the
        event loop, the latter allows queing multiple events and it is up to
        the caller to wake the event loop by calling EventLoop::wake when all
        events are added. When the event loop processes its event, the signal
        "event" is send for each processed event. Events are processes in the
        order they were added.

        To start the %EventLoop the method EventLoop::run must be executed. It blocks
        until the event loop is stopped. To stop the %EventLoop, EventLoop::exit
        must be called. The delivery of the events occurs inside the thread that
        started the execution of the event loop.

        %IODevices and %Timers can be added to an %EventLoop just as to Selector.
        In fact a %Selector is used internally to implement the %EventLoop.

        Since the %EventLoop is a Runnable, it can be easily assigned to a Thread
        to give it its own event loop.
     */
    class CXXTOOLS_API EventLoop : public EventLoopBase
    {
        public:
            /** @brief Constructs the EventLoop
            */
            EventLoop();

            /** @brief Destructs the EventLoop
             */
            virtual ~EventLoop();

        protected:
            virtual void onAdd( Selectable& s );

            virtual void onRemove( Selectable& s );

            virtual void onReinit(Selectable& s);

            virtual void onChanged(Selectable& s);

            virtual void onRun();

            virtual bool onWaitUntil(Timespan timeout);

            virtual void onWake();

            virtual void onExit();

            virtual void onCommitEvent(const Event& event);

            virtual void onProcessEvents();

        private:
            bool _exitLoop;
            SelectorImpl* _selector;
            std::deque<Event* > _eventQueue;
            RecursiveMutex _queueMutex;
    };

} // namespace cxxtools

#endif
