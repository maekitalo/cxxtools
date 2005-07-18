/* netmsg.cpp
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

************************************************************************

   usage: sender:   netmsg [-h host] [-p port] {message}
          receiver: netmsg -r [-l length] [-c]

*/

#include "cxxtools/udp.h"
#include "cxxtools/dynbuffer.h"
#include "cxxtools/arg.h"

void usage(const char* progname)
{
  std::cerr << "usage: " << progname << " [-h host] [-p port] {message}\n"
               "       " << progname << " -r [-h host] [-l length] [-c]" << std::endl;
}

int main(int argc, char* argv[])
{
  try
  {
    cxxtools::Arg<bool> help(argc, argv, '?');
    if (help)
    {
      usage(argv[0]);
      return 0;
    }

    cxxtools::Arg<bool> receive(argc, argv, 'l');
    cxxtools::Arg<unsigned short> port(argc, argv, 'p', 1234);

    if (receive)
    {
      cxxtools::Arg<unsigned> size(argc, argv, 's', 1024);
      cxxtools::Arg<const char*> host(argc, argv, 'h', "0.0.0.0");
      cxxtools::Arg<bool> continuous(argc, argv, 'c');
      cxxtools::Arg<bool> nonewline(argc, argv, 'n');

      if (argc > 1)
      {
        usage(argv[0]);
        return -1;
      }

      cxxtools::net::UdpReceiver receiver(host, port);
      cxxtools::Dynbuffer<char> buffer(size);

      std::cout << "waiting for messages on port " << port << std::endl;
      do
      {
        cxxtools::net::UdpReceiver::size_type s = receiver.recv(buffer.data(), size);
        std::cout << std::string(buffer.data(), s);
        if (!nonewline)
          std::cout << std::endl;
        else
          std::cout.flush();
      } while (continuous);
    }
    else
    {
      cxxtools::Arg<const char*> host(argc, argv, 'h', "127.0.0.1");

      if (argc == 1)
      {
        usage(argv[0]);
        return -1;
      }


      cxxtools::net::UdpSender sender(host, port);
      for (int a = 1; a < argc; ++a)
      {
        std::cout << "send message \"" << argv[a] << "\" to " << host << ':' << port << std::endl;
        sender.send(argv[a]);
      }
    }
  }
  catch (const std::exception& e)
  {
    std::cerr << e.what() << std::endl;
  }
}
