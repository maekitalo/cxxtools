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
#include <cxxtools/method.h>
#include <string>
#include <vector>
#include <map>
#include <memory>

namespace cxxtools
{
    class ServiceRegistry
    {
        public:
            ServiceRegistry() = default;

            template <typename R>
            void registerFunction(const std::string& name, R (*fn)())
            {
                this->registerProcedure(name, BasicServiceProcedure<R>(fn));
            }

            template <typename R>
            void registerFunction(const std::string& name, const std::function<R()>& fn)
            {
                this->registerProcedure(name, BasicServiceProcedure<R>(fn));
            }

            template <typename R, typename A1>
            void registerFunction(const std::string& name, R (*fn)(A1))
            {
                this->registerProcedure(name, BasicServiceProcedure<R, A1>(fn));
            }

            template <typename R, typename A1>
            void registerFunction(const std::string& name, const std::function<R(A1)>& fn)
            {
                this->registerProcedure(name, BasicServiceProcedure<R, A1>(fn));
            }

            template <typename R, typename A1, typename A2>
            void registerFunction(const std::string& name, R (fn)(A1, A2))
            {
                this->registerProcedure(name, BasicServiceProcedure<R, A1, A2>(fn));
            }

            template <typename R, typename A1, typename A2>
            void registerFunction(const std::string& name, const std::function<R(A1, A2)>& fn)
            {
                this->registerProcedure(name, BasicServiceProcedure<R, A1, A2>(fn));
            }

            template <typename R, typename A1, typename A2, typename A3>
            void registerFunction(const std::string& name, R (*fn)(A1, A2, A3))
            {
                this->registerProcedure(name, BasicServiceProcedure<R, A1, A2, A3>(fn));
            }

            template <typename R, typename A1, typename A2, typename A3>
            void registerFunction(const std::string& name, const std::function<R(A1, A2, A3)>& fn)
            {
                this->registerProcedure(name, BasicServiceProcedure<R, A1, A2, A3>(fn));
            }

            template <typename R, typename A1, typename A2, typename A3, typename A4>
            void registerFunction(const std::string& name, R (*fn)(A1, A2, A3, A4))
            {
                this->registerProcedure(name, BasicServiceProcedure<R, A1, A2, A3, A4>(fn));
            }

            template <typename R, typename A1, typename A2, typename A3, typename A4>
            void registerFunction(const std::string& name, const std::function<R(A1, A2, A3, A4)>& fn)
            {
                this->registerProcedure(name, BasicServiceProcedure<R, A1, A2, A3, A4>(fn));
            }

            template <typename R, typename A1, typename A2, typename A3, typename A4, typename A5>
            void registerFunction(const std::string& name, R (*fn)(A1, A2, A3, A4, A5))
            {
                this->registerProcedure(name, BasicServiceProcedure<R, A1, A2, A3, A4, A5>(fn));
            }

            template <typename R, typename A1, typename A2, typename A3, typename A4, typename A5>
            void registerFunction(const std::string& name, const std::function<R(A1, A2, A3, A4, A5)>& fn)
            {
                this->registerProcedure(name, BasicServiceProcedure<R, A1, A2, A3, A4, A5>(fn));
            }

            template <typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
            void registerFunction(const std::string& name, R (*fn)(A1, A2, A3, A4, A5, A6))
            {
                this->registerProcedure(name, BasicServiceProcedure<R, A1, A2, A3, A4, A5, A6>(fn));
            }

            template <typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
            void registerFunction(const std::string& name, const std::function<R(A1, A2, A3, A4, A5, A6)>& fn)
            {
                this->registerProcedure(name, BasicServiceProcedure<R, A1, A2, A3, A4, A5, A6>(fn));
            }

            template <typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7>
            void registerFunction(const std::string& name, R (*fn)(A1, A2, A3, A4, A5, A6, A7))
            {
                this->registerProcedure(name, BasicServiceProcedure<R, A1, A2, A3, A4, A5, A6, A7>(fn));
            }

            template <typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7>
            void registerFunction(const std::string& name, const std::function<R(A1, A2, A3, A4, A5, A6, A7)>& fn)
            {
                this->registerProcedure(name, BasicServiceProcedure<R, A1, A2, A3, A4, A5, A6, A7>(fn));
            }

            template <typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8>
            void registerFunction(const std::string& name, R (*fn)(A1, A2, A3, A4, A5, A6, A7, A8))
            {
                this->registerProcedure(name, BasicServiceProcedure<R, A1, A2, A3, A4, A5, A6, A7, A8>(fn));
            }

            template <typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8>
            void registerFunction(const std::string& name, const std::function<R(A1, A2, A3, A4, A5, A6, A7, A8)>& fn)
            {
                this->registerProcedure(name, BasicServiceProcedure<R, A1, A2, A3, A4, A5, A6, A7, A8>(fn));
            }

            template <typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9>
            void registerFunction(const std::string& name, R (*fn)(A1, A2, A3, A4, A5, A6, A7, A8, A9))
            {
                this->registerProcedure(name, BasicServiceProcedure<R, A1, A2, A3, A4, A5, A6, A7, A8, A9>(fn));
            }

