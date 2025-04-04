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

#ifndef CXXTOOLS_SERVICEREGISTRY_H
#define CXXTOOLS_SERVICEREGISTRY_H

#include <cxxtools/serviceprocedure.h>
#include <cxxtools/callable.h>
#include <cxxtools/function.h>
#include <cxxtools/method.h>
#include <string>
#include <vector>
#include <map>

namespace cxxtools
{
    class ServiceRegistry
    {
            ServiceRegistry(const ServiceRegistry&) = delete;
            ServiceRegistry(ServiceRegistry&&) = delete;
            ServiceRegistry& operator=(const ServiceRegistry&)= delete;
            ServiceRegistry& operator=(ServiceRegistry&&)= delete;

        public:
            ServiceRegistry() = default;
            ~ServiceRegistry();

            template <typename R>
            void registerFunction(const std::string& name, R (*fn)())
            {
                ServiceProcedure* proc = new BasicServiceProcedure<R>(callable(fn));
                this->registerProcedure(name, proc);
            }

            template <typename R, typename A1>
            void registerFunction(const std::string& name, R (*fn)(A1))
            {
                ServiceProcedure* proc = new BasicServiceProcedure<R, A1>(callable(fn));
                this->registerProcedure(name, proc);
            }

            template <typename R, typename A1, typename A2>
            void registerFunction(const std::string& name, R (*fn)(A1, A2))
            {
                ServiceProcedure* proc = new BasicServiceProcedure<R, A1, A2>(callable(fn));
                this->registerProcedure(name, proc);
            }

            template <typename R, typename A1, typename A2, typename A3>
            void registerFunction(const std::string& name, R (*fn)(A1, A2, A3))
            {
                ServiceProcedure* proc = new BasicServiceProcedure<R, A1, A2, A3>(callable(fn));
                this->registerProcedure(name, proc);
            }

            template <typename R, typename A1, typename A2, typename A3, typename A4>
            void registerFunction(const std::string& name, R (*fn)(A1, A2, A3, A4))
            {
                ServiceProcedure* proc = new BasicServiceProcedure<R, A1, A2, A3, A4>(callable(fn));
                this->registerProcedure(name, proc);
            }

            template <typename R, typename A1, typename A2, typename A3, typename A4, typename A5>
            void registerFunction(const std::string& name, R (*fn)(A1, A2, A3, A4, A5))
            {
                ServiceProcedure* proc = new BasicServiceProcedure<R, A1, A2, A3, A4, A5>(callable(fn));
                this->registerProcedure(name, proc);
            }

            template <typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
            void registerFunction(const std::string& name, R (*fn)(A1, A2, A3, A4, A5, A6))
            {
                ServiceProcedure* proc = new BasicServiceProcedure<R, A1, A2, A3, A4, A5, A6>(callable(fn));
                this->registerProcedure(name, proc);
            }

            template <typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7>
            void registerFunction(const std::string& name, R (*fn)(A1, A2, A3, A4, A5, A6, A7))
            {
                ServiceProcedure* proc = new BasicServiceProcedure<R, A1, A2, A3, A4, A5, A6, A7>(callable(fn));
                this->registerProcedure(name, proc);
            }

            template <typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8>
            void registerFunction(const std::string& name, R (*fn)(A1, A2, A3, A4, A5, A6, A7, A8))
            {
                ServiceProcedure* proc = new BasicServiceProcedure<R, A1, A2, A3, A4, A5, A6, A7, A8>(callable(fn));
                this->registerProcedure(name, proc);
            }

            template <typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9>
            void registerFunction(const std::string& name, R (*fn)(A1, A2, A3, A4, A5, A6, A7, A8, A9))
            {
                ServiceProcedure* proc = new BasicServiceProcedure<R, A1, A2, A3, A4, A5, A6, A7, A8, A9>(callable(fn));
                this->registerProcedure(name, proc);
            }

