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
#include <cxxtools/log.h>
#include <cxxtools/arg.h>
#include <cxxtools/remoteprocedure.h>
#include <cxxtools/xmlrpc/httpclient.h>
#include <cxxtools/bin/rpcclient.h>
#include <cxxtools/thread.h>
#include <cxxtools/mutex.h>
#include <cxxtools/clock.h>
#include <cxxtools/timespan.h>
#include <cxxtools/atomicity.h>

class BenchClient
{
    void exec();

    cxxtools::RemoteClient* client;
    cxxtools::AttachedThread thread;

    static unsigned _numRequests;
    static cxxtools::atomic_t _requestsStarted;
    static cxxtools::atomic_t _requestsFinished;
    static cxxtools::atomic_t _requestsFailed;

  public:

    explicit BenchClient(unsigned short port, cxxtools::RemoteClient* client_)
      : client(client_),
        thread(cxxtools::callable(*this, &BenchClient::exec))
    { }

    ~BenchClient()
    { delete client; }

    static unsigned numRequests()
    { return _numRequests; }

    static void numRequests(unsigned n)
    { _numRequests = n; }

    static unsigned requestsStarted()
    { return static_cast<unsigned>(cxxtools::atomicGet(_requestsStarted)); }

    static unsigned requestsFinished()
    { return static_cast<unsigned>(cxxtools::atomicGet(_requestsFinished)); }

    static unsigned requestsFailed()
    { return static_cast<unsigned>(cxxtools::atomicGet(_requestsFailed)); }

    void start()
    { thread.start(); }

    void join()
    { thread.join(); }
};

cxxtools::atomic_t BenchClient::_requestsStarted(0);
cxxtools::atomic_t BenchClient::_requestsFinished(0);
cxxtools::atomic_t BenchClient::_requestsFailed(0);
unsigned BenchClient::_numRequests = 0;
typedef std::vector<BenchClient*> BenchClients;

static cxxtools::Mutex mutex;

void BenchClient::exec()
{
  cxxtools::RemoteProcedure<std::string, std::string> echo(*client, "echo");

  while (static_cast<unsigned>(cxxtools::atomicIncrement(_requestsStarted)) <= _numRequests)
  {
    try
    {
      std::string ret = echo("hi");
      cxxtools::atomicIncrement(_requestsFinished);
      if (ret != "hi")
      {
        std::cerr << "wrong response result \"" << ret << '"' << std::endl;
        cxxtools::atomicIncrement(_requestsFailed);
      }
    }
    catch (const std::exception& e)
    {
      {
        cxxtools::MutexLock lock(mutex);
        std::cerr << "request failed with error message \"" << e.what() << '"' << std::endl;
      }

      cxxtools::atomicIncrement(_requestsFailed);
    }
  }
}

int main(int argc, char* argv[])
{
  try
  {
    log_init("rpcbenchclient.properties");

    cxxtools::Arg<std::string> ip(argc, argv, 'i');
    cxxtools::Arg<unsigned short> port(argc, argv, 'p', 7002);
    cxxtools::Arg<unsigned> threads(argc, argv, 't', 4);
    cxxtools::Arg<bool> binary(argc, argv, 'b');
    BenchClient::numRequests(cxxtools::Arg<unsigned>(argc, argv, 'n', 10000));

    std::cout << "execute " << BenchClient::numRequests() << " requests with " << threads.getValue() << " threads\n\n"
                 "options:\n"
                 "   -l ip      set ip address of server (default: localhost)\n"
                 "   -p number  set port number of server (default: 7002)\n"
                 "   -t number  set number of threads (default: 4)\n"
                 "   -n number  set number of requests (default: 10000)\n"
                 "   -b         use binary rpc protocol instead of xmlrpc\n"
              << std::endl;

    BenchClients clients;

    while (clients.size() < threads)
    {
      cxxtools::RemoteClient* client;
      if (binary)
        client = new cxxtools::bin::RpcClient("", port);
      else
        client = new cxxtools::xmlrpc::HttpClient("", port, "/myservice");
      clients.push_back(new BenchClient(port, client));
    }

    cxxtools::Clock cl;
    cl.start();

    for (BenchClients::iterator it = clients.begin(); it != clients.end(); ++it)
      (*it)->start();

    for (BenchClients::iterator it = clients.begin(); it != clients.end(); ++it)
      (*it)->join();

    cxxtools::Timespan t = cl.stop();

    std::cout << BenchClient::numRequests() << " requests in " << t.totalMSecs()/1e3 << " s => " << (BenchClient::requestsStarted() / (t.totalMSecs()/1e3)) << "#/s\n"
              << BenchClient::requestsFinished() << " finished " << BenchClient::requestsFailed() << " failed" << std::endl;

    for (BenchClients::iterator it = clients.begin(); it != clients.end(); ++it)
      delete *it;
  }
  catch (const std::exception& e)
  {
    std::cerr << e.what() << std::endl;
  }
}

