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
#include <cxxtools/selector.h>
#include <cxxtools/log.h>
#include <cassert>

log_define("cxxtools.net.httpserver")

namespace cxxtools {

namespace net {

void HttpResponder::beginRequest(std::istream& in, HttpRequest& request)
{
}

std::size_t HttpResponder::readBody(std::istream& in)
{
    std::streambuf* sb = in.rdbuf();

    std::size_t ret = 0;
    while (sb->in_avail() > 0)
    {
        sb->sbumpc();
        ++ret;
    }

    return ret;
}

void HttpResponder::replyError(std::ostream& out, HttpRequest& request, HttpReply& reply, const std::exception& ex)
{
    reply.httpReturn(500, "internal server error");
    reply.setHeader("Content-Type", "text/plain");
    out << ex.what();
}

void HttpNotFoundResponder::reply(std::ostream& out, HttpRequest& request, HttpReply& reply)
{
    reply.httpReturn(404, "Not found");
}

HttpResponder* HttpNotFoundService::createResponder(const HttpRequest&)
{
    return &_responder;
}

void HttpNotFoundService::releaseResponder(HttpResponder*)
{ }

void HttpSocket::ParseEvent::onMethod(const std::string& method)
{
    _request.method(method);
}

void HttpSocket::ParseEvent::onUrl(const std::string& url)
{
    _request.url(url);
}

void HttpSocket::ParseEvent::onUrlParam(const std::string& q)
{
    _request.qparams(q);
}

HttpSocket::HttpSocket(SelectorBase& selector, HttpServer& server)
    : TcpSocket(server),
      _server(server),
      _parseEvent(_request),
      _parser(_parseEvent, false),
      _responder(0)
{
    _stream.attachDevice(*this);
    _stream.buffer().beginRead();
    cxxtools::connect(_stream.buffer().inputReady, *this, &HttpSocket::onInput);
    cxxtools::connect(_stream.buffer().outputReady, *this, &HttpSocket::onOutput);
    cxxtools::connect(_timer.timeout, *this, &HttpSocket::onTimeout);


    _timer.start(_server.readTimeout());

    addSelector(selector);
}

void HttpSocket::removeSelector()
{
    TcpSocket::setSelector(0);
    _timer.setSelector(0);
}

void HttpSocket::addSelector(SelectorBase& selector)
{
    selector.add(*this);
    selector.add(_timer);
}

void HttpSocket::onInput(StreamBuffer& sb)
{
    log_trace("onInput");

    if (sb.in_avail() == 0 || sb.device()->eof())
    {
        log_debug("end of stream");
        close();
        delete this;
        return;
    }

    _timer.start(_server.readTimeout());
    if ( _responder == 0 )
    {
        _parser.advance(sb);

        if (_parser.fail())
        {
            _responder = _server.getDefaultResponder(_request);
            _responder->replyError(_reply.body(), _request, _reply,
                std::runtime_error("invalid http header"));
            _responder->release();
            _responder = 0;

            sendReply();

            onOutput(sb);
            return;
        }

        if (_parser.end())
        {
            _responder = _server.getResponder(_request);
            try
            {
                _responder->beginRequest(_stream, _request);
            }
            catch (const std::exception& e)
            {
                _reply.setHeader("Connection", "close");
                _responder->replyError(_reply.body(), _request, _reply, e);
                _responder->release();
                _responder = 0;
                sendReply();

                onOutput(sb);
                return;
            }

            _contentLength = _request.header().contentLength();
            if (_contentLength == 0)
            {
                _server.addReadySockets(this);
                return;
            }

        }
        else
        {
            sb.beginRead();
        }
    }

    if (_responder)
    {
        if (sb.in_avail() > 0)
        {
            try
            {
                std::size_t s = _responder->readBody(_stream);
                assert(s > 0);
                _contentLength -= s;
            }
            catch (const std::exception& e)
            {
                _reply.setHeader("Connection", "close");
                _responder->replyError(_reply.body(), _request, _reply, e);
                _responder->release();
                _responder = 0;
                sendReply();

                onOutput(sb);
                return;
            }
        }

        if (_contentLength <= 0)
        {
            _server.addReadySockets(this);
        }
        else
        {
            sb.beginRead();
        }
    }
}

bool HttpSocket::doReply()
{
    _responder->reply(_reply.body(), _request, _reply);
    _responder->release();
    _responder = 0;

    sendReply();

    return onOutput(_stream.buffer());
}

bool HttpSocket::onOutput(StreamBuffer& sb)
{
    log_trace("onOutput");

    sb.beginWrite();

    if ( sb.out_avail() )
    {
        _timer.start(_server.writeTimeout());
    }
    else
    {
        bool keepAlive = _request.header().keepAlive();

        if (keepAlive)
        {
            std::string connection = _reply.getHeader("Connection");

            if (connection == "close"
              || (connection.empty()
                    && (_reply.header().httpVersionMajor() < 1
                     || _reply.header().httpVersionMinor() < 1)))
            {
                keepAlive = false;
            }
        }

        if (keepAlive)
        {
            log_debug("do keep alive");
            _timer.start(_server.keepAliveTimeout());
            _request.clear();
            _reply.clear();
            _parser.reset(false);
            _stream.buffer().beginRead();
        }
        else
        {
            log_debug("don't do keep alive");
            close();
            delete this;
            return false;
        }
    }

    return true;
}

void HttpSocket::onTimeout()
{
    log_debug("timeout");
    close();
    delete this;
}

void HttpSocket::sendReply()
{
    const std::string contentLength = "Content-Length";
    const std::string server = "Server";
    const std::string connection = "Connection";
    const std::string date = "Date";

    _stream << "HTTP/"
        << _reply.header().httpVersionMajor() << '.'
        << _reply.header().httpVersionMinor() << ' '
        << _reply.header().httpReturnCode() << ' '
        << _reply.header().httpReturnText() << "\r\n";

    for (HttpReplyHeader::const_iterator it = _reply.header().begin();
        it != _reply.header().end(); ++it)
    {
        _stream << it->first << ": " << it->second << "\r\n";
    }

    if (!_reply.header().hasHeader(contentLength))
    {
        _stream << "Content-Length: " << _reply.bodySize() << "\r\n";
    }

    if (!_reply.header().hasHeader(server))
    {
        _stream << "Server: cxxtools-Net-HttpServer\r\n";
    }

    if (!_reply.header().hasHeader(connection))
    {
        _stream << "Connection: "
                << (_request.header().keepAlive() ? "keep-alive" : "close")
                << "\r\n";
    }

    if (!_reply.header().hasHeader(date))
    {
        _stream << "Date: " << HttpMessageHeader::htdateCurrent() << "\r\n";
    }

    _stream << "\r\n";

    _reply.sendBody(_stream);

}

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
        log_debug("wait for finished threads");
        _threadTerminated.wait(lock);
        log_debug(_terminatedThreads.size() << " threads finished");
        for (Threads::iterator it = _terminatedThreads.begin(); it != _terminatedThreads.end(); ++it)
        {
            (*it)->join();
            delete *it;
        }

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

        log_debug("execute reply");
        if (s->doReply())
        {
            log_debug("add socket to idle sockets and wake up selector");
            _idleSockets.push_back(s);
            _selector.wake();
        }
        else
            log_debug("socket destroyed");

    } while (atomicGet(_waitingThreads) < _minThreads);

    log_debug("end thread");
}

} // namespace net

} // namespace cxxtools
