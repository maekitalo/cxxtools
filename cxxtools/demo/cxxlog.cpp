/*
 * Copyright (C) 2008 Tommi Maekitalo
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * is provided AS IS, WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, and
 * NON-INFRINGEMENT.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 *
 */

#include <iostream>
#include <algorithm>
#include <iterator>
#include <cxxtools/arg.h>
#include <cxxtools/log.h>
#include <cxxtools/loginit.h>


// Normally the category parameter is a constant.
//
// log_define defines a function, which fetches a logger class using that
// constant. Since this is done only the first time something is logged, it is
// important to set the category before the first logging output statement.
//
std::string category;
log_define(category)

int main(int argc, char* argv[])
{
  try
  {
    cxxtools::Arg<bool> fatal(argc, argv, 'f');
    cxxtools::Arg<bool> fatal_l(argc, argv, "--fatal");
    cxxtools::Arg<bool> error(argc, argv, 'e');
    cxxtools::Arg<bool> error_l(argc, argv, "--error");
    cxxtools::Arg<bool> warn(argc, argv, 'w');
    cxxtools::Arg<bool> warn_l(argc, argv, "--warn");
    cxxtools::Arg<bool> info(argc, argv, 'i');
    cxxtools::Arg<bool> info_l(argc, argv, "--info");
    cxxtools::Arg<bool> debug(argc, argv, 'd');
    cxxtools::Arg<bool> debug_l(argc, argv, "--debug");
    cxxtools::Arg<std::string> properties(argc, argv, 'p', "log.properties");
    cxxtools::Arg<std::string> properties_l(argc, argv, "--properties", properties);

    if (argc <= 2)
    {
      std::cerr << "usage: " << argv[0] << " options category message\n"
                   "\toptions:  -f|--fatal\n"
                   "\t          -e|--error\n"
                   "\t          -w|--warn\n"
                   "\t          -i|--info\n"
                   "\t          -d|--debug\n"
                   "\t          -p|--properties filename" << std::endl;
      return -1;
    }

    category = argv[1];

    log_init(properties_l.getValue());

    std::ostringstream msg;
    std::copy(argv + 2,
              argv + argc,
              std::ostream_iterator<char*>(msg, " "));

    if (fatal || fatal_l)
      log_fatal(msg.str());

    if (error || error_l)
      log_error(msg.str());

    if (warn || warn_l)
      log_warn(msg.str());

    if (info || info_l)
      log_info(msg.str());

    if (debug || debug_l)
      log_debug(msg.str());
  }
  catch (const std::exception& e)
  {
    std::cerr << e.what() << std::endl;
  }
}

