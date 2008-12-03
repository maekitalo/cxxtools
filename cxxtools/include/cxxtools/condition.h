/***************************************************************************
 *   Copyright (C) 2006-2008 by Marc Boris Duerner                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   As a special exception, you may use this file as part of a free       *
 *   software library without restriction. Specifically, if other files    *
 *   instantiate templates or use macros or inline functions from this     *
 *   file, or you compile this file and link it with other files to        *
 *   produce an executable, this file does not by itself cause the         *
 *   resulting executable to be covered by the GNU General Public          *
 *   License. This exception does not however invalidate any other         *
 *   reasons why the executable file might be covered by the GNU Library   *
 *   General Public License.                                               *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this program; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef CXXTOOLS_SYSTEM_CONDITION_H
#define CXXTOOLS_SYSTEM_CONDITION_H

#include <cxxtools/api.h>
#include <cxxtools/noncopyable.h>
#include <cxxtools/mutex.h>
#include <cstddef>

namespace cxxtools {

    /** @brief This class is used to control concurrent access.

        The Condition class is used to control concurrent access in a queued
        manner. The Condition class supports two types of signalling events,
        manual reset and automatic reset.
        Manual resets cause all blocked callers to be released. This can be
        understood as some kind of broadcast to signal all blocked callers at
        once. Manual resets are triggered by a call to signal().
        Automatic resets cause only a single blocked caller to be released.
        So this can be seen as some kind of wait queue where only the topmost
        is signaled. Automatic resets are signaled by a call to broadcast().
     */
    class CXXTOOLS_API Condition : private NonCopyable
    {
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
                signaled. The given mutex will be unlocked before the caller
                is suspended. The suspension takes at maximum ms milliseconds.
                Returns true if successful, false if a timeout occurred.
            */
            bool wait( Mutex& mtx, unsigned int ms);

            bool wait( MutexLock& m, unsigned int ms)
            { return this->wait( m.mutex(), ms ); }

            //! @brief Unblock a single blocked thread.
            void signal();

            //! @brief Unblock all blocked threads.
            void broadcast();

        private:
            class ConditionImpl* _impl;
    };

} // !namespace cxxtools

#endif
