/*
 * Copyright (C) 2013 Tommi Maekitalo
 *
 */

#include <iostream>
#include <cxxtools/thread.h>
#include <cxxtools/threadpool.h>
#include <cxxtools/arg.h>
#include <cxxtools/log.h>
#include <iostream>

unsigned count = 0;

log_define("future")

// this simulates a long running task
void funct()
{
  log_info("wait");
  cxxtools::Thread::sleep(500);
  log_info("Hello " << ++count);
}

class AObject
{
    cxxtools::Timespan _waitTime;

  public:
    AObject(cxxtools::Seconds waitTime)
      : _waitTime(waitTime)
      { }

    void aMethod();
};

void AObject::aMethod()
{
  log_info("wait " << cxxtools::Seconds(_waitTime) << " in a method");
  cxxtools::Thread::sleep(_waitTime);
  log_info("Hello " << ++count);
}

// Just a simple task
void finish()
{
  log_info("finish task");
}

int main(int argc, char* argv[])
{
  try
  {
    log_init();

    cxxtools::Arg<unsigned> threads(argc, argv, 't', 5);
    cxxtools::Arg<unsigned> tasks(argc, argv, 'n', 17);

    // We create a thread pool with 5 threads by default
    cxxtools::ThreadPool p(threads);

    // Run the above function multiple times
    log_info("start tasks");
    for (unsigned n = 0; n < tasks; ++n)
        p.schedule(cxxtools::callable(funct));

    // Run the a method of a object instance multiple times
    AObject aObject(cxxtools::Milliseconds(400));
    for (unsigned n = 0; n < tasks; ++n)
        p.schedule(cxxtools::callable(aObject, &AObject::aMethod));

    // Run another job
    log_info("start finish task");
    cxxtools::ThreadPool::Future f = p.schedule(cxxtools::callable(finish));

    // We wait until the finish task is finished.
    // This does not mean, that all tasks are finished.
    // Others will continue to run on other threads.
    // We can also wait with just `f.wait()` without timeout.
    log_info("wait for finish task");
    bool b;
    do {
        b = f.wait(cxxtools::Milliseconds(300));
        log_info("future returns " << b);
    } while (b == false);

    log_info("finish task finished");

    // This stops the threads when all jobs are done.
    p.stop();

    log_info("tasks finished");
  }
  catch (const std::exception& e)
  {
    std::cerr << e.what() << std::endl;
  }
}

