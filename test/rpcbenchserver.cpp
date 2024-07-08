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
#include <vector>
#include <cxxtools/log.h>
#include <cxxtools/arg.h>
#include <cxxtools/eventloop.h>
#include <cxxtools/http/server.h>
#include <cxxtools/xmlrpc/service.h>
#include <cxxtools/bin/rpcserver.h>
#include <cxxtools/json/rpcserver.h>
#include <cxxtools/json/httpservice.h>
#include <cxxtools/sslctx.h>

#include "color.h"


std::string echo(const std::string& msg)
{
  return msg;
}

std::vector<int> seq(int from, int to)
{
    std::vector<int> ret;
    for (int n = from; n <= to; ++n)
        ret.push_back(n);
    return ret;
}

std::vector<Color> objects(unsigned count)
{
    std::vector<Color> ret(count);
    for (unsigned n = 0; n < count; ++n)
    {
        ret[n].red = n;
        ret[n].green = n + 1;
        ret[n].blue = n + 2;
    }

    return ret;
}

int main(int argc, char* argv[])
{
  try
  {
    log_init(argc, argv);

    cxxtools::Arg<std::string> ip(argc, argv, 'i');
    cxxtools::Arg<unsigned short> port(argc, argv, 'p', 7002);
    cxxtools::Arg<unsigned short> bport(argc, argv, 'b', 7003);
    cxxtools::Arg<unsigned short> jport(argc, argv, 'j', 7004);
    cxxtools::Arg<std::string> sslCert(argc, argv, 'c');
    cxxtools::Arg<unsigned> threads(argc, argv, 't', 4);
    cxxtools::Arg<unsigned> maxThreads(argc, argv, 'T', 200);

    std::cout << "rpc echo server running on port " << port.getValue() << "\n\n"
                 "options:\n\n"
                 "   -i ip      set interface address to listen on (default: all interfaces)\n"
                 "   -p number  set port number for http (xmlrpc and json over http, default: 7002)\n"
                 "   -b number  set port number run binary rpc server (default: 7003)\n"
                 "   -j number  set port number run json rpc server (default: 7004)\n"
                 "   -c cert    enable ssl using the specified server certificate\n"
                 "   -t number  set minimum number of threads (default: 4)\n"
                 "   -T number  set maximum number of threads (default: 200)\n"
              << std::endl;

    cxxtools::EventLoop loop;

    cxxtools::SslCtx sslCtx;
    if (sslCert.isSet())
        sslCtx.loadCertificateFile(sslCert);

    cxxtools::http::Server server(loop, ip, port, sslCtx);
    server.minThreads(threads);
    server.maxThreads(maxThreads);
    cxxtools::xmlrpc::Service service;
    service.registerFunction("echo", echo);
    service.registerFunction("seq", seq);
    service.registerFunction("objects", objects);
    server.addService("/xmlrpc", service);

    cxxtools::bin::RpcServer binServer(loop, ip, bport, sslCtx);
    binServer.minThreads(threads);
    binServer.maxThreads(maxThreads);
    binServer.addService(service);

    cxxtools::json::RpcServer jsonServer(loop, ip, jport, sslCtx);
    jsonServer.minThreads(threads);
    jsonServer.maxThreads(maxThreads);
    jsonServer.addService("", service);

    cxxtools::json::HttpService jsonhttpService;
    jsonhttpService.registerFunction("echo", echo);
    jsonhttpService.registerFunction("seq", seq);
    jsonhttpService.registerFunction("objects", objects);
    server.addService("/jsonrpc", jsonhttpService);

    loop.run();
  }
  catch (const std::exception& e)
  {
    std::cerr << e.what() << std::endl;
  }
}

