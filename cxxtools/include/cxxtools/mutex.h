/*
 * Copyright (C) 2005-2008 by Marc Boris Duerner
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
#ifndef cxxtools_Mutex_h
#define cxxtools_Mutex_h

#include <cxxtools/api.h>
#include <cxxtools/atomicity.h>
#include <cxxtools/noncopyable.h>
#include <cxxtools/thread.h>

namespace cxxtools {

/** @brief Mutual exclusion device.

    A Mutex is a mutual exclusion device. It is used to synchronize
    the access to data which is accessed by more than one thread or
    process at the same time. Mutexes are not recursive, that is the
    same thread can not lock a mutex multiple times without deadlocking.
*/
class CXXTOOLS_API Mutex : private NonCopyable
{
    private:
        class MutexImpl* _impl;

    public:
        //! @brief Default constructor.
        Mutex();

        /** @brief Destructor.

           The destructor destroys the mutex. The mutex must be in unlocked
           state when the destructor is called.
        */
        ~Mutex();

        /** @brief Lock the mutex.

            If the mutex is currently locked by another
            thread, the calling thread suspends until no other thread holds
            a lock on it. If the mutex is already locked by the calling
            thread a deadlock occurs.
        */
        void lock();

        bool tryLock();

        //! @brief Unlocks the mutex.
        void unlock();

        /** @brief Unlocks the mutex.

            This method does not throw an exception if unlocking the mutex
            fails but simply returns false.
        */
        bool unlockNoThrow();

        //! @internal @brief Access to platform specific implementation
        MutexImpl& impl()
        { return *_impl;  }
};

/** @brief MutexLock class for Mutex.

    The MutexLock class adds functionality for scoped
    locking. In the constructor of a  MutexLock, the mutex is locked
    and in the destructor it is unlocked. This way if for example an
    exception occures in the protected section the Mutex will be unlocked
    during stack unwinding when the MutexLock is destructed.

    @code
            // example how to make a member function thread-safe
            #include <cxxtools/System/Mutex.h>

            class MyClass
            {
                public:
                    void function()
                    {
                        MutexLock lock(_mtx);

                        //
                        // protected operations
                        //

                        // dtor of MutexLock unlocks _mtx
                    }

                private:
                    cxxtools::System::Mutex _mtx;
            };
    @endcode
*/
class MutexLock : private NonCopyable
{
    public:
        /** @brief Construct to guard a %Mutex

            Constructs a MutexLock object and locks the enclosing mutex
            if \a doLock is true. If \a isLocked is true, the %MutexLock
            will only unlock the given mutex in the destructor, but not
            lock it in the constructor.
        */
        MutexLock(Mutex& m, bool doLock = true, bool isLocked = false)
        : _mutex(m)
        , _isLocked(isLocked)
        {
            if(doLock)
                this->lock();
        }

        //! @brief Unlocks the mutex unless %unlock() was called
        ~MutexLock()
        {
            if(_isLocked)
                _mutex.unlockNoThrow();
        }

        void lock()
        {
            if(!_isLocked)
            {
                _mutex.lock();
                _isLocked = true;
            }
        }

        //! @brief Unlock so that the destructor does not unlock
        void unlock()
        {
            if(_isLocked)
            {
                _mutex.unlock();
                _isLocked = false;
            }
        }

        //! @brief Returns the guarded the mutex object
        Mutex& mutex()
        { return _mutex; }

        //! @brief Returns the guarded the mutex object
        const Mutex& mutex() const
        { return _mutex; }

        private:
            Mutex& _mutex;
            bool _isLocked;
};

/** @brief Recursive mutual exclusion device
*/
class CXXTOOLS_API RecursiveMutex : private NonCopyable
{
    private:
        class MutexImpl* _impl;

    public:
        RecursiveMutex();

        ~RecursiveMutex();

        void lock();

        bool tryLock();

        /** @brief Unlocks the mutex.

            If the mutex was locked more than one time by the
            same thread unlock decrements the lock-count. The mutex is actually
            unlocked when the lock-count is zero.
        **/
        void unlock();

        bool unlockNoThrow();
};

/** @brief Lock class for recursive mutexes.
*/
class RecursiveLock : private NonCopyable
{
    public:
        /** @brief Construct to guard a %RecursiveMutex

            Constructs a %RecursiveLock object and locks the enclosing
            recursive mutex if \a doLock is true. If \a isLocked is true,
            the %RecursiveLock will only unlock the given mutex in the
            destructor, but not lock it in the constructor.
        */
        RecursiveLock(RecursiveMutex& m, bool doLock = true, bool isLocked = false)
        : _mutex(m)
        , _isLocked(isLocked)
        {
            if(doLock)
                this->lock();
        }

        //! @brief Unlocks the mutex unless %unlock() was called
        ~RecursiveLock()
        {
            if(_isLocked)
                _mutex.unlockNoThrow();
        }

