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
#include <cxxtools/http/request.h>
#include <cxxtools/arg.h>
#include <cxxtools/eventloop.h>
#include <cxxtools/loginit.h>

std::size_t printer(cxxtools::http::Client& client)
{
  char buffer[8192];
  std::streamsize n = client.in().readsome(buffer, sizeof(buffer));
  std::cout.write(buffer, n);
  return n;
}

cxxtools::EventLoop loop;

void ready(cxxtools::http::Client& client)
{
  loop.exit();
}

void error(cxxtools::http::Client& client, const std::exception& e)
{
  throw;
}

int main(int argc, char* argv[])
{
  try
  {
    log_init();

    cxxtools::Arg<bool> headers(argc, argv, 'h');
    cxxtools::Arg<bool> returncode(argc, argv, 'c');
    cxxtools::Arg<std::string> user(argc, argv, 'u'); // passed as "username:password"
    cxxtools::Arg<std::string> server(argc, argv, 's', "127.0.0.1");
    cxxtools::Arg<unsigned short int> port(argc, argv, 'p', 80);
    cxxtools::Arg<bool> async(argc, argv, 'a');

    cxxtools::http::Client client(server, port);

    if (user.isSet())
    {
      std::string::size_type p = user.getValue().find(':');
      if (p != std::string::npos)
      {
        client.auth(user.getValue().substr(0, p),
                    user.getValue().substr(p + 1));
      }
      else
      {
        client.auth(user, std::string());
      }
    }

    if (async)
    {
      connect(client.bodyAvailable, printer);
      connect(client.replyFinished, ready);
      connect(client.errorOccured, error);
      client.setSelector(loop);
    }

    for (int a = 1; a < argc; ++a)
    {
      if (async)
      {
        cxxtools::http::Request request(argv[a]);
        client.beginExecute(request);
        loop.run();
      }
      else
        std::cout << client.get(argv[a]);
    }
  }
  catch (const std::exception& e)
  {
    std::cerr << "ERROR: " << e.what() << std::endl;
  }
}

