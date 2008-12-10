/*
 * Copyright (C) 2005 Tommi Maekitalo
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

#include <cxxtools/log.h>
#include <cxxtools/loginit.h>
#include <cxxtools/arg.h>
#include <iostream>

const char* category = "logsh";
log_define(category)

int main(int argc, char* argv[])
{
  try
  {
    cxxtools::Arg<bool> fatal;
    fatal.set(argc, argv, 'f');
    fatal.set(argc, argv, "--fatal");

    cxxtools::Arg<bool> error;
    error.set(argc, argv, 'e');
    error.set(argc, argv, "--error");

    cxxtools::Arg<bool> warn;
    warn.set(argc, argv, 'w');
    warn.set(argc, argv, "--warn");

    cxxtools::Arg<bool> info;
    info.set(argc, argv, 'i');
    info.set(argc, argv, "--info");

    cxxtools::Arg<bool> debug;
    debug.set(argc, argv, 'd');
    debug.set(argc, argv, "--debug");

    cxxtools::Arg<std::string> properties("log4j.properties");
    properties.set(argc, argv, 'p');
    properties.set(argc, argv, "--properties");

    if (argc <= 2)
    {
      std::cerr << "usage: " << argv[0] << " [options] category message\n"
                   "\toptions:  -f|--fatal\n"
                   "\t          -e|--error\n"
                   "\t          -w|--warn\n"
                   "\t          -i|--info\n"
                   "\t          -d|--debug\n"
                   "\t          -p|--properties filename" << std::endl;
      return -1;
    }

    log_init(properties.getValue());

    category = argv[1];

    for (int a = 2; a < argc; ++a)
    {
      if (fatal)
        log_fatal(argv[a]);
      else if (error)
        log_error(argv[a]);
      else if (warn)
        log_warn(argv[a]);
      else if (info)
        log_info(argv[a]);
      else if (debug)
        log_debug(argv[a]);
    }
  }
  catch (const std::exception& e)
  {
    std::cerr << e.what() << std::endl;
    return -1;
  }
}

