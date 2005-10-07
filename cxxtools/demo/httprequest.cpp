/* httprequest.cpp
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

#include <exception>
#include <iostream>
#include <cxxtools/httprequest.h>
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

