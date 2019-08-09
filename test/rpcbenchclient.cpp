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
#include <cxxtools/json/rpcclient.h>
#include <cxxtools/json/httpclient.h>
#include <cxxtools/thread.h>
#include <cxxtools/mutex.h>
#include <cxxtools/clock.h>
#include <cxxtools/timespan.h>
#include <cxxtools/datetime.h>
#include <atomic>

#include "color.h"


class BenchClient
{
    void exec();

    cxxtools::RemoteClient* client;
    cxxtools::AttachedThread thread;

    static unsigned _numRequests;
    static cxxtools::DateTime _until;
    static unsigned _vectorSize;
    static unsigned _objectsSize;
    static std::atomic<unsigned> _requestsStarted;
    static std::atomic<unsigned> _requestsFinished;
    static std::atomic<unsigned> _requestsFailed;

  public:
    explicit BenchClient(cxxtools::RemoteClient* client_)
      : client(client_),
        thread(cxxtools::callable(*this, &BenchClient::exec))
    { }

    ~BenchClient()
    { delete client; }

    static unsigned numRequests()
    { return _numRequests; }

    static void numRequests(unsigned n)
    { _numRequests = n; }

    static cxxtools::DateTime until()
    { return _until; }

    static void until(cxxtools::DateTime n)
    { _until = n; }

    static unsigned vectorSize()
    { return _vectorSize; }

    static void vectorSize(unsigned n)
    { _vectorSize = n; }

    static unsigned objectsSize()
    { return _objectsSize; }

    static void objectsSize(unsigned n)
    { _objectsSize = n; }

    static unsigned requestsStarted()
    { return _requestsStarted; }

    static unsigned requestsFinished()
    { return _requestsFinished; }

    static unsigned requestsFailed()
    { return _requestsFailed; }

    void start()
    { thread.start(); }

    void join()
    { thread.join(); }
};

std::atomic<unsigned> BenchClient::_requestsStarted(0);
std::atomic<unsigned> BenchClient::_requestsFinished(0);
std::atomic<unsigned> BenchClient::_requestsFailed(0);
unsigned BenchClient::_numRequests = 0;
cxxtools::DateTime BenchClient::_until(2999, 12, 31, 23, 59, 59, 999);
unsigned BenchClient::_vectorSize = 0;
unsigned BenchClient::_objectsSize = 0;
typedef std::vector<BenchClient*> BenchClients;

static cxxtools::Mutex mutex;

void BenchClient::exec()
{
  cxxtools::RemoteProcedure<std::string, std::string> echo(*client, "echo");
  cxxtools::RemoteProcedure<std::vector<int>, int, int> seq(*client, "seq");
  cxxtools::RemoteProcedure<std::vector<Color>, unsigned> objects(*client, "objects");

  while (_requestsStarted <= _numRequests
      && cxxtools::DateTime::gmtime() < _until)
  {
    try
    {
      if (_vectorSize > 0)
      {
        std::vector<int> ret = seq(1, _vectorSize);
        ++_requestsFinished;
        if (ret.size() != _vectorSize)
        {
          std::cerr << "wrong response result size " << ret.size() << std::endl;
          ++_requestsFailed;
        }
      }
      else if (_objectsSize > 0)
      {
        std::vector<Color> ret = objects(_objectsSize);
        ++_requestsFinished;
        if (ret.size() != _objectsSize)
        {
          std::cerr << "wrong response result size " << ret.size() << std::endl;
          ++_requestsFailed;
        }
      }
      else
      {
        std::string ret = echo("hi");
        ++_requestsFinished;
        if (ret != "hi")
        {
          std::cerr << "wrong response result \"" << ret << '"' << std::endl;
          ++_requestsFailed;
        }
      }
    }
    catch (const std::exception& e)
    {
      {
        cxxtools::MutexLock lock(mutex);
        std::cerr << "request failed with error message \"" << e.what() << '"' << std::endl;
      }

      ++_requestsFailed;
    }
  }
}

