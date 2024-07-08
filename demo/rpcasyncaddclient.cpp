/*
 * Copyright (C) 2011,2013 Tommi Maekitalo
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

This demo program shows how to run multiple remote rpc calls asncronously so
that they run in parallel.

 */

#include <iostream>
#include <cxxtools/arg.h>
#include <cxxtools/log.h>
#include <cxxtools/xmlrpc/httpclient.h>
#include <cxxtools/bin/rpcclient.h>
#include <cxxtools/json/rpcclient.h>
#include <cxxtools/json/httpclient.h>
#include <cxxtools/remoteprocedure.h>
#include <cxxtools/selector.h>

int main(int argc, char* argv[])
{
  try
  {
    // initialize logging - this reads the file log.xml from the current directory
    log_init(argc, argv);

    // read the command line options
    cxxtools::Arg<std::string> ip(argc, argv, 'i');  // option -i <ip-addres> defines the address where to find the server
    cxxtools::Arg<bool> binary(argc, argv, 'b');
    cxxtools::Arg<bool> json(argc, argv, 'j');
    cxxtools::Arg<bool> jsonhttp(argc, argv, 'J');
    cxxtools::Arg<cxxtools::Milliseconds> timeout(argc, argv, 't', cxxtools::RemoteClient::WaitInfinite);
    cxxtools::Arg<unsigned short> port(argc, argv, 'p', binary ? 7003 : json ? 7004 : 7002);

    // we need a selector, which controls the network activity
    cxxtools::Selector selector;

    // Normally we would define just one rpc client for the protocol we use but
    // here we want to demonstrate, that it is just up to the client, which protocol
    // is used for the remote call.

    // One client can run just one request at a time. To run parallel requests
    // we need 2 clients. So we define 2 clients for each protocol.

    // define a xlmrpc client
    cxxtools::xmlrpc::HttpClient xmlrpcClient1(selector, ip, port, "/xmlrpc");
    cxxtools::xmlrpc::HttpClient xmlrpcClient2(selector, ip, port, "/xmlrpc");

    // and a binary rpc client
    cxxtools::bin::RpcClient binaryClient1(selector, ip, port);
    cxxtools::bin::RpcClient binaryClient2(selector, ip, port);

    // and a tcp json rpc client
    cxxtools::json::RpcClient jsonClient1(selector, ip, port);
    cxxtools::json::RpcClient jsonClient2(selector, ip, port);

    // and a http json rpc client
    cxxtools::json::HttpClient httpJsonClient1(selector, ip, port, "/jsonrpc");
    cxxtools::json::HttpClient httpJsonClient2(selector, ip, port, "/jsonrpc");

    // now se select the client depending on the command line flags

    cxxtools::RemoteClient& client1(
        binary   ? static_cast<cxxtools::RemoteClient&>(binaryClient1) :
        json     ? static_cast<cxxtools::RemoteClient&>(jsonClient1) :
        jsonhttp ? static_cast<cxxtools::RemoteClient&>(httpJsonClient1) :
                   static_cast<cxxtools::RemoteClient&>(xmlrpcClient1));

    cxxtools::RemoteClient& client2(
        binary   ? static_cast<cxxtools::RemoteClient&>(binaryClient2) :
        json     ? static_cast<cxxtools::RemoteClient&>(jsonClient2) :
        jsonhttp ? static_cast<cxxtools::RemoteClient&>(httpJsonClient2) :
                   static_cast<cxxtools::RemoteClient&>(xmlrpcClient2));

    // define remote procedure with dobule return value and two double parameters
    cxxtools::RemoteProcedure<double, double, double> add1(client1, "add");
    cxxtools::RemoteProcedure<double, double, double> add2(client2, "add");

    // initiate the execution of our method
    add1.begin(5, 6);
    add2.begin(1, 2);

    // Calling RemoteProcedure::end will run the underlying event loop until
    // the remote procedure is finished and return the result.
    // In case of a error, an exception is thrown.

    // Note that waiting for the end of one remote procedure will also start
    // and maybe finish the second remote procedure.

    double result1 = add1.end(timeout);

    std::cout << "result1=" << result1 << std::endl;

    // Here we run the loop again until the second procedure is finished. It
    // may well be, that the procedure is already finished and we get the
    // result immediately.

    double result2 = add2.end(timeout);

    std::cout << "result2=" << result2 << std::endl;

  }
  catch (const std::exception& e)
  {
    std::cerr << "error: " << e.what() << std::endl;
  }
}

