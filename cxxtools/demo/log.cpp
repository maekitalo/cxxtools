#include <cxxtools/log.h>
#include <iostream>
#include <stdexcept>

log_define("log-demo");

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
  }
  catch (const std::exception& e)
  {
    std::cerr << e.what() << std::endl;
    return -1;
  }
}
