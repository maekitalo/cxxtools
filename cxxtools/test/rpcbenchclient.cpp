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
#include <cxxtools/xmlrpc/remoteprocedure.h>
#include <cxxtools/xmlrpc/httpclient.h>
#include <cxxtools/clock.h>
#include <cxxtools/timespan.h>

class BenchClient
{
    void exec();

    cxxtools::xmlrpc::HttpClient client;
    cxxtools::xmlrpc::RemoteProcedure<std::string, std::string> echo;
    cxxtools::AttachedThread thread;

    static cxxtools::atomic_t requestsStarted;

  public:
    static unsigned maxRequests;

    explicit BenchClient(unsigned short port)
      : client("", port, "/myservice"),
        echo(client, "echo"),
        thread(cxxtools::callable(*this, &BenchClient::exec))
    { }

    void start()
    { thread.start(); }

    void join()
    { thread.join(); }
};

cxxtools::atomic_t BenchClient::requestsStarted = 0;
unsigned BenchClient::maxRequests = 0;
typedef std::vector<BenchClient*> BenchClients;

void BenchClient::exec()
{
  while (static_cast<unsigned>(cxxtools::atomicIncrement(requestsStarted)) < maxRequests)
  {
    echo("hi");
  }
}

int main(int argc, char* argv[])
{
  try
  {
    log_init("rpcbenchclient.properties");

    cxxtools::Arg<unsigned short> port(argc, argv, 'p', 7002);
    cxxtools::Arg<unsigned> threads(argc, argv, 't', 4);
    BenchClient::maxRequests = cxxtools::Arg<unsigned>(argc, argv, 'n', 1000);

    BenchClients clients;

    while (clients.size() < threads)
      clients.push_back(new BenchClient(port));

    cxxtools::Clock cl;
    cl.start();

    for (BenchClients::iterator it = clients.begin(); it != clients.end(); ++it)
      (*it)->start();

    for (BenchClients::iterator it = clients.begin(); it != clients.end(); ++it)
      (*it)->join();

    cxxtools::Timespan t = cl.stop();

    std::cout << BenchClient::maxRequests << " requests in " << t.totalMSecs()/1e3 << " s => " << (BenchClient::maxRequests / (t.totalMSecs()/1e3)) << "#/s" << std::endl;

    for (BenchClients::iterator it = clients.begin(); it != clients.end(); ++it)
      delete *it;
  }
  catch (const std::exception& e)
  {
    std::cerr << e.what() << std::endl;
  }
}

