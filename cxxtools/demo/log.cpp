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

    log_fatal("fatal message " << 1);
    log_error("error message " << 2);
    log_warn("warn message " << 3);
    log_info("info message " << 4);
    log_debug("debug message " << 5);

    ns::main();
  }
  catch (const std::exception& e)
  {
    std::cerr << e.what() << std::endl;
    return -1;
  }
}
