#include <cxxtools/log.h>
#include <iostream>
#include <stdexcept>
#include <cxxtools/arg.h>

log_define("log");

namespace bench
{
  log_define("bench");
  void log(unsigned count, bool enabled)
  {
    if (enabled)
      for (int i = 0; i < count; ++i)
        log_fatal("message");
    else
      for (int i = 0; i < count; ++i)
        log_debug("info message");
  }
}

int main(int argc, char* argv[])
{
  try
  {
    log_init();

    cxxtools::arg<unsigned> count(argc, argv);
    if (count.isSet())
    {
      cxxtools::arg<bool> enable(argc, argv, 'e');
      log_fatal("start " << count.getValue());
      bench::log(count, enable);
      log_fatal("end");
    }
  }
  catch (const std::exception& e)
  {
    std::cerr << e.what() << std::endl;
    return -1;
  }
}

