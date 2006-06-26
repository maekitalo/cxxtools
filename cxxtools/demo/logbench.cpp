/* logbench.cpp
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

#include <cxxtools/log.h>
#include <cxxtools/loginit.h>
#include <iostream>
#include <stdexcept>
#include <cxxtools/arg.h>
#include <sys/time.h>
#include <iomanip>

log_define("log")

namespace bench
{
  log_define("bench")
  void log(unsigned long count, unsigned long loops, bool enabled)
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
    cxxtools::Arg<double> total(argc, argv, 'T', 5.0);
    cxxtools::Arg<long> loops(argc, argv, 'l', 1000);

    unsigned long count = 1;
    double T;

    log_init();

    while (count > 0)
    {
      std::cout << "count=" << (count * loops) << '\t' << std::flush;
      struct timeval tv0;
      struct timeval tv1;
      gettimeofday(&tv0, 0);
      bench::log(count, loops, enable);
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

