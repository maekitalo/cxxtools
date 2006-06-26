/* netmsg.cpp
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

#include <cxxtools/udp.h>
#include <cxxtools/dynbuffer.h>
#include <cxxtools/arg.h>
#include <cxxtools/loginit.h>
#include <iostream>

void usage(const char* progname)
{
  std::cerr << "usage: " << progname << " [-h host] [-p port] [-e] {message}\n"
               "       " << progname << " -l [-h host] [-p port] [-s size] [-c] [-e] [-n]\n"
               "options:\n"
               "  -l             receiver-mode\n"
               "  -h host        hostname (default 127.0.0.1 in sender, 0.0.0.0 in receiver)\n"
               "  -p port        udp-port to use\n"
               "  -s size        size of receive-buffer in bytes (default 1024)\n"
               "  -c             continuous-mode - don't stop after receiving message\n"
               "  -e             echo message back or receive echo-reply\n"
               "  -n             don't output newline"
            << std::endl;
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
    cxxtools::Arg<bool> echo(argc, argv, 'e');
    cxxtools::Arg<bool> nonewline(argc, argv, 'n');
    cxxtools::Arg<int> timeout(argc, argv, 't', 0);

    log_init();

    if (receive)
    {
      cxxtools::Arg<unsigned> size(argc, argv, 's', 1024);
      cxxtools::Arg<const char*> host(argc, argv, 'h', "0.0.0.0");
      cxxtools::Arg<bool> continuous(argc, argv, 'c');

      if (argc > 1)
      {
        usage(argv[0]);
        return -1;
      }

      cxxtools::net::UdpReceiver receiver(host, port);
      if (timeout.isSet())
        receiver.setTimeout(timeout);

      cxxtools::Dynbuffer<char> buffer(size);

      std::cout << "waiting for messages on port " << port << std::endl;
      do
      {
        cxxtools::net::UdpReceiver::size_type s = receiver.recv(buffer.data(), size);
        std::string msg(buffer.data(), s);
        std::cout << msg;
        if (!nonewline)
          std::cout << std::endl;
        else
          std::cout.flush();

        if (echo)
          receiver.send(msg);

      } while (continuous);
    }
    else
    {
      cxxtools::Arg<const char*> host(argc, argv, 'h', "127.0.0.1");

      if (argc <= 1)
      {
        usage(argv[0]);
        return -1;
      }

      cxxtools::net::UdpSender sender(host, port);
      if (timeout.isSet())
        sender.setTimeout(timeout);

      for (int a = 1; a < argc; ++a)
      {
        std::string msg = argv[a];
        std::cout << "send message \"" << msg << "\" to " << host << ':' << port << std::endl;
        cxxtools::net::UdpSender::size_type n = sender.send(msg);
        std::cout << n << " bytes sent " << msg.size() << " bytes queued" << std::endl;

        if (echo)
        {
          std::string reply = sender.recv(msg.size());
          std::cout << "reply: " << reply;
          if (!nonewline)
            std::cout << std::endl;
          else
            std::cout.flush();
        }
      }
    }
  }
  catch (const std::exception& e)
  {
    std::cerr << e.what() << std::endl;
  }
}
