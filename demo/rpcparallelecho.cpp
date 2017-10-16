/*
 * Copyright (C) 2013 Tommi Maekitalo
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
   This program demonstrates a way to issue multiple parallel rpc calls easily
   without using threads or callbacks.

   This example uses binary rpc but it is easy to replace the protocol with
   xmlrpc, json raw or json over http.
 */

#include <iostream>
#include <vector>
#include <cxxtools/arg.h>
#include <cxxtools/log.h>
#include <cxxtools/remoteprocedure.h>
#include <cxxtools/bin/rpcclient.h>
#include <cxxtools/selector.h>
#include <cxxtools/smartptr.h>

////////////////////////////////////////////////////////////////////////
// main
//
int main(int argc, char* argv[])
{
  try
  {
    log_init();

    // Switch -i <ip> specifies the ip address of the server; empty means localhost.
    cxxtools::Arg<std::string> ip(argc, argv, 'i');

    // Switch -p <port> is used to set the port of the server.
    cxxtools::Arg<unsigned short> port(argc, argv, 'p', 7003);

    // Switch -c <number> sets the number of parallel calls to execute. Default is 1.
    cxxtools::Arg<unsigned> count(argc, argv, 'c', 1);

    cxxtools::Arg<bool> ssl(argc, argv, 's');

    typedef cxxtools::RemoteProcedure<std::string, std::string> Echo;

    // We define vectors for the clients and remote procedures
    // One client can run only one request at one time hence we need one client
    // for each request, we plan to execute in parallel
    std::vector<cxxtools::bin::RpcClient> clients;
    std::vector<Echo> echo;

    // The cxxtools::RemoteProcedure holds a reference to his remote client
    // and we have to make sure, that exactly that instance of the remote
    // client is valid while the remote procedure is executed.
    // To achive that we reserve space for enough elements in the vector so
    // that the remote clients do not need to be copied on reallocation.
    clients.reserve(count);

    // The selector is a abstraction of the poll system call. This manages the
    // asynchronous event processing.
    cxxtools::Selector selector;

    for (unsigned n = 0; n < count; ++n)
    {
      // We instantiate a remote client and pass the selector to him
      clients.push_back(cxxtools::bin::RpcClient(selector, ip, port, ssl));

      // ... and a remote procedure object.
      echo.push_back(Echo(clients.back(), "echo"));
    }

    // process the command line arguments
    for (int a = 1; a < argc; ++a)
    {
      // For each remote procedure we call the begin method, which signals the
      // selector, that we want to run the remote procedure.

      for (unsigned n = 0; n < count; ++n)
        echo[n].begin(argv[a]);

      for (unsigned n = 0; n < count; ++n)
      {
        // For each client execute the event loop until the client is finished.
        // activeProcedure returns a pointer to our Echo object as long as it
        // is running in the event loop. Note that calling wait on the selector
        // handles all events for all clients although we wait for one specific
        // client to finish.
        while (clients[n].activeProcedure())
          selector.wait();

        // Since the nth client has finished processing, his corresponding
        // remote procedure is finished. We fetch the result using the result()
        // method. Note that the result() method may throw an exception when
        // the remote function throwed an exception on the server side.
        std::cout << echo[n].result() << '\n';
      }
    }
  }
  catch (const std::exception& e)
  {
    std::cerr << e.what() << std::endl;
  }
}

