#include <cxxtools/log.h>
#include <iostream>
#include <stdexcept>

log_define("log");

namespace ns
{
  log_define("log.ns");
  void main()
  {
    log_fatal("fatal message in namespace ns");
    log_error("error message in namespace ns");
    log_warn("warn message in namespace ns");
    log_info("info message in namespace ns");
    log_debug("debug message in namespace ns");
  }
}

int main(int argc, char* argv[])
{
  try
  {
    log_init();

    log_fatal("fatal message");
    log_error("error message");
    log_warn("warn message");
    log_info("info message");
    log_debug("debug message");

    ns::main();
  }
  catch (const std::exception& e)
  {
    std::cerr << e.what() << std::endl;
    return -1;
  }
}
