/* httprequest.cpp
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

#include <exception>
#include <iostream>
#include <cxxtools/httprequest.h>
#include <cxxtools/httpreply.h>
#include <cxxtools/arg.h>

int main(int argc, char* argv[])
{
  try
  {
    cxxtools::Arg<bool> raw(argc, argv, 'r');
    cxxtools::Arg<bool> debug(argc, argv, 'd');

    cxxtools::Arg<std::string> url("http://127.0.0.1/");
    url.set(argc, argv);

    // set parameters
    cxxtools::HttpRequest request(url);

    if (debug)
    {
      std::cout << "host: " << request.getHost() << "\n"
                   "port: " << request.getPort() << "\n"
                   "path: " << request.getUrl() << std::endl;
    }

    // execute request
    if (raw)
    {
      // execute request and read result including http-headers
      request.execute();
      std::cout << request.rdbuf() << std::flush;
    }
    else
    {
      // let httpreply parse the http-headers and read only body
      cxxtools::HttpReply reply(request);
      std::cout << "return-code " << reply.getReturnCode() << '\n'
                << reply.rdbuf() << std::flush;
    }
  }
  catch (const std::exception& e)
  {
    std::cerr << e.what() << std::endl;
  }
}

