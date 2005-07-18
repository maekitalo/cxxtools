/* log.cpp
   Copyright (C) 2003-2005 Tommi Maekitalo

This file is part of cxxtools.

Cxxtools is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

Cxxtools is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Cxxtools; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330,
Boston, MA  02111-1307  USA
*/

#include <cxxtools/log.h>
#include <cxxtools/loginit.h>
#include <iostream>
#include <stdexcept>

log_define("global")  // global definition

namespace ns
{
  log_define("namespace")  // namespace scope definition
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
    log_define("class")  // class-scope definition

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