            template <typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10>
            void registerFunction(const std::string& name, R (*fn)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10))
            {
                ServiceProcedure* proc = new BasicServiceProcedure<R, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10>(callable(fn));
                this->registerProcedure(name, proc);
            }

            template <typename R>
            void registerCallable(const std::string& name, const Callable<R>& cb)
            {
                ServiceProcedure* proc = new BasicServiceProcedure<R>(cb);
                this->registerProcedure(name, proc);
            }

            template <typename R, typename A1>
            void registerCallable(const std::string& name, const Callable<R, A1>& cb)
            {
                ServiceProcedure* proc = new BasicServiceProcedure<R, A1>(cb);
                this->registerProcedure(name, proc);
            }

            template <typename R, typename A1, typename A2>
            void registerCallable(const std::string& name, const Callable<R, A1, A2>& cb)
            {
                ServiceProcedure* proc = new BasicServiceProcedure<R, A1, A2>(cb);
                this->registerProcedure(name, proc);
            }

            template <typename R, typename A1, typename A2, typename A3>
            void registerCallable(const std::string& name, const Callable<R, A1, A2, A3>& cb)
            {
                ServiceProcedure* proc = new BasicServiceProcedure<R, A1, A2, A3>(cb);
                this->registerProcedure(name, proc);
            }

            template <typename R, typename A1, typename A2, typename A3, typename A4>
            void registerCallable(const std::string& name, const Callable<R, A1, A2, A3, A4>& cb)
            {
                ServiceProcedure* proc = new BasicServiceProcedure<R, A1, A2, A3, A4>(cb);
                this->registerProcedure(name, proc);
            }

            template <typename R, typename A1, typename A2, typename A3, typename A4, typename A5>
            void registerCallable(const std::string& name, const Callable<R, A1, A2, A3, A4, A5>& cb)
            {
                ServiceProcedure* proc = new BasicServiceProcedure<R, A1, A2, A3, A4, A5>(cb);
                this->registerProcedure(name, proc);
            }

            template <typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
            void registerCallable(const std::string& name, const Callable<R, A1, A2, A3, A4, A5, A6>& cb)
            {
                ServiceProcedure* proc = new BasicServiceProcedure<R, A1, A2, A3, A4, A5, A6>(cb);
                this->registerProcedure(name, proc);
            }

            template <typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7>
            void registerCallable(const std::string& name, const Callable<R, A1, A2, A3, A4, A5, A6, A7>& cb)
            {
                ServiceProcedure* proc = new BasicServiceProcedure<R, A1, A2, A3, A4, A5, A6, A7>(cb);
                this->registerProcedure(name, proc);
            }

            template <typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8>
            void registerCallable(const std::string& name, const Callable<R, A1, A2, A3, A4, A5, A6, A7, A8>& cb)
            {
                ServiceProcedure* proc = new BasicServiceProcedure<R, A1, A2, A3, A4, A5, A6, A7, A8>(cb);
                this->registerProcedure(name, proc);
            }

            template <typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9>
            void registerCallable(const std::string& name, const Callable<R, A1, A2, A3, A4, A5, A6, A7, A8, A9>& cb)
            {
                ServiceProcedure* proc = new BasicServiceProcedure<R, A1, A2, A3, A4, A5, A6, A7, A8, A9>(cb);
                this->registerProcedure(name, proc);
            }

            template <typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10>
            void registerCallable(const std::string& name, const Callable<R, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10>& cb)
            {
                ServiceProcedure* proc = new BasicServiceProcedure<R, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10>(cb);
                this->registerProcedure(name, proc);
            }

            template <typename R, class C>
            void registerMethod(const std::string& name, C& obj, R (C::*method)() )
            {
                ServiceProcedure* proc = new BasicServiceProcedure<R>( callable(obj, method) );
                this->registerProcedure(name, proc);
            }

