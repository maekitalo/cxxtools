/* log.cpp
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

log_define("global")  // global definition

namespace ns
{
  log_define("namespace")  // namespace scope definition
  void main1()
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
    log_define("class.cl")  // class-scope definition

  public:
    cl()
    {
      log_debug("constructor here");  // this is logged at category "class.cl"
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

    ns::main1();

    cl c;
  }
  catch (const std::exception& e)
  {
    std::cerr << e.what() << std::endl;
    return -1;
  }
}
