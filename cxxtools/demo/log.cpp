#include <cxxtools/log.h>
#include <cxxtools/loginit.h>
#include <iostream>
#include <stdexcept>

log_define("global");  // global definition

namespace ns
{
  log_define("namespace");  // namespace scope definition
  void main()
  {
    // this is logged at category "namespace"

    log_trace("main");

    log_fatal("fatal message in namespace ns");
    log_error("error message in namespace ns");
    log_warn("warn message in namespace ns");
    log_info("info message in namespace ns");
    log_debug("debug message in namespace ns");
  }
}

class cl
{
    log_define("class");  // class-scope definition

  public:
    cl()
    {
      log_debug("constructor here");  // this is logged at category "class"
    }
    ~cl()
    {
      log_debug("destructor here");
    }
};

int main(int argc, char* argv[])
{
  try
  {
    log_init();

    // this is logged at category "global"
    log_fatal("fatal message " << 1);
    log_error("error message " << 2);
    log_warn("warn message " << 3);
    log_info("info message " << 4);
    log_debug("debug message " << 5);

    ns::main();

    cl c;
  }
  catch (const std::exception& e)
  {
    std::cerr << e.what() << std::endl;
    return -1;
  }
}
