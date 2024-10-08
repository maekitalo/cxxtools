/*
 * Copyright (C) 2014 Tommi Maekitalo
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

#include <cxxtools/log.h>
#include <cxxtools/arg.h>
#include <cxxtools/remoteprocedure.h>
#include <cxxtools/xmlrpc/httpclient.h>
#include <cxxtools/bin/rpcclient.h>
#include <cxxtools/json/rpcclient.h>
#include <cxxtools/json/httpclient.h>
#include <cxxtools/signal.h>
#include <cxxtools/clock.h>
#include <cxxtools/timespan.h>
#include <cxxtools/eventloop.h>

#include <iostream>
#include <vector>
#include <set>
#include <thread>
#include <mutex>
#include <atomic>

#include "color.h"

static std::mutex mutex;

class ClientCreator
{
public:
    virtual ~ClientCreator()
    { }
    virtual cxxtools::RemoteClient* create(cxxtools::SelectorBase& selector) = 0;
};

class XmlRpcClientCreator : public ClientCreator
{
    std::string _ip;
    unsigned short _port;
    std::string _url;

public:
    XmlRpcClientCreator(const std::string& ip, unsigned short port, const std::string& url)
        : _ip(ip),
            _port(port),
            _url(url)
    { }

    virtual cxxtools::RemoteClient* create(cxxtools::SelectorBase& selector)
    {
        return new cxxtools::xmlrpc::HttpClient(selector, _ip, _port, _url);
    }
};

class JsonHttpClientCreator : public ClientCreator
{
    std::string _ip;
    unsigned short _port;
    std::string _url;

public:
    JsonHttpClientCreator(const std::string& ip, unsigned short port, const std::string& url)
        : _ip(ip),
            _port(port),
            _url(url)
    { }

    virtual cxxtools::RemoteClient* create(cxxtools::SelectorBase& selector)
    {
        return new cxxtools::json::HttpClient(selector, _ip, _port, _url);
    }
};

class JsonRpcClientCreator : public ClientCreator
{
    std::string _ip;
    unsigned short _port;

public:
    JsonRpcClientCreator(const std::string& ip, unsigned short port)
        : _ip(ip),
            _port(port)
    { }

    virtual cxxtools::RemoteClient* create(cxxtools::SelectorBase& selector)
    {
        return new cxxtools::json::RpcClient(selector, _ip, _port);
    }
};

class BinRpcClientCreator : public ClientCreator
{
    std::string _ip;
    unsigned short _port;

public:
    BinRpcClientCreator(const std::string& ip, unsigned short port)
        : _ip(ip),
            _port(port)
    { }

    virtual cxxtools::RemoteClient* create(cxxtools::SelectorBase& selector)
    {
        return new cxxtools::bin::RpcClient(selector, _ip, _port);
    }
};

class RemoteExecutor : public cxxtools::Connectable
{
public:
    virtual ~RemoteExecutor()
    { }
    virtual void begin() = 0;

    cxxtools::Signal<RemoteExecutor*> finished;
    cxxtools::Signal<RemoteExecutor*> failed;
};

class EchoExecutor : public RemoteExecutor
{
    cxxtools::RemoteProcedure<std::string, std::string> _echo;
    std::string _msg;

public:
    EchoExecutor(cxxtools::RemoteClient* client, const std::string& msg)
        : _echo(*client, "echo"),
            _msg(msg)
    {
        cxxtools::connect(_echo.finished, *this, &EchoExecutor::onEchoFinished);
    }

    void begin()
    {
        _echo.begin(_msg);
    }

    void onEchoFinished(cxxtools::RemoteResult<std::string>& result)
    {
        std::string ret = result.get();
        if (ret != _msg)
        {
            {
                std::lock_guard<std::mutex> lock(mutex);
                std::cerr << "wrong response result \"" << ret << '"' << std::endl;
            }

            failed(this);
        }

        finished(this);
    }
};

class SeqExecutor : public RemoteExecutor
{
    cxxtools::RemoteProcedure<std::vector<int>, int, int> _seq;
    int _startValue;
    int _endValue;

public:
    SeqExecutor(cxxtools::RemoteClient* client, int startValue, int endValue)
        : _seq(*client, "seq"),
            _startValue(startValue),
            _endValue(endValue)
    {
        cxxtools::connect(_seq.finished, *this, &SeqExecutor::onSeqFinished);
    }

    void begin()
    {
        _seq.begin(_startValue, _endValue);
    }

    void onSeqFinished(cxxtools::RemoteResult<std::vector<int> >& result)
    {
        std::vector<int> ret = result.get();
        if (ret.size() != static_cast<std::vector<int>::size_type>(_endValue - _startValue + 1))
        {
            {
                std::lock_guard<std::mutex> lock(mutex);
                std::cerr << "wrong response result size " << ret.size() << std::endl;
            }

            failed(this);
        }

        finished(this);
    }
};

class ObjectsExecutor : public RemoteExecutor
{
    cxxtools::RemoteProcedure<std::vector<Color>, unsigned> _objects;
    unsigned _objectsCount;

public:
    ObjectsExecutor(cxxtools::RemoteClient* client, unsigned objectsCount)
        : _objects(*client, "objects"),
            _objectsCount(objectsCount)
    {
        cxxtools::connect(_objects.finished, *this, &ObjectsExecutor::onObjectsFinished);
    }

    void begin()
    {
        _objects.begin(_objectsCount);
    }

    void onObjectsFinished(cxxtools::RemoteResult<std::vector<Color> >& result)
    {
        std::vector<Color> ret = result.get();
        if (ret.size() != _objectsCount)
        {
            {
                std::lock_guard<std::mutex> lock(mutex);
                std::cerr << "wrong response result size " << ret.size() << std::endl;
            }

            failed(this);
        }

        finished(this);
    }
};

class BenchClient : public cxxtools::Connectable
{
    std::set<cxxtools::RemoteClient*> _clients;
    std::set<RemoteExecutor*> executors;
    cxxtools::EventLoop _loop;
    std::thread _thread;

    static unsigned _numRequests;
    static unsigned _vectorSize;
    static unsigned _objectsSize;
    static std::atomic<unsigned> _requestsStarted;
    static std::atomic<unsigned> _requestsFinished;
    static std::atomic<unsigned> _requestsFailed;

    void onFinished(RemoteExecutor* e)
    {
        ++_requestsFinished;
        if (++_requestsStarted <= _numRequests)
            e->begin();
        else
        {
            executors.erase(e);
            delete e;
            if (executors.empty())
                _loop.exit();
        }
    }

    void onFailed(RemoteExecutor* /*e*/)
    {
        ++_requestsFailed;
    }

