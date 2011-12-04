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
#include <cxxtools/arg.h>
#include <cxxtools/log.h>
#include <cxxtools/xmlrpc/httpclient.h>
#include <cxxtools/bin/rpcclient.h>
#include <cxxtools/remoteprocedure.h>
#include <cxxtools/eventloop.h>

cxxtools::EventLoop loop;

void onFinished(const cxxtools::RemoteResult<double>& result)
{
  try
  {
    double r = result.get(); // This may throw an exception, which may have
                             // happened on the server method, but also local
                             // errors like "connection refused" are reported
                             // here.

    std::cout << "result=" << r << std::endl;
  }
  catch (const std::exception& e)
  {
    std::cout << "failed with error: " << e.what() << std::endl;
  }

  // exit the main event loop
  loop.exit();
}

int main(int argc, char* argv[])
{
  try
  {
    log_init();

    cxxtools::Arg<std::string> ip(argc, argv, 'i');
    cxxtools::Arg<bool> binary(argc, argv, 'b');
    cxxtools::Arg<unsigned short> port(argc, argv, 'p', binary ? 7003 : 7002);

    // define a xlmrpc client
    cxxtools::xmlrpc::HttpClient xmlrpcClient(loop, ip, port, "/myservice");
    // and a binary rpc client
    cxxtools::bin::RpcClient binaryClient(loop, ip, port);

    // define remote procedure with dobule return value and two double parameter:
    cxxtools::RemoteProcedure<double, double, double> add(
        binary ? static_cast<cxxtools::RemoteClient&>(binaryClient) :
                 static_cast<cxxtools::RemoteClient&>(xmlrpcClient), "add");

    // connect the callback method to our method
    cxxtools::connect(add.finished, onFinished);

    // request the execution of our method
    add.begin(5, 6);

    // Run the loop, which sends the request to the server and receives the answer.
    // After receiving the answer, the callback onFinished is called.
    //
    // Note that you may have multiple rpc clients running on the loop on the same time
    // as well as other async stuff, like a rpc server.
    loop.run();
  }
  catch (const std::exception& e)
  {
    std::cerr << e.what() << std::endl;
  }
}

