/*
 * Copyright (C) 2009 by Dr. Marc Boris Duerner
 * Copyright (C) 2009 by Tommi Meakitalo
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
#ifndef cxxtools_xmlrpc_Service_h
#define cxxtools_xmlrpc_Service_h

#include <cxxtools/xmlrpc/api.h>
#include <cxxtools/httpserver.h>
#include <cxxtools/deserializer.h>
#include <cxxtools/serializer.h>
#include <cxxtools/void.h>
#include <cxxtools/method.h>
#include <cxxtools/typetraits.h>
#include <stdexcept>
#include <string>
#include <map>

namespace cxxtools {

namespace xmlrpc {

class ServiceProcedure
{
    public:
        ServiceProcedure()
        {}

        virtual ~ServiceProcedure()
        {}

        virtual IDeserializer** beginCall() = 0;

        virtual ISerializer* endCall() = 0;
};


template < typename R,
           class C,
           typename A1 = cxxtools::Void,
           typename A2 = cxxtools::Void>
class BasicServiceProcedure : public ServiceProcedure
{
    public:
        BasicServiceProcedure( const Callable<R, A1, A2>& cb )
        : ServiceProcedure()
        , _cb(0)
        {
            _cb = cb.clone();

            _args[0] = &_a1;
            _args[1] = &_a2;
            _args[2] = 0;
        }

        ~BasicServiceProcedure()
        {
            delete _cb;
        }

        IDeserializer** beginCall()
        {
            _a1.begin(_v1);
            _a2.begin(_v2);
            return _args;
        }

        ISerializer* endCall()
        {
            _rv = _cb->call(_v1, _v2);
            _r.begin(_rv);
            return &_r;
        }

    private:
        typedef typename TypeTraits<A1>::Value V1;
        typedef typename TypeTraits<A2>::Value V2;
        typedef typename TypeTraits<R>::Value RV;

        Callable<R, A1, A2>* _cb;
        RV _rv;
        V1 _v1;
        V2 _v2;
        IDeserializer* _args[3];
        Deserializer<V1> _a1;
        Deserializer<V2> _a2;
        Serializer<RV> _r;
};


template < typename R,
           class C,
           typename A1>
class BasicServiceProcedure<R, C, A1, cxxtools::Void> : public ServiceProcedure
{
    public:
        BasicServiceProcedure( const Callable<R, A1>& cb )
        : ServiceProcedure()
        , _cb(0)
        {
            _cb = cb.clone();

            _args[0] = &_a1;
            _args[1] = 0;
        }

        ~BasicServiceProcedure()
        {
            delete _cb;
        }

        IDeserializer** beginCall()
        {
            _a1.begin(_v1);
            return _args;
        }

        ISerializer* endCall()
        {
            _rv = _cb->call(_v1);
            _r.begin(_rv);
            return &_r;
        }

    private:
        typedef typename TypeTraits<A1>::Value V1;
        typedef typename TypeTraits<R>::Value RV;

        Callable<R, A1>* _cb;
        RV _rv;
        V1 _v1;
        IDeserializer* _args[2];
        Deserializer<V1> _a1;
        Serializer<RV> _r;
};


template < typename R,
           class C>
class BasicServiceProcedure<R, C, cxxtools::Void, cxxtools::Void> : public ServiceProcedure
{
    public:
        BasicServiceProcedure( const Callable<R>& cb )
        : ServiceProcedure()
        , _cb(0)
        {
            _cb = cb.clone();

            _args[0] = 0;
        }

        ~BasicServiceProcedure()
        {
            delete _cb;
        }

        IDeserializer** beginCall()
        {
            return _args;
        }

        ISerializer* endCall()
        {
            _rv = _cb->call();
            _r.begin(_rv);
            return &_r;
        }

    private:
        typedef typename TypeTraits<R>::Value RV;

        Callable<R>* _cb;
        RV _rv;
        IDeserializer* _args[1];
        Serializer<RV> _r;
};


class CXXTOOLS_XMLRPC_API Service : public net::HttpService
{
    public:
        Service();

        virtual ~Service();

        // TODO cache service procedures and clone on demand
        ServiceProcedure* procedure(const std::string& name);

        template <typename R, class C>
        void registerMethod(const std::string& name, C& obj, R (C::*method)() )
        {
            ServiceProcedure* proc = new BasicServiceProcedure<R, C>( callable(obj, method) );
            this->registerProcedure(name, proc);
        }

        template <typename R, class C, typename A1>
        void registerMethod(const std::string& name, C& obj, R (C::*method)(A1) )
        {
            ServiceProcedure* proc = new BasicServiceProcedure<R, C, A1>( callable(obj, method) );
            this->registerProcedure(name, proc);
        }

        template <typename R, class C, typename A1, typename A2>
        void registerMethod(const std::string& name, C& obj, R (C::*method)(A1, A2) )
        {
            ServiceProcedure* proc = new BasicServiceProcedure<R, C, A1, A2>( callable(obj, method) );
            this->registerProcedure(name, proc);
        }

        virtual net::HttpResponder* createResponder(const net::HttpRequest&);

        virtual void releaseResponder(net::HttpResponder* resp);

    protected:
        void registerProcedure(const std::string& name, ServiceProcedure* proc);

    private:
        typedef std::map<std::string, ServiceProcedure*> ProcedureMap;
        ProcedureMap _procedures;
};

}

}

#endif