public:
    explicit BenchClient(ClientCreator& clientCreator, unsigned numClients)
        : _thread(&cxxtools::EventLoop::run, &_loop)
    {
        while (_clients.size() < numClients)
        {
            cxxtools::RemoteClient* client = clientCreator.create(_loop);
            _clients.insert(client);

            RemoteExecutor* executor;

            if (_vectorSize > 0)
                executor = new SeqExecutor(client, 1, _vectorSize);
            else if (_objectsSize > 0)
                executor = new ObjectsExecutor(client, _objectsSize);
            else
                executor = new EchoExecutor(client, "Hi");

            cxxtools::connect(executor->finished, *this, &BenchClient::onFinished);
            cxxtools::connect(executor->failed, *this, &BenchClient::onFailed);

            ++_requestsStarted;
            executor->begin();

            executors.insert(executor);
        }
    }

    ~BenchClient()
    {
        for (std::set<cxxtools::RemoteClient*>::iterator it = _clients.begin(); it != _clients.end(); ++it)
            delete *it;
        for (std::set<RemoteExecutor*>::iterator it = executors.begin(); it != executors.end(); ++it)
            delete *it;
    }

    static unsigned numRequests()
    { return _numRequests; }

    static void numRequests(unsigned n)
    { _numRequests = n; }

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

    void join()
    { _thread.join(); }
};