            template <typename R, class C, typename A1>
            void registerMethod(const std::string& name, C& obj, R (C::*method)(A1) )
            {
                ServiceProcedure* proc = new BasicServiceProcedure<R, A1>( callable(obj, method) );
                this->registerProcedure(name, proc);
            }

            template <typename R, class C, typename A1, typename A2>
            void registerMethod(const std::string& name, C& obj, R (C::*method)(A1, A2) )
            {
                ServiceProcedure* proc = new BasicServiceProcedure<R, A1, A2>( callable(obj, method) );
                this->registerProcedure(name, proc);
            }

            template <typename R, class C, typename A1, typename A2, typename A3>
            void registerMethod(const std::string& name, C& obj, R (C::*method)(A1, A2, A3) )
            {
                ServiceProcedure* proc = new BasicServiceProcedure<R, A1, A2, A3>( callable(obj, method) );
                this->registerProcedure(name, proc);
            }

            template <typename R, class C, typename A1, typename A2, typename A3, typename A4>
            void registerMethod(const std::string& name, C& obj, R (C::*method)(A1, A2, A3, A4) )
            {
                ServiceProcedure* proc = new BasicServiceProcedure<R, A1, A2, A3, A4>( callable(obj, method) );
                this->registerProcedure(name, proc);
            }

            template <typename R, class C, typename A1, typename A2, typename A3, typename A4, typename A5>
            void registerMethod(const std::string& name, C& obj, R (C::*method)(A1, A2, A3, A4, A5) )
            {
                ServiceProcedure* proc = new BasicServiceProcedure<R, A1, A2, A3, A4, A5>( callable(obj, method) );
                this->registerProcedure(name, proc);
            }

            template <typename R, class C, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
            void registerMethod(const std::string& name, C& obj, R (C::*method)(A1, A2, A3, A4, A5, A6) )
            {
                ServiceProcedure* proc = new BasicServiceProcedure<R, A1, A2, A3, A4, A5, A6>( callable(obj, method) );
                this->registerProcedure(name, proc);
            }

            template <typename R, class C, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7>
            void registerMethod(const std::string& name, C& obj, R (C::*method)(A1, A2, A3, A4, A5, A6, A7) )
            {
                ServiceProcedure* proc = new BasicServiceProcedure<R, A1, A2, A3, A4, A5, A6, A7>( callable(obj, method) );
                this->registerProcedure(name, proc);
            }

            template <typename R, class C, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8>
            void registerMethod(const std::string& name, C& obj, R (C::*method)(A1, A2, A3, A4, A5, A6, A7, A8) )
            {
                ServiceProcedure* proc = new BasicServiceProcedure<R, A1, A2, A3, A4, A5, A6, A7, A8>( callable(obj, method) );
                this->registerProcedure(name, proc);
            }

            template <typename R, class C, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9>
            void registerMethod(const std::string& name, C& obj, R (C::*method)(A1, A2, A3, A4, A5, A6, A7, A8, A9) )
            {
                ServiceProcedure* proc = new BasicServiceProcedure<R, A1, A2, A3, A4, A5, A6, A7, A8, A9>( callable(obj, method) );
                this->registerProcedure(name, proc);
            }

            template <typename R, class C, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10>
            void registerMethod(const std::string& name, C& obj, R (C::*method)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10) )
            {
                ServiceProcedure* proc = new BasicServiceProcedure<R, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10>( callable(obj, method) );
                this->registerProcedure(name, proc);
            }

            ServiceProcedure* getProcedure(const std::string& name) const;

            void releaseProcedure(ServiceProcedure* proc) const;

            std::vector<std::string> getProcedureNames() const;

        protected:
            void registerProcedure(const std::string& name, ServiceProcedure* proc);

        private:
            typedef std::map<std::string, ServiceProcedure*> ProcedureMap;
            ProcedureMap _procedures;
    };

}

#endif // CXXTOOLS_SERVICEREGISTRY_H

