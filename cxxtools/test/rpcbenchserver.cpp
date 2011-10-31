/*
 * Copyright (C) 2011 Tommi Maekitalo
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

#include <iostream>
#include <cxxtools/log.h>
#include <cxxtools/arg.h>
#include <cxxtools/eventloop.h>
#include <cxxtools/http/server.h>
#include <cxxtools/xmlrpc/service.h>

std::string echo(const std::string& msg)
{
  return msg;
}

int main(int argc, char* argv[])
{
  try
  {
    log_init("rpcbenchserver.properties");

    cxxtools::Arg<std::string> ip(argc, argv, 'i');
    cxxtools::Arg<unsigned short> port(argc, argv, 'p', 7002);
    cxxtools::Arg<unsigned> threads(argc, argv, 't', 4);
    cxxtools::Arg<unsigned> maxThreads(argc, argv, 'T', 200);

    std::cout << "rpc echo server running on port " << port.getValue() << "\n\n"
                 "options:\n\n"
                 "   -i ip      set interface address to listen on (default: all interfaces)\n"
                 "   -p number  set port number (default: 7002)\n"
                 "   -t number  set minimum number of threads (default: 4)\n"
                 "   -T number  set maximum number of threads (default: 200)\n"
              << std::endl;

    cxxtools::EventLoop loop;
    cxxtools::http::Server server(loop, ip, port);
    server.minThreads(threads);
    server.maxThreads(maxThreads);
    cxxtools::xmlrpc::Service service;
    service.registerFunction("echo", echo);
    server.addService("/myservice", service);

    loop.run();
  }
  catch (const std::exception& e)
  {
    std::cerr << e.what() << std::endl;
  }
}