std::atomic<unsigned> BenchClient::_requestsStarted(0);
std::atomic<unsigned> BenchClient::_requestsFinished(0);
std::atomic<unsigned> BenchClient::_requestsFailed(0);
unsigned BenchClient::_numRequests = 0;
unsigned BenchClient::_vectorSize = 0;
unsigned BenchClient::_objectsSize = 0;
typedef std::vector<std::unique_ptr<BenchClient>> BenchClients;

int main(int argc, char* argv[])
{
    try
    {
        log_init(argc, argv);

        cxxtools::Arg<std::string> ip(argc, argv, 'i');
        cxxtools::Arg<unsigned> threads(argc, argv, 't', 4);
        cxxtools::Arg<unsigned> concurrentRequestsPerThread(argc, argv, 'c', 4);
        cxxtools::Arg<bool> xmlrpc(argc, argv, 'x');
        cxxtools::Arg<bool> binary(argc, argv, 'b');
        cxxtools::Arg<bool> json(argc, argv, 'j');
        cxxtools::Arg<bool> jsonhttp(argc, argv, 'J');
        cxxtools::Arg<unsigned short> port(argc, argv, 'p', binary ? 7003 : json ? 7004 : 7002);
        BenchClient::numRequests(cxxtools::Arg<unsigned>(argc, argv, 'n', 10000));
        BenchClient::vectorSize(cxxtools::Arg<unsigned>(argc, argv, 'v', 0));
        BenchClient::objectsSize(cxxtools::Arg<unsigned>(argc, argv, 'o', 0));

        if (!xmlrpc && !binary && !json && !jsonhttp)
        {
                std::cerr << "usage: " << argv[0] << " [options]\n"
                             "options:\n"
                             "     -l ip            set ip address of server (default: localhost)\n"
                             "     -p number    set port number of server (default: 7002 for http, 7003 for binary and 7004 for json)\n"
                             "     -x                 use xmlrpc protocol\n"
                             "     -b                 use binary rpc protocol\n"
                             "     -j                 use json rpc protocol\n"
                             "     -J                 use json rpc over http protocol\n"
                             "     -c number    concurrent request per thread (default: 4)\n"
                             "     -t number    set number of threads (default: 4)\n"
                             "     -n number    set number of requests (default: 10000)\n"
                             "one protocol must be selected\n"
                        << std::endl;
                return -1;
        }

        BenchClients clients;

        XmlRpcClientCreator xmlRpcClientCreator(ip, port, "/xmlrpc");
        JsonHttpClientCreator jsonHttpClientCreator(ip, port, "/jsonrpc");
        JsonRpcClientCreator jsonRpcClientCreator(ip, port);
        BinRpcClientCreator binRpcClientCreator(ip, port);
        ClientCreator& clientCreator = binary ? static_cast<ClientCreator&>(binRpcClientCreator)
                                     : json     ? static_cast<ClientCreator&>(jsonRpcClientCreator)
                                     : jsonhttp ? static_cast<ClientCreator&>(jsonHttpClientCreator)
                                     :            static_cast<ClientCreator&>(xmlRpcClientCreator);

        cxxtools::Clock cl;
        cl.start();

        while (clients.size() < threads)
        {
            clients.emplace_back(new BenchClient(clientCreator, concurrentRequestsPerThread));
        }

        for (auto& cl: clients)
            cl->join();

        cxxtools::Timespan t = cl.stop();

        std::cout << BenchClient::numRequests() << " requests in " << t.totalMSecs()/1e3 << " s => " << (BenchClient::requestsStarted() / (t.totalMSecs()/1e3)) << "#/s\n"
                            << BenchClient::requestsFinished() << " finished " << BenchClient::requestsFailed() << " failed" << std::endl;
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
}

