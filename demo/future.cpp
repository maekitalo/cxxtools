/*
 * Copyright (C) 2013 Tommi Maekitalo
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

