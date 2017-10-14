/*
 * Copyright (C) 2009 Tommi Maekitalo
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

#ifndef CXXTOOLS_HTTP_SERVERIMPL_H
#define CXXTOOLS_HTTP_SERVERIMPL_H

#include "serverimplbase.h"
#include <set>
#include <vector>
#include <cxxtools/queue.h>
#include <cxxtools/event.h>
#include <cxxtools/http/server.h>

namespace cxxtools
{

class EventLoopBase;

namespace net
{
    class TcpServer;
}

namespace http
{

class Worker;
class ServerImpl;
class Socket;
class IdleSocketEvent;
class KeepAliveTimeoutEvent;
class ServerStartEvent;
class NoWaitingThreadsEvent;
class ThreadTerminatedEvent;
class ActiveSocketEvent;

class ServerImpl : public ServerImplBase, public Connectable
{
    public:
        ServerImpl(EventLoopBase& eventLoop, Signal<Server::Runmode>& runmodeChanged);
        ~ServerImpl();

        // override from ServerImplBase
        void listen(const std::string& ip, unsigned short int port, const std::string& certificateFile, const std::string& privateKeyFile);

        bool isTerminating() const
        { return runmode() == Server::Terminating; }

        // override from ServerImplBase
        void terminate();

    private:
        void noWaitingThreads();
        void onInput(Socket& _socket);
        void onTimeout(Socket& _socket);

        void addIdleSocket(Socket* socket);
        void onIdleSocket(const IdleSocketEvent& event);
        void onActiveSocket(const ActiveSocketEvent& event);
        void onKeepAliveTimeout(const KeepAliveTimeoutEvent& event);
        void onNoWaitingThreads(const NoWaitingThreadsEvent& event);
        void onThreadTerminated(const ThreadTerminatedEvent& event);
        void onServerStart(const ServerStartEvent& event);
        void start();

        friend class Worker;

        ////////////////////////////////////////////////////

        MethodSlot<void, ServerImpl, Socket&> inputSlot;
        MethodSlot<void, ServerImpl, Socket&> timeoutSlot;

        Queue<Socket*> _queue;
        std::set<Socket*> _idleSockets;

        ////////////////////////////////////////////////////
        typedef std::vector<net::TcpServer*> ListenerType;
        ListenerType _listener;

        ////////////////////////////////////////////////////
        typedef std::set<Worker*> Threads;
        Threads _threads;
        Threads _terminatedThreads;
        Mutex _threadMutex;
        Condition _threadTerminated;
        void threadTerminated(Worker* worker);
};

}
}

#endif // CXXTOOLS_HTTP_SERVERIMPL_H

