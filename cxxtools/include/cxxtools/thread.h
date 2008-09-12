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
 *
 */

#ifndef CXXTOOLS_THREAD_H
#define CXXTOOLS_THREAD_H

#include <pthread.h>
#include <signal.h>
#include <stdexcept>
#include <time.h>
#include <cxxtools/syserror.h>
#include <cxxtools/noncopyable.h>
#include <cxxtools/mutex.h>  // include for compatibility

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

}

#endif // CXXTOOLS_THREAD_H

