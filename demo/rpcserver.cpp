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

/*

  Cxxtools implements a rpc framework, which can run with 4 protocols.

  Xmlrpc is a simple xml based standard protocol. The spec can be found at
  http://www.xmlrpc.org/.

  Json rpc is a json based standard protocol. The sepc can be found at
  http://json-rpc.org. Json is less verbose than xml and therefore jsonrpc is a
  little faster than xmlrpc.

  Json rpc comes in cxxtools in 2 flavours: raw and http.

  And last but not least cxxtools has a own non standard binary protocol. It is
  faster than xmlrpc or jsonrpc but works only with cxxtools.

  This demo program implements a server, which runs all 4 flavours in a single
  event loop.

 */

#include <iostream>
#include <cxxtools/arg.h>
#include <cxxtools/log.h>
#include <cxxtools/xmlrpc/service.h>
#include <cxxtools/http/server.h>
#include <cxxtools/bin/rpcserver.h>
#include <cxxtools/json/rpcserver.h>
#include <cxxtools/json/httpservice.h>
#include <cxxtools/eventloop.h>
#include <cxxtools/posix/daemonize.h>

////////////////////////////////////////////////////////////////////////
// This defines functions, which we want to be called remotely.
//

// Parameters and return values of the functions, which can be exported must be
// serializable and deserializable with the cxxtools serialization framework.
// For all standard types including container classes in the standard library
// proper operators are defined in cxxtools.
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
    // initialize logging - this reads the file log.xml from the current directory
    log_init();

    // read the command line options

    // option -i <ip-address> defines the ip address of the interface, where the
    // server waits for connections. Default is empty, which tells the server to
    // listen on all local interfaces
    cxxtools::Arg<std::string> ip(argc, argv, 'i');

    // option -p <number> specifies the port, where http requests are expected.
    // This port is valid for xmlrpc and json over http. It defaults to 7002.
    cxxtools::Arg<unsigned short> port(argc, argv, 'p', 7002);

    // option -b <number> specifies the port, where the binary server waits for
    // requests. It defaults to port 7003.
    cxxtools::Arg<unsigned short> bport(argc, argv, 'b', 7003);

    // option -j <number> specifies the port, where the json server wait for
    // requests. It defaults to port 7004.
    cxxtools::Arg<unsigned short> jport(argc, argv, 'j', 7004);

    cxxtools::Arg<bool> daemonize(argc, argv, 'd');
    cxxtools::Arg<std::string> pidfile(argc, argv, "--pidfile");

    cxxtools::Arg<std::string> sslCert(argc, argv, 'c');

    std::cout << "run rpcecho server\n"
              << "http protocol on port "<< port.getValue() << "\n"
              << "binary protocol on port " << bport.getValue() << "\n"
              << "json protocol on port " << jport.getValue() << std::endl;

    // create an event loop
    cxxtools::EventLoop loop;

    // the http server is instantiated with an ip address and a port number
    // It will be used for xmlrpc and json over http on different urls.
    cxxtools::http::Server httpServer(loop, ip, port);

    ////////////////////////////////////////////////////////////////////////
    // Xmlrpc

    // we create an instance of the service class
    cxxtools::xmlrpc::Service xmlrpcService;

    // we register our functions
    xmlrpcService.registerFunction("echo", echo);
    xmlrpcService.registerFunction("add", add);

    // ... and register the service under a url
    httpServer.addService("/xmlrpc", xmlrpcService);

    ////////////////////////////////////////////////////////////////////////
    // Binary rpc

    // for the binary rpc server we define a binary server
    cxxtools::bin::RpcServer binServer(loop, ip, bport, sslCert);

    // and register the functions in the server
    binServer.registerFunction("echo", echo);
    binServer.registerFunction("add", add);

    ////////////////////////////////////////////////////////////////////////
    // Json rpc

    // for the json rpc server we define a json server
    cxxtools::json::RpcServer jsonServer(loop, ip, jport, sslCert);

    // and register the functions in the server
    jsonServer.registerFunction("echo", echo);
    jsonServer.registerFunction("add", add);

    ////////////////////////////////////////////////////////////////////////
    // Json rpc over http

    // for json over http we need a service object
    cxxtools::json::HttpService jsonhttpService;

    // we register our functions
    jsonhttpService.registerFunction("echo", echo);
    jsonhttpService.registerFunction("add", add);
    // ... and register the service under a url
    httpServer.addService("/jsonrpc", jsonhttpService);

    // go to the background if requested
    if (daemonize)
      cxxtools::posix::daemonize(pidfile);

    ////////////////////////////////////////////////////////////////////////
    // set ssl
    //
    if (sslCert.isSet())
    {
        httpServer.loadSslCertificateFile(sslCert);
    }

    ////////////////////////////////////////////////////////////////////////
    // Run

    // now start the servers by running the event loop
    loop.run();
  }
  catch (const std::exception& e)
  {
    std::cerr << e.what() << std::endl;
  }
}

