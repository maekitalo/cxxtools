/* cxxtools/thread.h
   Copyright (C) 2003-2005 Tommi Maekitalo

This file is part of cxxtools.

Cxxtools is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

Cxxtools is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with cxxtools; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330,
Boston, MA  02111-1307  USA
*/

#ifndef CXXTOOLS_THREAD_H
#define CXXTOOLS_THREAD_H

#include <pthread.h>
#include <semaphore.h>
#include <stdexcept>

namespace cxxtools
{

class ThreadException : public std::runtime_error
{
    int m_errno;

    static std::string formatMsg(const char* method, int e);

  public:
    ThreadException(const char* method, int e)
      : std::runtime_error(formatMsg(method, e)),
        m_errno(e)
      { }

    int getErrno() const
    { return m_errno; }
};

class Thread
{
    pthread_t      pthread;
    pthread_attr_t pthread_attr;
    static void* start(void* arg);

  public:
    Thread();
    virtual ~Thread();

    void Create();
    void Join();

  protected:
    virtual void Run() = 0;

  private:
};

template <typename function_type>
class FunctionThread : public Thread
{
    function_type& function;

  public:
    FunctionThread(function_type& f)
      : function(f)
      { }

  protected:
    virtual void Run()
    {
      function();
    }
};

template <typename object_type>
class MethodThread : public Thread
{
    object_type& object;
    void (object_type::*method)();

  public:
    MethodThread(object_type& a, void (object_type::*m)())
      : object(a),
        method(m)
      { }

  protected:
    virtual void Run()
    {
      (object.*method)();
    }
};

class Condition;

class Mutex
{
    friend class Condition;

    // make copy and assignment private without implementation
    Mutex(const Mutex&);
    const Mutex& operator= (const Mutex&);

  protected:
    pthread_mutex_t m_mutex;

  public:
    Mutex();
    ~Mutex();

    void Lock();
    bool tryLock();
    void Unlock();
};

class RWLock
{
    pthread_rwlock_t m_rwlock;

    // make copy and assignment private without implementation
    RWLock(const RWLock&);
    const RWLock& operator= (const RWLock&);

  public:
    RWLock();
    ~RWLock();

    void RdLock();
    void WrLock();
    void Unlock();
};

template <class mutex_type,
          void (mutex_type::*lock_method)() = &mutex_type::Lock,
          void (mutex_type::*unlock_method)() = &mutex_type::Unlock>
class LockBase
{
    mutex_type& mutex;
    bool locked;

    // make copy and assignment private without implementation
    LockBase(const LockBase&);
    const LockBase& operator= (const LockBase&);

  public:
    LockBase(mutex_type& m, bool lock = true)
      : mutex(m), locked(false)
    {
      if (lock)
        Lock();
    }

    ~LockBase()
    {
      if (locked)
        Unlock();
    }

    void Lock()
    {
      if (!locked)
      {
        (mutex.*lock_method)();
        locked = true;
      }
    }

    void Unlock()
    {
      if (locked)
      {
        (mutex.*unlock_method)();
        locked = false;
      }
    }

    mutex_type& getMutex()
      { return mutex; }
};

typedef LockBase<Mutex> MutexLock;
typedef LockBase<RWLock, &RWLock::RdLock> RdLock;
typedef LockBase<RWLock, &RWLock::WrLock> WrLock;

class Semaphore
{
    sem_t sem;

  public:
    Semaphore(unsigned value);
    ~Semaphore();

    void Wait();
    bool TryWait();
    void Post();
    int getValue();
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

    void Signal();
    void Broadcast();
    void Wait(MutexLock& lock);
};

}

#endif // CXXTOOLS_THREAD_H

