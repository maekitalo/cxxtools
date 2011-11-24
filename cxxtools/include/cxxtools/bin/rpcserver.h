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

#ifndef CXXTOOLS_BIN_RPCSERVER_H
#define CXXTOOLS_BIN_RPCSERVER_H

#include <string>
#include <cxxtools/signal.h>
#include <cxxtools/callable.h>
#include <cxxtools/serviceprocedure.h>
#include <cxxtools/noncopyable.h>

namespace cxxtools
{
    class EventLoopBase;

    namespace bin
    {
        class Responder;
        class RpcServerImpl;

        class RpcServer : private NonCopyable
        {
                friend class Responder;

            public:
                RpcServer(EventLoopBase& eventLoop);
                RpcServer(EventLoopBase& eventLoop, const std::string& ip, unsigned short int port, int backlog = 5);
                ~RpcServer();

                void listen(const std::string& ip, unsigned short int port, int backlog = 5);

                unsigned minThreads() const;
                void minThreads(unsigned m);

                unsigned maxThreads() const;
                void maxThreads(unsigned m);

                // idleTimeout is the time in milliseconds of inactivity after
                // which a socket is moved from a worker thread to the main event loop.
                std::size_t idleTimeout() const;
                void idleTimeout(std::size_t ms);

                enum Runmode {
                  Stopped,
                  Starting,
                  Running,
                  Terminating,
                  Failed
                };

                Signal<Runmode> runmodeChanged;

                template <typename R>
                void registerFunction(const std::string& name, R (*fn)())
                {
                    ServiceProcedure* proc = new BasicServiceProcedure<R>(cxxtools::callable(fn));
                    registerProcedure(name, proc);
                }

                template <typename R, typename A1>
                void registerFunction(const std::string& name, R (*fn)(A1))
                {
                    ServiceProcedure* proc = new BasicServiceProcedure<R, A1>(cxxtools::callable(fn));
                    registerProcedure(name, proc);
                }

                template <typename R, typename A1, typename A2>
                void registerFunction(const std::string& name, R (*fn)(A1, A2))
                {
                    ServiceProcedure* proc = new BasicServiceProcedure<R, A1, A2>(cxxtools::callable(fn));
                    registerProcedure(name, proc);
                }

                template <typename R, typename A1, typename A2, typename A3>
                void registerFunction(const std::string& name, R (*fn)(A1, A2, A3))
                {
                    ServiceProcedure* proc = new BasicServiceProcedure<R, A1, A2, A3>(cxxtools::callable(fn));
                    registerProcedure(name, proc);
                }

                template <typename R, typename A1, typename A2, typename A3, typename A4>
                void registerFunction(const std::string& name, R (*fn)(A1, A2, A3, A4))
                {
                    ServiceProcedure* proc = new BasicServiceProcedure<R, A1, A2, A3, A4>(cxxtools::callable(fn));
                    registerProcedure(name, proc);
                }

                template <typename R, typename A1, typename A2, typename A3, typename A4, typename A5>
                void registerFunction(const std::string& name, R (*fn)(A1, A2, A3, A4, A5))
                {
                    ServiceProcedure* proc = new BasicServiceProcedure<R, A1, A2, A3, A4, A5>(cxxtools::callable(fn));
                    registerProcedure(name, proc);
                }

                template <typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
                void registerFunction(const std::string& name, R (*fn)(A1, A2, A3, A4, A5, A6))
                {
                    ServiceProcedure* proc = new BasicServiceProcedure<R, A1, A2, A3, A4, A5, A6>(cxxtools::callable(fn));
                    registerProcedure(name, proc);
                }

                template <typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7>
                void registerFunction(const std::string& name, R (*fn)(A1, A2, A3, A4, A5, A6, A7))
                {
                    ServiceProcedure* proc = new BasicServiceProcedure<R, A1, A2, A3, A4, A5, A6, A7>(cxxtools::callable(fn));
                    registerProcedure(name, proc);
                }

