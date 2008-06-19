/* cxxtools/thread.h
 *
 * cxxtools - general purpose C++-toolbox
 * Copyright (C) 2003 Tommi Maekitalo
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#ifndef CXXTOOLS_THREAD_H
#define CXXTOOLS_THREAD_H

#include <pthread.h>
#include <signal.h>
#include <semaphore.h>
#include <stdexcept>
#include <time.h>
#include <cxxtools/syserror.h>
#include <cxxtools/noncopyable.h>

namespace cxxtools
{

/**
 Exception-class for problems with threads.

 This Exception is thrown in case of problems with threads.
 */
class ThreadException : public SysError
{
  public:
    ThreadException(int err, const char* method)
      : SysError(err, method)
      { }
};

/**
 Baseclass for threads.

 This is the base-class for Threads.
 */
class Thread
{
    Thread(const Thread&);
    Thread& operator=(const Thread&);

  protected:
    pthread_t      pthreadId;
    pthread_attr_t pthread_attr;

  public:
    Thread();
    virtual ~Thread();

    virtual void create() = 0;
    void kill(int signo = SIGTERM);

  protected:
    virtual void run() = 0;
};

/**
 Baseclass for attached threads.

 Attached threads, are threads, which are managed by the creator. A attached
 thread is created on the stack. On destruction, when the scope is leaved,
 the creator waits, until the thread is terminated.
 
 example:
 \code
 class MyThread : public cxxtools::AttachedThread
 {
   protected:
     void run();
 };

 void MyThread::run()
 {
   // implement, whatever need to be done in parallel
 }

 void someFunc()
 {
   {
     MyThread thread;
     thread.create();
     // here the thread runs and the program can do something in parallel
   }

   // here the thread is ready
 }
 \endcode

 */
class AttachedThread : public Thread
{
    static void* start(void* arg);
    bool joined;

  public:
    AttachedThread()
      : joined(false)
      { }

    /**
     The destructor joins the thread, if not already done.
     */
    virtual ~AttachedThread();

    /**
      Create the thread.

      After calling this method, the thread runs.
     */
    void create();

    /**
     Wait explicitely for the thread to come to an end.

     This is done automatically in the destructor if not already called.
     Therefore this method is rarely needed.
     */
    void join();
};

/**
 A detached thread manages itself.

 A detached thread runs just for its own. The user does not need (actually
 can't) wait for the thread to stop. The object is created on the heap.

 example:

 \code
 class MyThread : public cxxtools::DetachedThread
 {
   protected:
     void run();
 };

 void MyThread::run()
 {
   // implement, whatever need to be done in parallel
 }

 void someFunc()
 {
   MyThread *thread = new MyThread();
   thread->create();
   // here the thread runs and the program can do something in parallel.
   // it continues to run even after this function. The object is
   // automatically destroyed, when the thread is ready.
 }

 \endcode
 */
class DetachedThread : public Thread
{
    static void* start(void* arg);

  protected:
    // Detached threads delete themself at end.
    // Users must create them on the heap.
    ~DetachedThread()  { }

  public:
    DetachedThread();
    void create();
};

template <typename function_type = void (*)(), typename thread_type = AttachedThread>
class FunctionThread : public thread_type
{
    function_type& function;

  public:
    FunctionThread(function_type& f)
      : function(f)
      { }

  protected:
    virtual void run()
    {
      function();
    }
};

template <typename object_type, typename thread_type>
class MethodThread : public thread_type
{
    object_type& object;
    void (object_type::*method)();

  public:
    MethodThread(object_type& a, void (object_type::*m)())
      : object(a),
        method(m)
      { }

  protected:
    virtual void run()
    {
      (object.*method)();
    }
};

template <typename function_type>
void createThread(function_type& function)
{
  typedef FunctionThread<function_type, DetachedThread> ThreadType;
  ThreadType* thread = new ThreadType(function);
  try
  {
    thread->create();
  }
  catch (const ThreadException& e)
  {
    delete thread;
    throw;
  }
  // The thread deletes itself, so it is not returned here.
  // The caller can't be shure, when the object is deleted.
}

template <typename object_type>
void createThread(object_type& object, void (object_type::*method)())
{
  typedef MethodThread<object_type, DetachedThread> ThreadType;
  ThreadType* thread = new ThreadType(object, method);
  try
  {
    thread->create();
  }
  catch (const ThreadException& e)
  {
    delete thread;
    throw;
  }
  // The thread deletes itself, so it is not returned here.
  // The caller can't be shure, when the object is deleted.
}

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

template <class mutex_type,
          void (mutex_type::*lock_method)() = &mutex_type::lock,
          bool (mutex_type::*unlock_method)() = &mutex_type::unlockNoThrow>
class LockBase
{
    mutex_type& mutex;
    bool locked;

    // make copy and assignment private without implementation
    LockBase(const LockBase&);
    const LockBase& operator= (const LockBase&);

  public:
    LockBase(mutex_type& m, bool doLock = true)
      : mutex(m), locked(false)
    {
      if (doLock)
        lock();
    }

    ~LockBase()
    {
      if (locked)
        unlock();
    }

    void lock()
    {
      if (!locked)
      {
        (mutex.*lock_method)();
        locked = true;
      }
    }

    void unlock()
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

template <typename mutex_type = Mutex,
          bool (mutex_type::*unlock_method)() = &mutex_type::unlockNoThrow>
class UnlockMonitor : private NonCopyable
{
    mutex_type* mutex;

  public:
    explicit UnlockMonitor(mutex_type& mutex_)
      : mutex(&mutex_)
      { }

    ~UnlockMonitor()
    {
      if (mutex)
        (mutex->*unlock_method)();
    }

    void unlock()
    {
      (mutex->*unlock_method)();
      mutex = 0;
    }
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

class Semaphore
{
    sem_t sem;

  public:
    Semaphore(unsigned value);
    ~Semaphore();

    void wait();
    bool tryWait();
    void post();
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

    void signal();
    void broadcast();
    void wait(MutexLock& lock);
    void wait(LockBase<Mutex>& lock); // keep for binary compatibility to 1.4.8
    bool timedwait(MutexLock& lock, unsigned ms);
    bool timedwait(MutexLock& lock, const struct timespec& time);
};

}

#endif // CXXTOOLS_THREAD_H

