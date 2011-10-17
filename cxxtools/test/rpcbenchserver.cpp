/*
 * Copyright (C) 2011 Tommi Maekitalo
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

    cxxtools::Arg<unsigned short> port(argc, argv, 'p', 7002);

    cxxtools::EventLoop loop;
    cxxtools::http::Server server(loop, "", port);
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