                template <typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8>
                void registerFunction(const std::string& name, R (*fn)(A1, A2, A3, A4, A5, A6, A7, A8))
                {
                    ServiceProcedure* proc = new BasicServiceProcedure<R, A1, A2, A3, A4, A5, A6, A7, A8>(cxxtools::callable(fn));
                    registerProcedure(name, proc);
                }

                template <typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9>
                void registerFunction(const std::string& name, R (*fn)(A1, A2, A3, A4, A5, A6, A7, A8, A9))
                {
                    ServiceProcedure* proc = new BasicServiceProcedure<R, A1, A2, A3, A4, A5, A6, A7, A8, A9>(cxxtools::callable(fn));
                    registerProcedure(name, proc);
                }

                template <typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10>
                void registerFunction(const std::string& name, R (*fn)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10))
                {
                    ServiceProcedure* proc = new BasicServiceProcedure<R, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10>(cxxtools::callable(fn));
                    registerProcedure(name, proc);
                }

                template <typename R>
                void registerCallable(const std::string& name, const Callable<R>& cb)
                {
                    ServiceProcedure* proc = new BasicServiceProcedure<R>(cb);
                    registerProcedure(name, proc);
                }

                template <typename R, typename A1>
                void registerCallable(const std::string& name, const Callable<R, A1>& cb)
                {
                    ServiceProcedure* proc = new BasicServiceProcedure<R, A1>(cb);
                    registerProcedure(name, proc);
                }

                template <typename R, typename A1, typename A2>
                void registerCallable(const std::string& name, const Callable<R, A1, A2>& cb)
                {
                    ServiceProcedure* proc = new BasicServiceProcedure<R, A1, A2>(cb);
                    registerProcedure(name, proc);
                }

                template <typename R, typename A1, typename A2, typename A3>
                void registerCallable(const std::string& name, const Callable<R, A1, A2, A3>& cb)
                {
                    ServiceProcedure* proc = new BasicServiceProcedure<R, A1, A2, A3>(cb);
                    registerProcedure(name, proc);
                }

                template <typename R, typename A1, typename A2, typename A3, typename A4>
                void registerCallable(const std::string& name, const Callable<R, A1, A2, A3, A4>& cb)
                {
                    ServiceProcedure* proc = new BasicServiceProcedure<R, A1, A2, A3, A4>(cb);
                    registerProcedure(name, proc);
                }

                template <typename R, typename A1, typename A2, typename A3, typename A4, typename A5>
                void registerCallable(const std::string& name, const Callable<R, A1, A2, A3, A4, A5>& cb)
                {
                    ServiceProcedure* proc = new BasicServiceProcedure<R, A1, A2, A3, A4, A5>(cb);
                    registerProcedure(name, proc);
                }

                template <typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
                void registerCallable(const std::string& name, const Callable<R, A1, A2, A3, A4, A5, A6>& cb)
                {
                    ServiceProcedure* proc = new BasicServiceProcedure<R, A1, A2, A3, A4, A5, A6>(cb);
                    registerProcedure(name, proc);
                }

                template <typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7>
                void registerCallable(const std::string& name, const Callable<R, A1, A2, A3, A4, A5, A6, A7>& cb)
                {
                    ServiceProcedure* proc = new BasicServiceProcedure<R, A1, A2, A3, A4, A5, A6, A7>(cb);
                    registerProcedure(name, proc);
                }

                template <typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8>
                void registerCallable(const std::string& name, const Callable<R, A1, A2, A3, A4, A5, A6, A7, A8>& cb)
                {
                    ServiceProcedure* proc = new BasicServiceProcedure<R, A1, A2, A3, A4, A5, A6, A7, A8>(cb);
                    registerProcedure(name, proc);
                }

                template <typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9>
                void registerCallable(const std::string& name, const Callable<R, A1, A2, A3, A4, A5, A6, A7, A8, A9>& cb)
                {
                    ServiceProcedure* proc = new BasicServiceProcedure<R, A1, A2, A3, A4, A5, A6, A7, A8, A9>(cb);
                    registerProcedure(name, proc);
                }

                template <typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10>
                void registerCallable(const std::string& name, const Callable<R, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10>& cb)
                {
                    ServiceProcedure* proc = new BasicServiceProcedure<R, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10>(cb);
                    registerProcedure(name, proc);
                }

