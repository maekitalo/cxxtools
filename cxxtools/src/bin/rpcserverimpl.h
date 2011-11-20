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

#ifndef CXXTOOLS_BIN_RPCSERVERIMPL_H
#define CXXTOOLS_BIN_RPCSERVERIMPL_H

#include <map>
#include <set>
#include <vector>
#include <cxxtools/noncopyable.h>
#include <cxxtools/event.h>
#include <cxxtools/mutex.h>
#include <cxxtools/condition.h>
#include <cxxtools/queue.h>
#include <cxxtools/signal.h>
#include <cxxtools/connectable.h>
#include <cxxtools/bin/rpcserver.h>

namespace cxxtools
{
    class EventLoopBase;
    class ServiceProcedure;

    namespace bin
    {
        class RpcServerImpl;
        class Worker;
        class Socket;
        class Listener;
        class IdleSocketEvent;
        class ServerStartEvent;
        class NoWaitingThreadsEvent;
        class ThreadTerminatedEvent;
        class ActiveSocketEvent;

        class RpcServerImpl : private NonCopyable, public Connectable
        {
            public:
                RpcServerImpl(EventLoopBase& eventLoop, Signal<RpcServer::Runmode>& runmodeChanged);

                ~RpcServerImpl();

                void listen(const std::string& ip, unsigned short int port, int backlog);

                ServiceProcedure* getProcedure(const std::string& name);

                void releaseProcedure(ServiceProcedure* proc);

                void registerProcedure(const std::string& name, ServiceProcedure* proc);

                unsigned minThreads() const
                { return _minThreads; }

                void minThreads(unsigned m)
                { _minThreads = m; }

                unsigned maxThreads() const
                { return _maxThreads; }

                void maxThreads(unsigned m)
                { _maxThreads = m; }

                std::size_t idleTimeout() const
                { return _idleTimeout; }

                void idleTimeout(std::size_t ms)
                { _idleTimeout = ms; }

                void terminate();

                RpcServer::Runmode runmode() const
                { return _runmode; }

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

                ////////////////////////////////////////////////////

                MethodSlot<void, RpcServerImpl, Socket&> inputSlot;

                typedef std::map<std::string, ServiceProcedure*> ProcedureMap;
                ProcedureMap _procedures;
                unsigned _minThreads;
                unsigned _maxThreads;
                std::size_t _idleTimeout;

                std::vector<Listener*> _listener;
                Queue<Socket*> _queue;

                typedef std::set<Socket*> IdleSocket;
                IdleSocket _idleSocket;

                Mutex _threadMutex;
                Condition _threadTerminated;
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
