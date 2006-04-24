/* netcat.cpp
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
#include <cxxtools/tcpstream.h>
#include <cxxtools/arg.h>
#include <cxxtools/loginit.h>

int main(int argc, char* argv[])
{
  try
  {
    log_init();

    cxxtools::Arg<const char*> ip(argc, argv, 'i', "0.0.0.0");
    cxxtools::Arg<unsigned short> port(argc, argv, 'p', 1234);
    cxxtools::Arg<unsigned> bufsize(argc, argv, 'b', 8192);
    cxxtools::Arg<bool> listen(argc, argv, 'l');
    cxxtools::Arg<bool> read_reply(argc, argv, 'r');

    if (listen)
    {
      // I'm a server

      // listen to a port
      cxxtools::net::Server server(ip.getValue(), port);

      // accept a connetion
      cxxtools::net::iostream worker(server, bufsize);

      // copy to stdout
      std::cout << worker.rdbuf();
    }
    else
    {
      // I'm a client

      // connect to server
      cxxtools::net::iostream peer(ip, port, bufsize);

      // copy stdin to server
      peer << std::cin.rdbuf() << std::flush;

      if (read_reply)
        // copy answer to stdout
        std::cout << peer.rdbuf() << std::flush;
    }
  }
  catch (const std::exception& e)
  {
    std::cerr << e.what() << std::endl;
  }
}

