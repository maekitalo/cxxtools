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

#include <cxxtools/http/server.h>
#include <cxxtools/http/socket.h>
#include <cxxtools/log.h>

log_define("cxxtools.http.server")

namespace cxxtools {

namespace http {


Server::Server(const std::string& ip, unsigned short int port)
: _ip(ip),
  _port(port),
  _readTimeout(5000),
  _writeTimeout(5000),
  _keepAliveTimeout(30000),
  _minThreads(4),
  _maxThreads(200),
  _waitingThreads(0),
  _runmode(Stopped)
{
    log_trace("initialize Server class");

    _selector.add(*this);
    cxxtools::connect(connectionPending, *this, &Server::onConnect);
}

void Server::createThread()
{
    log_trace("Server::createThread");

    MutexLock clock(_createThreadMutex);
    MutexLock lock(_threadMutex);

    if (_threads.size() < _maxThreads)
    {
        log_debug("create thread object");
        _startingThread = new AttachedThread(callable(*this, &Server::serverThread));

        log_debug("run thread " << _startingThread);
        _startingThread->create();

        log_debug("wait for thread running");
        _threadRunning.wait(lock);

        log_debug("thread " << _startingThread << " is running");
        _startingThread = 0;
    }
}

void Server::terminate()
{
    MutexLock lock(_threadMutex);

    if (_runmode != Running)
        return;

    _runmode = Terminating;
    _selector.wake();
    _terminated.wait(lock);
    _runmode = Stopped;
}

void Server::run()
{
    log_trace("run server");

    if (_runmode != Stopped)
        throw std::runtime_error("http server already running");

    _runmode = Starting;

    {
        MutexLock selectorLock(_selectorMutex);

        listen(_ip, _port);
        _runmode = Running;

        log_debug("start " << _minThreads << " threads");
        for (unsigned n = 0; n < _minThreads; ++n)
        {
            log_debug("start thread " << n);
            createThread();
        }
    }

    MutexLock lock(_threadMutex);

    while (_runmode == Running || !_threads.empty())
    {
        log_info("wait for finished threads");

        _threadTerminated.wait(lock);
        log_debug(_terminatedThreads.size() << " threads finished");

        for (Threads::iterator it = _terminatedThreads.begin(); it != _terminatedThreads.end(); ++it)
        {
            log_info("join thread " << (*it));
            (*it)->join();
            log_debug("delete thread " << (*it));
            delete *it;
        }

        log_info("delete " << _terminatedThreads.size() << " thread objects");
        _terminatedThreads.clear();
    }

    for (Threads::iterator it = _terminatedThreads.begin(); it != _terminatedThreads.end(); ++it)
    {
        log_info("join thread " << (*it));
        (*it)->join();
        log_debug("delete thread " << (*it));
        delete *it;
    }

    _terminated.signal();
}

void Server::addService(const std::string& url, Service& service)
{
    log_debug("add service for url <" << url << '>');
    _service.insert(ServicesType::value_type(url, &service));
}

void Server::removeService(Service& service)
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

Responder* Server::getResponder(const Request& request)
{
    log_debug("get responder for url <" << request.url() << '>');

    for (ServicesType::const_iterator it = _service.lower_bound(request.url());
        it != _service.end() && it->first == request.url(); ++it)
    {
        Responder* resp = it->second->createResponder(request);
        if (resp)
        {
            log_debug("responder created");
            return resp;
        }
    }

    log_debug("use default responder");
    return _defaultService.createResponder(request);
}

void Server::onConnect(TcpServer& server)
{
    log_trace("onConnect");
    new Socket(_selector, *this);
}

void Server::serverThread()
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
            Mutex& _threadMutex;

        public:
            ThreadTerminator(AttachedThread* thread, Threads& threads, Threads& terminatedThreads, Condition& threadTerminated, Mutex& threadMutex)
                : _thread(thread),
                  _threads(threads),
                  _terminatedThreads(terminatedThreads),
                  _threadTerminated(threadTerminated),
                  _threadMutex(threadMutex)
                { }
            ~ThreadTerminator()
            {
                MutexLock threadLock(_threadMutex);
                _threads.erase(_thread);
                _terminatedThreads.insert(_thread);
                _threadTerminated.signal();
            }

            const AttachedThread* thread() const  { return _thread; }
    };

    MutexLock threadLock(_threadMutex);

    ThreadTerminator terminator(_startingThread, _threads, _terminatedThreads, _threadTerminated, _threadMutex);

    _threads.insert(_startingThread);

    log_info("thread running (" << _startingThread << ')');
    _threadRunning.signal();

    threadLock.unlock();

    MutexLock selectorLock(_selectorMutex, false);

    do
    {
        {
            atomicIncrement(_waitingThreads);
            Dec m(_waitingThreads);
            log_debug("wait for selector lock; now " << _waitingThreads << " threads waiting");
            selectorLock.lock();
        }

        log_debug("selectorLock obtained; " << _waitingThreads << " threads left");

        if (_runmode == Terminating)
        {
            log_debug("server terminating");
            break;
        }

        if (atomicGet(_waitingThreads) == 0)
            createThread();

        while (!hasReplyToDo())
        {
            log_debug("wait selector");
            _selector.wait();

            if (_runmode == Terminating)
                return;

            log_debug("check for idle sockets to add to selector");
            MutexLock lock(_idleSocketsMutex);
            while (!_idleSockets.empty())
            {
                log_debug("add idle socket " << _idleSockets.front() << " to selector");
                _idleSockets.front()->addSelector(_selector);
                _idleSockets.pop_front();
            }
        }

        Socket* s = _readySockets.front();
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
                MutexLock lock(_idleSocketsMutex);
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

    log_info("end thread " << terminator.thread());
}

} // namespace http

} // namespace cxxtools
