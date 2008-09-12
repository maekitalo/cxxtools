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
 *
 */

#include <exception>
#include <iostream>
#include <list>
#include <cxxtools/httprequest.h>
#include <cxxtools/httpreply.h>
#include <cxxtools/arg.h>

int main(int argc, char* argv[])
{
  try
  {
    cxxtools::Arg<bool> raw(argc, argv, 'r');
    cxxtools::Arg<bool> headers(argc, argv, 'h');
    cxxtools::Arg<bool> returncode(argc, argv, 'c');
    cxxtools::Arg<bool> body(argc, argv, 'b');
    cxxtools::Arg<bool> debug(argc, argv, 'd');
    cxxtools::Arg<std::string> user(argc, argv, 'u');

    cxxtools::Arg<std::string> url("http://127.0.0.1/");
    url.set(argc, argv);

    // set parameters
    cxxtools::HttpRequest request(url);

    std::string::size_type p = user.getValue().find('/');
    if (p != std::string::npos)
      request.setAuth(user.getValue().substr(0, p),
                      user.getValue().substr(p + 1));

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

      if (headers)
      {
        typedef std::list<std::string> headerlistType;
        headerlistType headerlist;
        reply.getHeaders(std::back_inserter(headerlist));
        for (headerlistType::iterator it = headerlist.begin();
             it != headerlist.end(); ++it)
          std::cout << *it << ":\t" << reply.getHeader(*it) << '\n';
        std::cout << std::endl;
      }

      if (returncode)
        std::cout << "return-code " << reply.getReturnCode() << '\n';

      if (!headers || body)
        std::cout << reply.rdbuf();

      std::cout.flush();
    }
  }
  catch (const std::exception& e)
  {
    std::cerr << e.what() << std::endl;
  }
}