                template <typename R, class C>
                void registerMethod(const std::string& name, C& obj, R (C::*method)() )
                {
                    ServiceProcedure* proc = new BasicServiceProcedure<R>( callable(obj, method) );
                    registerProcedure(name, proc);
                }

                template <typename R, class C, typename A1>
                void registerMethod(const std::string& name, C& obj, R (C::*method)(A1) )
                {
                    ServiceProcedure* proc = new BasicServiceProcedure<R, A1>( callable(obj, method) );
                    registerProcedure(name, proc);
                }

                template <typename R, class C, typename A1, typename A2>
                void registerMethod(const std::string& name, C& obj, R (C::*method)(A1, A2) )
                {
                    ServiceProcedure* proc = new BasicServiceProcedure<R, A1, A2>( callable(obj, method) );
                    registerProcedure(name, proc);
                }

                template <typename R, class C, typename A1, typename A2, typename A3>
                void registerMethod(const std::string& name, C& obj, R (C::*method)(A1, A2, A3) )
                {
                    ServiceProcedure* proc = new BasicServiceProcedure<R, A1, A2, A3>( callable(obj, method) );
                    registerProcedure(name, proc);
                }

                template <typename R, class C, typename A1, typename A2, typename A3, typename A4>
                void registerMethod(const std::string& name, C& obj, R (C::*method)(A1, A2, A3, A4) )
                {
                    ServiceProcedure* proc = new BasicServiceProcedure<R, A1, A2, A3, A4>( callable(obj, method) );
                    registerProcedure(name, proc);
                }

                template <typename R, class C, typename A1, typename A2, typename A3, typename A4, typename A5>
                void registerMethod(const std::string& name, C& obj, R (C::*method)(A1, A2, A3, A4, A5) )
                {
                    ServiceProcedure* proc = new BasicServiceProcedure<R, A1, A2, A3, A4, A5>( callable(obj, method) );
                    registerProcedure(name, proc);
                }

                template <typename R, class C, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
                void registerMethod(const std::string& name, C& obj, R (C::*method)(A1, A2, A3, A4, A5, A6) )
                {
                    ServiceProcedure* proc = new BasicServiceProcedure<R, A1, A2, A3, A4, A5, A6>( callable(obj, method) );
                    registerProcedure(name, proc);
                }

                template <typename R, class C, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7>
                void registerMethod(const std::string& name, C& obj, R (C::*method)(A1, A2, A3, A4, A5, A6, A7) )
                {
                    ServiceProcedure* proc = new BasicServiceProcedure<R, A1, A2, A3, A4, A5, A6, A7>( callable(obj, method) );
                    registerProcedure(name, proc);
                }

                template <typename R, class C, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8>
                void registerMethod(const std::string& name, C& obj, R (C::*method)(A1, A2, A3, A4, A5, A6, A7, A8) )
                {
                    ServiceProcedure* proc = new BasicServiceProcedure<R, A1, A2, A3, A4, A5, A6, A7, A8>( callable(obj, method) );
                    registerProcedure(name, proc);
                }

                template <typename R, class C, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9>
                void registerMethod(const std::string& name, C& obj, R (C::*method)(A1, A2, A3, A4, A5, A6, A7, A8, A9) )
                {
                    ServiceProcedure* proc = new BasicServiceProcedure<R, A1, A2, A3, A4, A5, A6, A7, A8, A9>( callable(obj, method) );
                    registerProcedure(name, proc);
                }

                template <typename R, class C, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10>
                void registerMethod(const std::string& name, C& obj, R (C::*method)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10) )
                {
                    ServiceProcedure* proc = new BasicServiceProcedure<R, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10>( callable(obj, method) );
                    registerProcedure(name, proc);
                }

            private:
                ServiceProcedure* getProcedure(const std::string& name);

                void releaseProcedure(ServiceProcedure* proc);

                void registerProcedure(const std::string& name, ServiceProcedure* proc);

                RpcServerImpl* _impl;
        };
    }
}

#endif // CXXTOOLS_BIN_RPCSERVER_H
