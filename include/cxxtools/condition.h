/*
 * Copyright (C) 2006-2008 by Marc Boris Duerner
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
#ifndef CXXTOOLS_SYSTEM_CONDITION_H
#define CXXTOOLS_SYSTEM_CONDITION_H

#include <cxxtools/mutex.h>
#include <cxxtools/timespan.h>
#include <cstddef>

namespace cxxtools {

    /** @brief This class is used to control concurrent access.

        The Condition class is used to control concurrent access in a queued
        manner. The Condition class supports two types of signaling events,
        manual reset and automatic reset.
        Manual resets cause all blocked callers to be released. This can be
        understood as some kind of broadcast to signal all blocked callers at
        once. Manual resets are triggered by a call to signal().
        Automatic resets cause only a single blocked caller to be released.
        So this can be seen as some kind of wait queue where only the topmost
        is signaled. Automatic resets are signaled by a call to broadcast().
     */
    class Condition
    {
#if __cplusplus >= 201103L
            Condition(const Condition&) = delete;
            Condition& operator=(const Condition&) = delete;
#else
            Condition(const Condition&) { }
            Condition& operator=(const Condition&) { return *this; }
#endif

        public:
            //! @brief Default Constructor.
            Condition();

            //! @brief Destructor.
            ~Condition();

            /** @brief Wait until condition becomes signalled.

                Causes the caller to be suspended until the condition will be
                signaled. The given mutex will be unlocked before the caller
                is suspended.
            */
            void wait( Mutex& mtx);

            void wait( MutexLock& m)
            { this->wait( m.mutex() ); }

            /** @brief Wait until condition becomes signalled.

                Causes the caller to be suspended until the condition will be
                signaled. The given mutex will be unlocked before the caller is
                suspended. The suspension takes at maximum the time specified
                by ts. Returns true if successful, false if a timeout occurred.

                The time is specified as a Millisecond object, which can be
                implicitly constructed with a number or another Timespan derived
                object like Second.

                Example:
                @code
                  cxxtools::MutexLock lock(someMutex);
                  cxxtools::Condition condition;
                  ...
                  condition.wait(lock, 2000); // waits up to 2 seconds for the condition
                                              // to be signaled by another thread
                  condition.wait(lock, cxxtools::Seconds(2);  // same as above
                @endcode
            */
            bool wait( Mutex& mtx, const Milliseconds& ts);

            bool wait( MutexLock& m, const Milliseconds& ts)
            { return this->wait( m.mutex(), ts ); }

            //! @brief Unblock a single blocked thread.
            void signal();

            //! @brief Unblock all blocked threads.
            void broadcast();

        private:
            class ConditionImpl* _impl;
    };

} // !namespace cxxtools

#endif
