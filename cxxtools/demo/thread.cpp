#include <cxxtools/thread.h>
#include <iostream>
#include <unistd.h>

cxxtools::Mutex coutMutex;
cxxtools::Mutex conditionMutex;
cxxtools::Condition running;

#define PRINTLN(expr) \
  do { \
    cxxtools::MutexLock lock(coutMutex); \
    std::cout << time(0) << ' ' << expr << std::endl; \
  } while (false)

class T : public cxxtools::AttachedThread
{
  public:
    ~T();

  protected:
    void run();
};

T::~T()
{
  PRINTLN("T::~T() called");
}

void T::run()
{
  PRINTLN("T is starting");

  cxxtools::MutexLock lock(conditionMutex);
  running.broadcast();
  lock.unlock();

  sleep(3);

  PRINTLN("T is ready");
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
    T t;
    t.create();
    running.wait(lock);
    PRINTLN("T is running");

    // run a function as a Detached thread
    cxxtools::createThread(someFunction);

    // run a function as a Attached thread
    typedef void (*functionType)();
    functionType fn = someFunction;
    cxxtools::FunctionThread<functionType> th(fn);
    th.create();

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
