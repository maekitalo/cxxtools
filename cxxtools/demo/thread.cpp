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

class A : public cxxtools::AttachedThread
{
  public:
    ~A();

  protected:
    void run();
};

A::~A()
{
  PRINTLN("A::~A() called");
}

void A::run()
{
  PRINTLN("A is starting");

  cxxtools::MutexLock lock(conditionMutex);
  running.broadcast();
  lock.unlock();

  sleep(3);

  PRINTLN("A is ready");
}

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

    void aFunction()
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

    // Detached threads are created on the heap.
    // They delete themselves.
    cxxtools::Thread* d = new D;
    d->create();
    running.wait(lock);
    PRINTLN("D is running");

    // Non-detached threads are created on the stack.
    // They are joined, when block ends.
    A t;
    t.create();
    running.wait(lock);
    PRINTLN("A is running");

    // run a function as a Detached thread
    cxxtools::createThread(someFunction);

    // run a function as a Attached thread
    typedef void (*functionType)();
    functionType fn = someFunction;
    cxxtools::FunctionThread<> th(fn);
    th.create();

    // run a method of a object as a thread
    AClass aInstance("a instance");
    cxxtools::createThread(aInstance, &AClass::aFunction);
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
