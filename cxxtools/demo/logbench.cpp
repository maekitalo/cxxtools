#include <cxxtools/log.h>
#include <cxxtools/loginit.h>
#include <iostream>
#include <stdexcept>
#include <cxxtools/arg.h>
#include <sys/time.h>
#include <iomanip>

log_define("log");

static const unsigned long loops = 1000;

namespace bench
{
  log_define("bench");
  void log(unsigned long count, bool enabled)
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
    cxxtools::arg<bool> enable(argc, argv, 'e');
    cxxtools::arg<double> total(argc, argv, 'T', 5.0);
    unsigned long count = 1;
    double T;

    log_init_fatal();

    while (count > 0)
    {
      std::cout << "count=" << count << "000" << '\t' << std::flush;
      struct timeval tv0;
      struct timeval tv1;
      gettimeofday(&tv0, 0);
      bench::log(count, enable);
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