        void lock()
        {
            if(!_isLocked)
            {
                _mutex.lock();
                _isLocked = true;
            }
        }

        //! @brief Unlock so that the destructor does not unlock
        void unlock()
        {
            if(_isLocked)
            {
                _mutex.unlock();
                _isLocked = false;
            }
        }

        //! @brief Returns the guarded the mutex object
        RecursiveMutex& mutex()
        { return _mutex; }

        //! @brief Returns the guarded the mutex object
        const RecursiveMutex& mutex() const
        { return _mutex; }

        private:
            RecursiveMutex& _mutex;
            bool _isLocked;
};

/** @brief Synchronisation device similar to a POSIX rwlock

    A %ReadWriteMutex allows multiple concurrent readers or one exclusive writer to
    access a resource.
*/
class CXXTOOLS_API ReadWriteMutex : private NonCopyable
{
    public:
        //! @brief Creates the Reader/Writer lock.
        ReadWriteMutex();

        //! @brief Destroys the Reader/Writer lock.
        ~ReadWriteMutex();

        void readLock();
        /** @brief Acquires a read lock.

            If another thread currently holds a write lock, this method
            waits until the write lock is released.
        */
        bool tryReadLock();

        /** @brief Acquires a write lock.

            If one or more other threads currently hold locks, this method
            waits until all locks are released. The results are undefined
            if the same thread already holds a read or write lock.
        */
        void writeLock();

        /** @brief Tries to acquire a write lock.

            Immediately returns true if successful, or false if one or more
            other threads currently hold locks. The result is undefined if
            the same thread already holds a read or write lock.
        */
        bool tryWriteLock();

        //! @brief Releases the read or write lock.
        void unlock();

        bool unlockNoThrow();

    private:
        //! @internal
        class ReadWriteMutexImpl* _impl;
};


class ReadLock : private NonCopyable
{
    public:
        ReadLock(ReadWriteMutex& m, bool doLock = true, bool isLocked = false)
        : _mutex(m)
        , _locked(isLocked)
        {
            if(doLock)
                this->lock();
        }

        ~ReadLock()
        {
            if(_locked)
                _mutex.unlockNoThrow();
        }

        void lock()
        {
            if( ! _locked )
            {
                _mutex.readLock();
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

        ReadWriteMutex& mutex()
        { return _mutex; }

    private:
        ReadWriteMutex& _mutex;
        bool _locked;
};


class WriteLock : private NonCopyable
{
    public:
        WriteLock(ReadWriteMutex& m, bool doLock = true, bool isLocked = false)
        : _mutex(m)
        , _locked(isLocked)
        {
            if(doLock)
                this->lock();
        }

        ~WriteLock()
        {
            if(_locked)
                _mutex.unlockNoThrow();
        }

        void lock()
        {
            if( ! _locked )
            {
                _mutex.writeLock();
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

        ReadWriteMutex& mutex()
        { return _mutex; }

    private:
        ReadWriteMutex& _mutex;
        bool _locked;
};


/** @brief Spinmutex class.

   The most lightweight synchronisation object is the Spinlock. It is
   usually implemented with a status variable that can be set to Locked
   and Unlocked and atomic operations to change and inspect the status.
   When Spinlock::lock is called, the status is changed to Locked.
   Subsequent calls of Spinlock::lock from other threads will block until
   the first thread has called Spinlock::unlock and the state of
   the Spinlock has changed to Unlocked. Note that Spinlocks are not recursive.
   When a Spinlock::lock blocks a busy-wait happens, therefore a Spinlock is only
   usable in cases where resources need to be locked for a very short time, but in
   these cases a higher performance can be achieved.
*/
class SpinMutex : private NonCopyable
{
    public:
        //! @brief Default Constructor.
        SpinMutex()
        : _count(0)
        {}

        //! @brief Destructor.
        ~SpinMutex()
        {}


        /** @brief Lock.

            Locks the Spinlock. If the Spinlock is currently locked
            by another thread, the calling thread suspends until no
            other thread holds a lock on it. This happens
            performing a  busy-wait. Spinlocks are not recursive
            locking it multiple times before unlocking it is undefined.
        */
        inline void lock()
        {
            // busy loop until unlock
            while( atomicCompareExchange(_count, 1, 0) )
            {
                Thread::yield();
            }
        }

        bool tryLock()
        {
           return ! atomicCompareExchange(_count, 1, 0);
        }

        //! @brief Unlocks the Spinlock.
        void unlock()
        {
            // set unlocked
            atomicExchange(_count, 0);
        }

        //! @internal for unit test only
        bool testIsLocked() const
        { return _count != 0; }

private:
    volatile cxxtools::atomic_t _count;
};


class SpinLock : private NonCopyable
{
    public:
        SpinLock(SpinMutex& m, bool doLock = true, bool isLocked = false)
        : _mutex(m)
        , _locked(isLocked)
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

} // !namespace cxxtools

#endif
