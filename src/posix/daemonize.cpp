/*
 * Copyright (C) 2013 Tommi Maekitalo
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

#include <cxxtools/posix/daemonize.h>
#include <cxxtools/posix/fork.h>
#include <cxxtools/systemerror.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <fstream>
#include <signal.h>

namespace cxxtools
{
namespace posix
{

namespace
{
std::string pidfile;

extern "C"
{
  void sigstop(int)
  {
    if (!pidfile.empty())
      ::unlink(pidfile.c_str());
    exit(0);
  }
}

}

void daemonize(const std::string& pidfile_)
{
  cxxtools::posix::Fork fork1;

  if (fork1.parent())
    exit(0);

  if (::freopen("/dev/null", "r", stdin) == 0)
    throw cxxtools::SystemError("freopen(stdin)");

  if (::freopen("/dev/null", "w", stdout) == 0)
    throw cxxtools::SystemError("freopen(stdout)");

  if (::freopen("/dev/null", "w", stderr) == 0)
    throw cxxtools::SystemError("freopen(stderr)");

  if (::setsid() == -1)
    throw cxxtools::SystemError("setsid");

  cxxtools::posix::Fork fork2;
  if (fork2.parent())
  {
    // don't wait but just exit
    fork2.setNowait();
    exit(0);
  }

  if (!pidfile_.empty())
  {
    std::ofstream f(pidfile_.c_str());
    f << getpid();
    if (f)
      pidfile = pidfile_;
    signal(SIGTERM, sigstop);
  }
}

}
}
