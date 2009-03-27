/*
 * Copyright (C) 2009 by Marc Boris Duerner, Tommi Maekitalo
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

#include <cxxtools/httpserver.h>
#include <cxxtools/httpsocket.h>
#include <cxxtools/log.h>

log_define("cxxtools.net.http.server")

namespace cxxtools {

namespace net {


HttpServer::HttpServer(const std::string& ip, unsigned short int port)
: TcpServer(ip, port),
  _readTimeout(5000),
  _writeTimeout(5000),
  _keepAliveTimeout(30000),
  _minThreads(4),
  _maxThreads(10),
  _waitingThreads(0),
  _terminating(false)
{
    log_trace("initialize HttpServer class");

    _selector.add(*this);
    cxxtools::connect(connectionPending, *this, &HttpServer::onConnect);
}

void HttpServer::createThread()
{
    log_trace("HttpServer::createThread");

    log_info("create http worker thread");

    MutexLock lock(_threadMutex);

    if (_threads.size() < _maxThreads)
    {
        log_debug("create thread object");
        _startingThread = new AttachedThread(callable(*this, &HttpServer::serverThread));

        _threads.insert(_startingThread);

        log_debug("run thread");
        _startingThread->create();

        log_debug("wait for thread running");
        _threadRunning.wait(lock);

        log_debug("thread is running");
        _startingThread = 0;
    }
}

void HttpServer::terminate()
{
    MutexLock lock(_threadMutex);
    _terminating = true;
    _selector.wake();
    _terminated.wait(lock);
}

void HttpServer::run()
{
    log_trace("run server");

    log_debug("start " << _minThreads << " threads");
    for (unsigned n = 0; n < _minThreads; ++n)
    {
        log_debug("start thread " << n);
        createThread();
    }

    do
    {
        MutexLock lock(_threadMutex);
        log_info("wait for finished threads");
        _threadTerminated.wait(lock);
        log_debug(_terminatedThreads.size() << " threads finished");
        for (Threads::iterator it = _terminatedThreads.begin(); it != _terminatedThreads.end(); ++it)
        {
            log_info("join thread");
            (*it)->join();
            log_info("delete thread");
            delete *it;
        }

        log_info("delete " << _terminatedThreads.size() << " thread objects");
        _terminatedThreads.clear();

    } while (!_terminating && _threads.size() > 0);

    MutexLock lock(_threadMutex);
    _terminated.signal();
}

void HttpServer::addService(const std::string& url, HttpService& service)
{
    log_debug("add service for url <" << url << '>');
    _service.insert(ServicesType::value_type(url, &service));
}

void HttpServer::removeService(HttpService& service)
{
    ServicesType::iterator it = _service.begin();
    while (it != _service.end())
    {
        if (it->second == &service)
        {
            _service.erase(it++);
        }
        else
        {
            ++it;
        }
    }
}

HttpResponder* HttpServer::getResponder(const HttpRequest& request)
{
    log_debug("get responder for url <" << request.url() << '>');

    for (ServicesType::const_iterator it = _service.lower_bound(request.url());
        it != _service.end() && it->first == request.url(); ++it)
    {
        HttpResponder* resp = it->second->createResponder(request);
        if (resp)
        {
            log_debug("responder created");
            return resp;
        }
    }

    log_debug("use default responder");
    return _defaultService.createResponder(request);
}

void HttpServer::onConnect(TcpServer& server)
{
    log_trace("onConnect");
    new HttpSocket(_selector, *this);
}

void HttpServer::serverThread()
{
    log_trace("serverThread");
    class Dec
    {
            atomic_t& _counter;

        public:
            explicit Dec(atomic_t& counter)
                : _counter(counter)
                { }
            ~Dec()
                { atomicDecrement(_counter); }
    };

    class ThreadTerminator
    {
            AttachedThread* _thread;
            Threads& _threads;
            Threads& _terminatedThreads;
            Condition& _threadTerminated;

        public:
            ThreadTerminator(AttachedThread* thread, Threads& threads, Threads& terminatedThreads, Condition& threadTerminated)
                : _thread(thread),
                  _threads(threads),
                  _terminatedThreads(terminatedThreads),
                  _threadTerminated(threadTerminated)
                { }
            ~ThreadTerminator()
            {
                _threads.erase(_thread);
                _terminatedThreads.insert(_thread);
                _threadTerminated.signal();
            }
    };

    ThreadTerminator terminator(_startingThread, _threads, _terminatedThreads, _threadTerminated);

    MutexLock selectorLock(_selectorMutex, false);

    log_debug("signal threadRunning");
    {
        MutexLock lock(_threadMutex);
        _threadRunning.signal();
    }

    do
    {
        {
            atomicIncrement(_waitingThreads);
            Dec m(_waitingThreads);
            log_debug("wait for selector lock; now " << _waitingThreads << " threads waiting");
            selectorLock.lock();
        }

        log_debug("selectorLock obtained; " << _waitingThreads << " threads left");

        if (_terminating)
            break;

        if (atomicGet(_waitingThreads) == 0)
            createThread();

        while (!hasReplyToDo())
        {
            log_debug("wait selector");
            _selector.wait();

            if (_terminating)
                return;

            log_debug("check for idle sockets to add to selector");
            MutexLock lock(_idleSocketsMutex);
            while (!_idleSockets.empty())
            {
                log_debug("idle socket " << _idleSockets.front() << " found");
                _idleSockets.front()->addSelector(_selector);
                _idleSockets.pop_front();
            }
        }

        HttpSocket* s = _readySockets.front();
        log_debug("socket " << s << " ready");
        _readySockets.pop_front();
        s->removeSelector();

        log_debug("release selector lock");
        selectorLock.unlock();

        try
        {
          log_info("execute reply for url " << s->request().url());
          if (s->doReply())
          {
              log_debug("add socket to idle sockets and wake up selector");
              _idleSockets.push_back(s);
              _selector.wake();
          }
          else
              log_debug("socket destroyed");
        }
        catch (const std::exception& e)
        {
          delete s;
          log_error(e.what());
        }

    } while (atomicGet(_waitingThreads) < _minThreads);

    log_info("end thread");
}

} // namespace net

} // namespace cxxtools
