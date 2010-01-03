/*
 * Copyright (C) 2009 Tommi Maekitalo
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

This demo implements a rpc server and a rpc client.

To run the demo start the server with "rpcecho -S". It listens by default on
port 7002. It waits for incoming xmlrpc requests. The only service function
is echo, which takes a string as a parameter just returns that string.

To use the server run rpcecho with a text e.g. "rpcecho hello". It sends the
text to the server and prints the return string to standard out. You may pass
with -f <filename> a file, which will be sent to the server instead.

*/

#include <iostream>
#include <cxxtools/arg.h>
#include <cxxtools/loginit.h>
#include <cxxtools/xmlrpc/service.h>
#include <cxxtools/http/server.h>
#include <cxxtools/xmlrpc/remoteprocedure.h>
#include <cxxtools/xmlrpc/httpclient.h>
#include <cxxtools/clock.h>
#include <fstream>

////////////////////////////////////////////////////////////////////////
// This defines a xmlrpc service. A xmlrpc service defines functions, which
// can be called remotely.
//
class EchoServerService : public cxxtools::xmlrpc::Service
{
  public:
    EchoServerService()
    {
      registerMethod("echo", *this, &EchoServerService::echo);
    }

    std::string echo(const std::string& message, bool tostdout);
};

std::string EchoServerService::echo(const std::string& message, bool tostdout)
{
  if (tostdout)
    std::cout << message << std::endl;

  return message;
}

////////////////////////////////////////////////////////////////////////
// main
//
int main(int argc, char* argv[])
{
  try
  {
    log_init();

    cxxtools::Arg<bool> server(argc, argv, 'S');

    cxxtools::Arg<std::string> ip(argc, argv, 'i', server.isSet() ? "0.0.0.0" : "127.0.0.1");
    cxxtools::Arg<unsigned short> port(argc, argv, 'p', 7002);

    if (server)
    {
      std::cout << "run rpcecho server" << std::endl;

      // the http server is instantiated with a ip address and a port number
      cxxtools::http::Server server(ip, port);

      // we create an instance of the service class
      EchoServerService service;
      // ... and register it under a url
      server.addService("/myservice", service);

      // now run the server
      server.run();
    }
    else
    {
      std::cout << "run rpcecho client" << std::endl;

      // take option -n <number> to specify, how often the request should be called (1 by default)
      cxxtools::Arg<unsigned> number(argc, argv, 'c', 1);
      cxxtools::Arg<bool> doEcho(argc, argv, 'e');

      // define a xlmrpc client
      cxxtools::xmlrpc::HttpClient client(ip, port, "/myservice");

      // define remote procedure with std::string return value and a std::string and a bool parameter:
      cxxtools::xmlrpc::RemoteProcedure<std::string, std::string, bool> echo(client, "echo");

      // optionally pass a filename with -f
      cxxtools::Arg<const char*> filename(argc, argv, 'f');

      // option -n - do not output return value (good for benchmarking)
      cxxtools::Arg<bool> noout(argc, argv, 'n');

      cxxtools::Clock clock;
      clock.start();

      unsigned size = 0;

      if (filename.isSet())
      {
        // read a file into a stringstream
        std::ifstream in(filename);
        std::ostringstream data;
        data << in.rdbuf();  // read file into stringstream

        // send data from file to server and print reply to std::cout
        for (unsigned n = 0; n < number; ++n)
        {
          std::string v = echo(data.str(), doEcho);
          size += v.size();
          if (!noout)
            std::cout << v;
        }
      }
      else
      {
        // no filename given, so send just the parameters to the server.

        for (unsigned n = 0; n < number; ++n)
        {
          for (int a = 1; a < argc; ++a)
          {
            std::string v = echo(argv[a], doEcho);
            size += v.size();
            if (!noout)
              std::cout << v << '\n';
          }
        }
      }

      cxxtools::Timespan t = clock.stop();
      double T = t.toUSecs() / 1e6;
      unsigned kbytes = size / 1024;
      std::cerr << T << " s, " << (number.getValue() / T) << " msg/s\n"
                << kbytes << " kbytes, " << (kbytes / T) << " kbytes/s\n";
    }
  }
  catch (const std::exception& e)
  {
    std::cerr << e.what() << std::endl;
  }
}

