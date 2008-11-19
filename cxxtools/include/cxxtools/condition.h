/*
 * Copyright (C) 2008 Tommi Maekitalo, Marc Boris Duerner
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
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
 * This program is distributed in the hope that it will be useful, but
 * is provided AS IS, WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, and
 * NON-INFRINGEMENT.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 *
 */

#ifndef CXXTOOLS_CONDITION_H
#define CXXTOOLS_CONDITION_H

#include <cxxtools/noncopyable.h>
#include <cxxtools/mutex.h>
#include <pthread.h>

namespace cxxtools {

class Mutex;
class MutexLock;

class Condition : private NonCopyable
{
    pthread_cond_t cond;

  public:
    Condition();

    ~Condition();

    void signal();

    void broadcast();

    void wait(Mutex& mtx);

    void wait(MutexLock& lock)
    { wait( lock.getMutex() ); }

    bool wait(MutexLock& lock, unsigned ms);

    //! @internal Only for compatibility
    bool timedwait(MutexLock& lock, unsigned ms)
    { return wait(lock, ms); }

    //! @internal Only for compatibility
    bool timedwait(MutexLock& lock, const struct timespec& time);
};

}

#endif // CXXTOOLS_MUTEX_H
