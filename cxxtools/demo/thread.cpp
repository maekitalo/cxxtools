/* thread.cpp
 * demostrate usage of thread-classes
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

#include <cxxtools/function.h>
#include <cxxtools/thread.h>
#include <cxxtools/mutex.h>
#include <iostream>
#include <string>
#include <unistd.h>

cxxtools::Mutex coutMutex;
cxxtools::Mutex conditionMutex;
cxxtools::Condition running;

#define PRINTLN(expr) \
  do { \
    cxxtools::MutexLock lock(coutMutex); \
    std::cout << time(0) << ' ' << expr << std::endl; \
  } while (false)

class D : public cxxtools::DetachedThread
{
    ~D();

  protected:
    void run();
};

D::~D()
{
  PRINTLN("D::~D() called");
}

void D::run()
{
  PRINTLN("D is starting");

  cxxtools::MutexLock lock(conditionMutex);
  running.broadcast();
  lock.unlock();

  sleep(2);
  PRINTLN("D is ready");
}

void someFunction()
{
  PRINTLN("someFunction()");
  sleep(1);
  PRINTLN("someFunction() ends");
}

class AClass
{
    std::string id;
    cxxtools::Mutex readyMutex;

  public:
    AClass(const std::string& id_)
      : id(id_)
      {
        readyMutex.lock();
      }
    ~AClass()
      { PRINTLN("AClass::~AClass of object \"" << id << '"'); }

    void waitReady()
    {
      readyMutex.lock();
    }

    void run()
    {
      PRINTLN("aFunction() of object \"" << id << '"');
      sleep(1);
      PRINTLN("aFunction() of object \"" << id << "\" ends");
      readyMutex.unlock();
    }
};

int main()
{
  try
  {
    cxxtools::MutexLock lock(conditionMutex);

    // detached threads are created on the heap.
    // They delete themselves.
    cxxtools::Thread* d = new D;
    d->create();
    running.wait(lock);
    PRINTLN("D is running");

    // run a function as a detached thread
    cxxtools::Thread* dt = new cxxtools::DetachedThread(someFunction);
    dt->start();

    // run a function as a attached thread
    cxxtools::AttachedThread th( cxxtools::callable(someFunction) );
    th.start();

    // run a method of a object as a thread
    AClass aInstance("a instance");
    cxxtools::AttachedThread aclassThread( cxxtools::callable(aInstance, &AClass::run) );
    sleep(2);
    aInstance.waitReady();

    // The detached thread is killed, if it does not come to an end before main.
    // The attached thread blocks the main-program from stopping, until it is ready.
    PRINTLN("main stops");
  }
  catch (const std::exception& e)
  {
    std::cerr << "ERROR: " << e.what() << std::endl;
  }

  std::cout << "main stopped" << std::endl;
}