            template <typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9>
            void registerFunction(const std::string& name, const std::function<R(A1, A2, A3, A4, A5, A6, A7, A8, A9)>& fn)
            {
                this->registerProcedure(name, BasicServiceProcedure<R, A1, A2, A3, A4, A5, A6, A7, A8, A9>(fn));
            }

            template <typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10>
            void registerFunction(const std::string& name, R (*fn)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10))
            {
                this->registerProcedure(name, BasicServiceProcedure<R, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10>(fn));
            }

            template <typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10>
            void registerFunction(const std::string& name, const std::function<R(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10)>& fn)
            {
                this->registerProcedure(name, BasicServiceProcedure<R, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10>(fn));
            }

            template <typename R, class C>
            void registerMethod(const std::string& name, C& obj, R (C::*method)() )
            {
                this->registerProcedure(name, BasicServiceProcedure<R>(std::bind(method, &obj)));
            }

            template <typename R, class C, typename A1>
            void registerMethod(const std::string& name, C& obj, R (C::*method)(A1) )
            {
                this->registerProcedure(name, BasicServiceProcedure<R, A1>(std::bind(method, &obj, std::placeholders::_1)));
            }

            template <typename R, class C, typename A1, typename A2>
            void registerMethod(const std::string& name, C& obj, R (C::*method)(A1, A2) )
            {
                this->registerProcedure(name, BasicServiceProcedure<R, A1, A2>(std::bind(method, &obj, std::placeholders::_1, std::placeholders::_2)));
            }

            template <typename R, class C, typename A1, typename A2, typename A3>
            void registerMethod(const std::string& name, C& obj, R (C::*method)(A1, A2, A3) )
            {
                this->registerProcedure(name, BasicServiceProcedure<R, A1, A2, A3>(std::bind(method, &obj, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)));
            }

            template <typename R, class C, typename A1, typename A2, typename A3, typename A4>
            void registerMethod(const std::string& name, C& obj, R (C::*method)(A1, A2, A3, A4) )
            {
                this->registerProcedure(name, BasicServiceProcedure<R, A1, A2, A3, A4>(std::bind(method, &obj, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4)));
            }

            template <typename R, class C, typename A1, typename A2, typename A3, typename A4, typename A5>
            void registerMethod(const std::string& name, C& obj, R (C::*method)(A1, A2, A3, A4, A5) )
            {
                this->registerProcedure(name, BasicServiceProcedure<R, A1, A2, A3, A4, A5>(std::bind(method, &obj, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5)));
            }

            template <typename R, class C, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
            void registerMethod(const std::string& name, C& obj, R (C::*method)(A1, A2, A3, A4, A5, A6) )
            {
                this->registerProcedure(name, BasicServiceProcedure<R, A1, A2, A3, A4, A5, A6>(std::bind(method, &obj, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6)));
            }

            template <typename R, class C, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7>
            void registerMethod(const std::string& name, C& obj, R (C::*method)(A1, A2, A3, A4, A5, A6, A7) )
            {
                this->registerProcedure(name, BasicServiceProcedure<R, A1, A2, A3, A4, A5, A6, A7>(std::bind(method, &obj, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6, std::placeholders::_7)));
            }

            template <typename R, class C, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8>
            void registerMethod(const std::string& name, C& obj, R (C::*method)(A1, A2, A3, A4, A5, A6, A7, A8) )
            {
                this->registerProcedure(name, BasicServiceProcedure<R, A1, A2, A3, A4, A5, A6, A7, A8>(std::bind(method, &obj, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6, std::placeholders::_7, std::placeholders::_8)));
            }

            template <typename R, class C, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9>
            void registerMethod(const std::string& name, C& obj, R (C::*method)(A1, A2, A3, A4, A5, A6, A7, A8, A9) )
            {
                this->registerProcedure(name, BasicServiceProcedure<R, A1, A2, A3, A4, A5, A6, A7, A8, A9>(std::bind(method, &obj, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6, std::placeholders::_7, std::placeholders::_8, std::placeholders::_9)));
            }

            template <typename R, class C, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10>
            void registerMethod(const std::string& name, C& obj, R (C::*method)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10) )
            {
                this->registerProcedure(name, BasicServiceProcedure<R, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10>(std::bind(method, &obj, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6, std::placeholders::_7, std::placeholders::_8, std::placeholders::_9, std::placeholders::_10)));
            }


           std::unique_ptr<ServiceProcedure> getProcedure(const std::string& name) const;

            std::vector<std::string> getProcedureNames() const;

            void registerProcedure(const std::string& name, const ServiceProcedure& proc);
            void registerDefaultProcedure(const ServiceProcedure& proc)
                { _defaultProcedure = std::unique_ptr<ServiceProcedure>(proc.clone()); }

        private:
            typedef std::map<std::string, std::unique_ptr<ServiceProcedure>> ProcedureMap;
            std::unique_ptr<ServiceProcedure> _defaultProcedure;
            ProcedureMap _procedures;
    };

}

#endif // CXXTOOLS_SERVICEREGISTRY_H

