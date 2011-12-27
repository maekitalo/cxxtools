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

#include <iostream>
#include <cxxtools/arg.h>
#include <cxxtools/log.h>
#include <cxxtools/remoteprocedure.h>
#include <cxxtools/xmlrpc/httpclient.h>
#include <cxxtools/bin/rpcclient.h>
#include <cxxtools/json/rpcclient.h>

////////////////////////////////////////////////////////////////////////
// main
//
int main(int argc, char* argv[])
{
  try
  {
    log_init();

    cxxtools::Arg<std::string> ip(argc, argv, 'i');
    cxxtools::Arg<bool> binary(argc, argv, 'b');
    cxxtools::Arg<bool> json(argc, argv, 'j');
    cxxtools::Arg<unsigned short> port(argc, argv, 'p', binary ? 7003 : json ? 7004 : 7002);

    // define a xlmrpc client
    cxxtools::xmlrpc::HttpClient xmlrpcClient(ip, port, "/myservice");
    // and a binary rpc client
    cxxtools::bin::RpcClient binaryClient(ip, port);
    // and a json rpc client
    cxxtools::json::RpcClient jsonClient(ip, port);

    // define remote procedure with std::string return value and a std::string parameter,
    // which uses one of the clients
    cxxtools::RemoteProcedure<std::string, std::string> echo(
        binary ? static_cast<cxxtools::RemoteClient&>(binaryClient) :
        json   ? static_cast<cxxtools::RemoteClient&>(jsonClient) :
                 static_cast<cxxtools::RemoteClient&>(xmlrpcClient), "echo");

    for (int a = 1; a < argc; ++a)
    {
      std::string v = echo(argv[a]);
      std::cout << v << '\n';
    }
  }
  catch (const std::exception& e)
  {
    std::cerr << e.what() << std::endl;
  }
}

