/*
 * Copyright (C) 2003,2009 Tommi Maekitalo
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

#include <exception>
#include <iostream>
#include <cxxtools/http/client.h>
#include <cxxtools/arg.h>
#include <cxxtools/loginit.h>

int main(int argc, char* argv[])
{
  try
  {
    log_init();

    cxxtools::Arg<bool> headers(argc, argv, 'h');
    cxxtools::Arg<bool> returncode(argc, argv, 'c');
    cxxtools::Arg<std::string> user(argc, argv, 'u');   // TODO auth
    cxxtools::Arg<std::string> server(argc, argv, 's', "127.0.0.1");
    cxxtools::Arg<unsigned short int> port(argc, argv, 'p', 80);

    //std::string::size_type p = user.getValue().find('/');
    //if (p != std::string::npos)
      //request.setAuth(user.getValue().substr(0, p),
                      //user.getValue().substr(p + 1));

    cxxtools::http::Client client(server, port);

    for (int a = 1; a < argc; ++a)
        std::cout << client.get(argv[a]);
  }
  catch (const std::exception& e)
  {
    std::cerr << e.what() << std::endl;
  }
}

