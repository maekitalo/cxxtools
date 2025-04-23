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

#ifndef CXXTOOLS_BIN_RPCSERVERIMPL_H
#define CXXTOOLS_BIN_RPCSERVERIMPL_H

#include <cxxtools/bin/rpcserver.h>
#include <cxxtools/event.h>
#include <cxxtools/queue.h>
#include <cxxtools/signal.h>
#include <cxxtools/delegate.h>
#include <cxxtools/connectable.h>

#include <mutex>
#include <condition_variable>
#include <set>
#include <vector>
#include <memory>

namespace cxxtools
{

class EventLoopBase;
class ServiceProcedure;
class SslCtx;

namespace net
{
    class TcpServer;
}

namespace bin
{
    class RpcServerImpl;
    class Worker;
    class Socket;
    class IdleSocketEvent;
    class ServerStartEvent;
    class NoWaitingThreadsEvent;
    class ThreadTerminatedEvent;
    class ActiveSocketEvent;

    class RpcServerImpl : public Connectable
    {
            RpcServerImpl(const RpcServerImpl&) = delete;
            RpcServerImpl& operator=(const RpcServerImpl&) = delete;

        public:
            RpcServerImpl(EventLoopBase& eventLoop, Signal<RpcServer::Runmode>& runmodeChanged, ServiceRegistry& serviceRegistry);
            ~RpcServerImpl();

            void listen(const std::string& ip, unsigned short int port, const SslCtx& sslCtx);

            unsigned minThreads() const
            { return _minThreads; }

            void minThreads(unsigned m)
            { _minThreads = m; }

            unsigned maxThreads() const
            { return _maxThreads; }

            void maxThreads(unsigned m)
            { _maxThreads = m; }

            void terminate();

            RpcServer::Runmode runmode() const
            { return _runmode; }

            Delegate<bool, const SslCertificate&> acceptSslCertificate;

        private:
            void runmode(RpcServer::Runmode runmode)
            {
                _runmode = runmode;
                _runmodeChanged(runmode);
            }

            RpcServer::Runmode _runmode;
            Signal<RpcServer::Runmode>& _runmodeChanged;

            EventLoopBase& _eventLoop;

            void noWaitingThreads();
            void onInput(Socket& _socket);

            void addIdleSocket(Socket* socket);
            void onIdleSocket(const IdleSocketEvent& event);
            void onActiveSocket(const ActiveSocketEvent& event);
            void onNoWaitingThreads(const NoWaitingThreadsEvent& event);
            void onThreadTerminated(const ThreadTerminatedEvent& event);
            void onServerStart(const ServerStartEvent& event);
            void start();

            friend class Worker;
            friend class Socket;

            ////////////////////////////////////////////////////

            MethodSlot<void, RpcServerImpl, Socket&> inputSlot;

            ServiceRegistry& _serviceRegistry;
            unsigned _minThreads;
            unsigned _maxThreads;

            std::vector<std::unique_ptr<net::TcpServer>> _listener;
            Queue<Socket*> _queue;

            typedef std::set<Socket*> IdleSocket;
            IdleSocket _idleSocket;

            std::mutex _threadMutex;
            std::condition_variable _threadTerminated;
            typedef std::set<Worker*> Threads;
            Threads _threads;
            Threads _terminatedThreads;
            void threadTerminated(Worker* worker);

            bool isTerminating() const
            { return runmode() == RpcServer::Terminating; }
    };
}
}
#endif // CXXTOOLS_BIN_RPCSERVERIMPL_H
