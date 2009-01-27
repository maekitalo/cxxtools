/*
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
 */

#include <cxxtools/log.h>
#include <cxxtools/loginit.h>
#include <cxxtools/smartptr.h>
#include <cxxtools/refcounted.h>
#include <iostream>
#include <vector>
#include <stdexcept>
#include <cxxtools/arg.h>
#include <cxxtools/thread.h>
#include <sys/time.h>
#include <iomanip>

log_define("log")

namespace bench
{
  log_define("bench")

  class Logtester : public cxxtools::RefCounted
  {
      cxxtools::AttachedThread thread;
      unsigned long count;
      unsigned long loops;
      unsigned long enabled;

    public:
      Logtester(unsigned long count_,
                unsigned long loops_,
                unsigned long enabled_)
        : thread( cxxtools::callable(*this, &Logtester::run) ),
          count(count_),
          loops(loops_),
          enabled(enabled_)
          { }

      void start()
      { thread.start(); }

      void join()
      { thread.join(); }

      void setCount(unsigned long count_)   { count = count_; }
      void setLoops(unsigned long loops_)   { loops = loops_; }
      void setEnabled(bool sw = true)       { enabled = sw; }

      void run();
  };

  void Logtester::run()
  {
    for (unsigned long l = 0; l < loops; ++l)
    {
      if (enabled)
        for (unsigned long i = 0; i < count; ++i)
          log_fatal("fatal message");
      else
        for (unsigned long i = 0; i < count; ++i)
          log_debug("info message");
    }
  }
}

int main(int argc, char* argv[])
{
  try
  {
    cxxtools::Arg<bool> enable(argc, argv, 'e');
    cxxtools::Arg<double> total(argc, argv, 'T', 5.0); // minimum runtime
    cxxtools::Arg<long> loops(argc, argv, 'l', 1000);
    cxxtools::Arg<unsigned> numthreads(argc, argv, 't', 1);

    unsigned long count = 1;
    double T;

    log_init();

    typedef std::vector<cxxtools::SmartPtr<bench::Logtester> > Threads;
    while (count > 0)
    {
      std::cout << "count=" << (count * loops) << '\t' << std::flush;

      struct timeval tv0;
      struct timeval tv1;

      {
        Threads threads;
        for (unsigned t = 0; t < numthreads; ++t)
          threads.push_back(new bench::Logtester(count, loops.getValue() / numthreads.getValue(), enable));

        gettimeofday(&tv0, 0);

        if (threads.size() == 1)
        {
          (*threads.begin())->run();
        }
        else
        {
          for (Threads::iterator it = threads.begin(); it != threads.end(); ++it)
            (*it)->start();
        }
      }

      gettimeofday(&tv1, 0);

      double t0 = tv0.tv_sec + tv0.tv_usec / 1e6;
      double t1 = tv1.tv_sec + tv1.tv_usec / 1e6;
      T = t1 - t0;

      std::cout.precision(6);
      std::cout << " T=" << T << '\t' << std::setprecision(12) << (count / T * loops)
        << " msg/s" << std::endl;

      if (T >= total)
        break;

      count <<= 1;
    }

  }
  catch (const std::exception& e)
  {
    std::cerr << e.what() << std::endl;
    return -1;
  }
}

