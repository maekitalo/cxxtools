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

#ifndef CXXTOOLS_MUTEX_H
#define CXXTOOLS_MUTEX_H

#include <sched.h>
#include <cxxtools/atomicity.h>
#include <cxxtools/noncopyable.h>
#include <cxxtools/syserror.h>
#include <pthread.h>

namespace cxxtools
{

/**
 Exception-class for problems with mutexes.

 This Exception is thrown in case of problems with mutexes.
 */
class MutexException : public SysError
{
  public:
    MutexException(int err, const char* method)
      : SysError(err, method)
      { }
};

class Condition;

class Mutex : private NonCopyable
{
    friend class Condition;

  protected:
    pthread_mutex_t m_mutex;

  public:
    Mutex();
    ~Mutex();

    void lock();
    bool tryLock();
    void unlock();
    bool unlockNoThrow();
};

class RWLock : private NonCopyable
{
    pthread_rwlock_t m_rwlock;

  public:
    RWLock();
    ~RWLock();

    void rdLock();
    void wrLock();
    void unlock();
    bool unlockNoThrow();
};

class MutexLock : private NonCopyable
{
    Mutex& mutex;
    bool locked;

  public:
    explicit MutexLock(Mutex& m, bool doLock = true, bool locked_ = false)
      : mutex(m), locked(locked_)
    {
      if (doLock)
        lock();
    }

    ~MutexLock()
    {
      if (locked)
        mutex.unlockNoThrow();
    }

    void lock()
    {
      if (!locked)
      {
        mutex.lock();
        locked = true;
      }
    }

    void unlock()
    {
      if (locked)
      {
        mutex.unlock();
        locked = false;
      }
    }

    Mutex& getMutex()
      { return mutex; }
};

class RdLock : private NonCopyable
{
    RWLock& mutex;
    bool locked;

  public:
    explicit RdLock(RWLock& m, bool doLock = true, bool locked_ = false)
      : mutex(m), locked(locked_)
    {
      if (doLock)
        lock();
    }

    ~RdLock()
    {
      if (locked)
        mutex.unlockNoThrow();
    }

    void lock()
    {
      if (!locked)
      {
        mutex.rdLock();
        locked = true;
      }
    }

    void unlock()
    {
      if (locked)
      {
        mutex.unlock();
        locked = false;
      }
    }

    RWLock& getMutex()
      { return mutex; }
};

class WrLock : private NonCopyable
{
    RWLock& mutex;
    bool locked;

  public:
    explicit WrLock(RWLock& m, bool doLock = true, bool locked_ = false)
      : mutex(m), locked(locked_)
    {
      if (doLock)
        lock();
    }

    ~WrLock()
    {
      if (locked)
        mutex.unlockNoThrow();
    }

    void lock()
    {
      if (!locked)
      {
        mutex.wrLock();
        locked = true;
      }
    }

    void unlock()
    {
      if (locked)
      {
        mutex.unlock();
        locked = false;
      }
    }

    RWLock& getMutex()
      { return mutex; }
};

class Condition
{
    pthread_cond_t cond;

    // no implementation
    Condition(const Condition&);
    const Condition& operator= (const Condition&);

  public:
    Condition();
    ~Condition();

    void signal();
    void broadcast();
    void wait(MutexLock& lock);
    bool timedwait(MutexLock& lock, unsigned ms);
    bool timedwait(MutexLock& lock, const struct timespec& time);
};

//! @brief Spinmutex class.
/**
*  The most lightweight synchronisation object is the Spinlock. It is
*  usually implemented with a status variable that can be set to Locked
*  and Unlocked and atomic operations to change and inspect the status.
*  When Spinlock::lock is called, the status is changed to Locked.
*  Subsequent calls of Spinlock::lock from other threads will block until
*  the first thread has called Spinlock::unlock and the state of
*  the Spinlock has changed to Unlocked. Note that Spinlocks are not recursive.
*  When a Spinlock::lock blocks a busy-wait happens, therefore a Spinlock is only
*  usable in cases where resources need to be locked for a very short time, but in
*  these cases a higher performance can be achieved.
*/
class SpinMutex : private NonCopyable
{
    public:
        //! Default Constructor.
        SpinMutex()
        : _count(0)
        {}

        //! @brief Lock.
        /// Locks the Spinlock. If the Spinlock is currently locked
        /// by another thread, the calling thread suspends until no
        /// other thread holds a lock on it. This happens
        /// performing a  busy-wait. Spinlocks are not recursive
        /// locking it multiple times before unlocking it is undefined.
        void lock()
        {
            // busy loop until unlock
            while( atomicCompareExchange(_count, 1, 0) )
            {
                sched_yield();
            }
        }

        bool tryLock()
        {
           return ! atomicCompareExchange(_count, 1, 0);
        }

        //! @brief Unlock.
        /// Unlocks the Spinlock.
        void unlock()
        {
            // set unlocked
            atomicExchange(_count, 0);
        }

        /// @internal for unit test only
        bool testIsLocked() const
        { return _count != 0; }

    private:
        volatile atomic_t _count;
};


class SpinLock : private NonCopyable
{
    public:
        SpinLock(SpinMutex& m, bool doLock = true, bool locked = false)
        : _mutex(m)
        , _locked(locked)
        {
            if(doLock)
                this->lock();
        }

        ~SpinLock()
        {
            if(_locked)
                this->unlock();
        }

        void lock()
        {
            if( ! _locked )
            {
                _mutex.lock();
                _locked = true;
            }
        }

        void unlock()
        {
            if( _locked)
            {
                _mutex.unlock();
                _locked = false;
            }
        }

    private:
        SpinMutex& _mutex;
        bool _locked;
};

}

#endif // CXXTOOLS_MUTEX_H
