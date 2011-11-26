/*
 * Copyright (C) 2009,2011 Tommi Maekitalo
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
#include <cxxtools/arg.h>
#include <cxxtools/log.h>
#include <cxxtools/xmlrpc/service.h>
#include <cxxtools/http/server.h>
#include <cxxtools/bin/rpcserver.h>
#include <cxxtools/eventloop.h>

////////////////////////////////////////////////////////////////////////
// This defines functions, which we want to be called remotely.
//
std::string echo(const std::string& message)
{
  std::cout << message << std::endl;
  return message;
}

double add(double a1, double a2)
{
  return a1 + a2;
}

////////////////////////////////////////////////////////////////////////
// main
//
int main(int argc, char* argv[])
{
  try
  {
    log_init();

    cxxtools::Arg<std::string> ip(argc, argv, 'i');
    cxxtools::Arg<unsigned short> port(argc, argv, 'p', 7002);
    cxxtools::Arg<unsigned short> bport(argc, argv, 'b', 7003);

    std::cout << "run rpcecho server\n"
              << "xmlrpc protocol on port "<< port.getValue() << "\n"
              << "binary protocol on port " << bport.getValue() << std::endl;

    // create an event loop
    cxxtools::EventLoop loop;

    // the http server is instantiated with an ip address and a port number
    cxxtools::http::Server xmlrpcServer(loop, ip, port);

    // we create an instance of the service class
    cxxtools::xmlrpc::Service service;

    // we register our functions
    service.registerFunction("echo", echo);
    service.registerFunction("add", add);

    // ... and register the service under a url
    xmlrpcServer.addService("/myservice", service);

    // for the binary rpc server we define a binary server
    cxxtools::bin::RpcServer binServer(loop, ip, bport);

    // and register the functions in the server
    binServer.registerFunction("echo", echo);
    binServer.registerFunction("add", add);

    // now start the server and run the event loop
    loop.run();
  }
  catch (const std::exception& e)
  {
    std::cerr << e.what() << std::endl;
  }
}