int main(int argc, char* argv[])
{
  try
  {
    log_init("rpcbenchclient.properties");

    cxxtools::Arg<std::string> ip(argc, argv, 'i');
    cxxtools::Arg<unsigned> threads(argc, argv, 't', 4);
    cxxtools::Arg<bool> xmlrpc(argc, argv, 'x');
    cxxtools::Arg<bool> binary(argc, argv, 'b');
    cxxtools::Arg<bool> json(argc, argv, 'j');
    cxxtools::Arg<bool> jsonhttp(argc, argv, 'J');
    cxxtools::Arg<unsigned short> port(argc, argv, 'p', binary ? 7003 : json ? 7004 : 7002);
    cxxtools::Arg<bool> ssl(argc, argv, 's');
    cxxtools::Arg<cxxtools::Seconds> maxtime(argc, argv, 'T');

    BenchClient::numRequests(cxxtools::Arg<unsigned>(argc, argv, 'n', 10000));
    BenchClient::vectorSize(cxxtools::Arg<unsigned>(argc, argv, 'v', 0));
    BenchClient::objectsSize(cxxtools::Arg<unsigned>(argc, argv, 'o', 0));

    if (maxtime.isSet())
        BenchClient::until(cxxtools::DateTime::gmtime() + maxtime);

    if (!xmlrpc && !binary && !json && !jsonhttp)
    {
        std::cerr << "usage: " << argv[0] << " [options]\n"
                     "options:\n"
                     "   -l ip      set ip address of server (default: localhost)\n"
                     "   -p number  set port number of server (default: 7002 for http, 7003 for binary and 7004 for json)\n"
                     "   -x         use xmlrpc protocol\n"
                     "   -b         use binary rpc protocol\n"
                     "   -j         use json rpc protocol\n"
                     "   -J         use json rpc over http protocol\n"
                     "   -s         enable ssl\n"
                     "   -t number  set number of threads (default: 4)\n"
                     "   -n number  set number of requests (default: 10000)\n"
                     "   -T seconds set maximum runtime after which the test stops\n"
                     "   -v number  test int vector with <number> of elements\n"
                     "   -o number  test vector of objects\n"
                     "one protocol must be selected\n"
                  << std::endl;
        return -1;
    }

    BenchClients clients;

    while (clients.size() < threads)
    {
      cxxtools::RemoteClient* client;
      if (binary)
      {
        cxxtools::bin::RpcClient* c = new cxxtools::bin::RpcClient(ip, port, ssl);
        client = c;
      }
      else if (json)
      {
        cxxtools::json::RpcClient* c = new cxxtools::json::RpcClient(ip, port, ssl);
        client = c;
      }
      else if (jsonhttp)
      {
        cxxtools::json::HttpClient* c = new cxxtools::json::HttpClient(ip, port, "/jsonrpc", ssl);
        client = c;
      }
      else // if (xmlrpc)
      {
        cxxtools::xmlrpc::HttpClient* c = new cxxtools::xmlrpc::HttpClient(ip, port, "/xmlrpc", ssl);
        client = c;
      }

      clients.push_back(new BenchClient(client));
    }

    cxxtools::Clock cl;
    cl.start();

    for (BenchClients::iterator it = clients.begin(); it != clients.end(); ++it)
      (*it)->start();

    for (BenchClients::iterator it = clients.begin(); it != clients.end(); ++it)
      (*it)->join();

    cxxtools::Timespan t = cl.stop();

    std::cout << BenchClient::requestsStarted() << " requests in " << t.totalMSecs()/1e3 << " s => " << (BenchClient::requestsStarted() / (t.totalMSecs()/1e3)) << "#/s\n"
              << BenchClient::requestsFinished() << " finished " << BenchClient::requestsFailed() << " failed" << std::endl;

    for (BenchClients::iterator it = clients.begin(); it != clients.end(); ++it)
      delete *it;
  }
  catch (const std::exception& e)
  {
    std::cerr << e.what() << std::endl;
  }
}

