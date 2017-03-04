/*
 * Copyright (C) 2006-2007 Laurentiu-Gheorghe Crisan
 * Copyright (C) 2006-2007 Marc Boris Duerner
 * Copyright (C) 2006-2007 PTV AG
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
#ifndef CXXTOOLS_SYSTEM_SELECTOR_H
#define CXXTOOLS_SYSTEM_SELECTOR_H

#include <cxxtools/timespan.h>
#include <cxxtools/connectable.h>
#include <map>

namespace cxxtools {

    class Timer;
    class Selectable;
    class Application;
    class SelectorImpl;

    /** @brief Reports activity on a set of devices.

        A Selector can be used to monitor a set of Selectables and Timers
        and wait for activity on them. The wait call can be performed with
        a timeout and the respective timeout signal is sent if it occurs.
        Clients can be notified about Timer and Selectable activity by
        connecting to the appropriate signals of the Timer and Selectable
        classes.

        The following example uses a %Selector to wait on acitvity on
        a %Timer, which is set to time-out after 1000 msecs.

        @code
        // slot to handle timer activity
        void onTimer();

        int main()
        {
            Timer timer;
            timer.start(1000);
            connect(timer.timeout, ontimer);

            Selector selector;
            selector.addTimer(timer);
            selector wait();

            return 0;
        }
        @endcode

        A Selector is the heart of the EventLoop, which calls Selector::wait
        continously. The %EventLoop and %Application classes provide the same API
        as the Selector itself.
    */
    class SelectorBase : public Connectable
    {
            friend class Selectable;
            friend class Timer;

#if __cplusplus >= 201103L
            SelectorBase(SelectorBase&) = delete;
            SelectorBase& operator=(SelectorBase&) = delete;
#else
            SelectorBase(SelectorBase&) { }
            SelectorBase& operator=(SelectorBase&) { return *this; }
#endif

        public:
            static const int WaitInfinite = -1;

            //! @brief Destructor
            virtual ~SelectorBase();

            /** @brief Adds an IOResult

                Adds an IOResult to the selector. IOResult are removed
                automatically when they get destroyed.
            */
            void add(Selectable& s);

            /** @brief Cancel an IOResult.
            */
            void remove(Selectable& s);

            /** @brief Adds a Timer

                Adds a Timer to the selector. Timers are removed
                automatically when they get destroyed.

                @param timer The device to add
            */
            void add(Timer& timer);

            /** @brief Removes a Timer

                @param timer The timer to remove
            */
            void remove(Timer& timer);

            /** @brief Wait for activity

                This method will wait for activity on the registered
                Selectables and Timers and execute the registered callbacks.
                It will return when either the timeout has expired or at least
                one event has occured.

                When the timeout is 0, then just active events are triggered.

                When the timeout is negative, it will wait until the next event
                without timeout. Use Selector::WaitInfinite to wait without
                timeout.

                Note that the class Milliseconds has a implicit constructor
                which accepts a double value, so that you can pass just a
                number to the method to get a timeout in milliseconds.

                @param msecs timeout
                @return false on timeout
            */
            bool wait(Milliseconds msecs = WaitInfinite);

            /** @brief Wait for activity

                This method is like wait but the timeout is specified as a
                timestamp. The method will wait until the specified time.

                When the timespan is negative, the method will wait until the
                next event without timeout.

                When the timespan is smaller or equal the current time, it will
                just check for active events and execute them.

                @param t timeout as a timespan since 1970-01-01 00:00:00
             */
            bool waitUntil(Timespan t);

            /** @brief Wakes the selctor from waiting

                This method can be used to end a Selector::wait call
                before the timeout expires. It is supposed to be used from
                another thread and thus is thread-safe.
            */
            void wake();

            /** @brief Wait for activity

                This method executes just the pending events and returns the time
                when the next timer is due. When there are no active timers a
                negative timespan is returned.
             */
            Timespan waitTimer();

        protected:
            //! @brief Default constructor
            SelectorBase();

            void onAddTimer(Timer& timer);

            void onRemoveTimer(Timer& timer);

            void onTimerChanged(Timer& timer);

            /** @brief A Selectable is added to this %Selector

                Do not throw exceptions.
            */
            virtual void onAdd(Selectable&) = 0;

            /** @brief A Selectable is removed from this %Selector

                Do not throw exceptions.
            */
            virtual void onRemove(Selectable&) = 0;

            /** @brief A Selectable is reinitialised and needs to be updated

                Do not throw exceptions.
            */
            virtual void onReinit(Selectable&) = 0;

            /** @brief A Selectable in this %Selector has changed

                Do not throw exceptions.
            */
            virtual void onChanged(Selectable& s) = 0;

            virtual bool onWaitUntil(Timespan timeout) = 0;

            virtual void onWake() = 0;

        private:
            /** @internal Update all timers and return true if a timer fired

                @param timeout interval to next expiring timer
            */
            bool updateTimer(Timespan& timeout);

            //! @internal
            typedef std::multimap<Timespan, Timer*> TimerMap;

            //! @internal
            TimerMap _timers;

            void* _reserved;
    };

    class Selector : public SelectorBase
    {
        public:
            Selector();

            virtual ~Selector();

            SelectorImpl& impl();

        protected:
            void onAdd( Selectable& dev );

            void onRemove( Selectable& dev );

            void onReinit(Selectable&);

            void onChanged(Selectable&);

            bool onWaitUntil(Timespan timeout);

            void onWake();

        private:
            //! @internal
            class SelectorImpl* _impl;
    };

} //namespace cxxtools

#endif
