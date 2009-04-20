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
#include <cxxtools/http/server.h>
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

        virtual ServiceProcedure* clone() const = 0;

        virtual IDeserializer** beginCall() = 0;

        virtual ISerializer* endCall() = 0;
};


template < typename R,
           class C,
           typename A1 = cxxtools::Void,
           typename A2 = cxxtools::Void,
           typename A3 = cxxtools::Void,
           typename A4 = cxxtools::Void,
           typename A5 = cxxtools::Void>
class BasicServiceProcedure : public ServiceProcedure
{
    public:
        typedef Callable<R, A1, A2, A3, A4, A5> CallableT;

        BasicServiceProcedure( const CallableT& cb )
        : ServiceProcedure()
        , _cb(0)
        {
            _cb = cb.clone();

            _args[0] = &_a1;
            _args[1] = &_a2;
            _args[2] = &_a3;
            _args[3] = &_a4;
            _args[4] = &_a5;
            _args[5] = 0;
        }

        ~BasicServiceProcedure()
        {
            delete _cb;
        }

        ServiceProcedure* clone() const
        {
            return new BasicServiceProcedure(*_cb);
        }

        IDeserializer** beginCall()
        {
            _a1.begin(_v1);
            _a2.begin(_v2);
            _a3.begin(_v3);
            _a4.begin(_v4);
            _a5.begin(_v5);
            return _args;
        }

        ISerializer* endCall()
        {
            _rv = _cb->call(_v1, _v2, _v3, _v4, _v5);
            _r.begin(_rv);
            return &_r;
        }

    private:
        typedef typename TypeTraits<A1>::Value V1;
        typedef typename TypeTraits<A2>::Value V2;
        typedef typename TypeTraits<A3>::Value V3;
        typedef typename TypeTraits<A4>::Value V4;
        typedef typename TypeTraits<A5>::Value V5;
        typedef typename TypeTraits<R>::Value RV;

        CallableT* _cb;
        RV _rv;
        V1 _v1;
        V2 _v2;
        V3 _v3;
        V4 _v4;
        V5 _v5;

        IDeserializer* _args[6];
        Deserializer<V1> _a1;
        Deserializer<V2> _a2;
        Deserializer<V3> _a3;
        Deserializer<V4> _a4;
        Deserializer<V5> _a5;
        Serializer<RV> _r;
};


template < typename R,
           class C,
           typename A1,
           typename A2,
           typename A3,
           typename A4>
class BasicServiceProcedure<R, C, A1, A2, A3, A4,
                            cxxtools::Void> : public ServiceProcedure
{
    public:
        typedef Callable<R, A1, A2, A3, A4> CallableT;

        BasicServiceProcedure( const CallableT& cb )
        : ServiceProcedure()
        , _cb(0)
        {
            _cb = cb.clone();

            _args[0] = &_a1;
            _args[1] = &_a2;
            _args[2] = &_a3;
            _args[3] = &_a4;
            _args[4] = 0;
        }

        ~BasicServiceProcedure()
        {
            delete _cb;
        }

        ServiceProcedure* clone() const
        {
            return new BasicServiceProcedure(*_cb);
        }

        IDeserializer** beginCall()
        {
            _a1.begin(_v1);
            _a2.begin(_v2);
            _a3.begin(_v3);
            _a4.begin(_v4);

            return _args;
        }

        ISerializer* endCall()
        {
            _rv = _cb->call(_v1, _v2, _v3, _v4);
            _r.begin(_rv);
            return &_r;
        }

    private:
        typedef typename TypeTraits<A1>::Value V1;
        typedef typename TypeTraits<A2>::Value V2;
        typedef typename TypeTraits<A3>::Value V3;
        typedef typename TypeTraits<A4>::Value V4;

        typedef typename TypeTraits<R>::Value RV;

        CallableT* _cb;
        RV _rv;
        V1 _v1;
        V2 _v2;
        V3 _v3;
        V4 _v4;

        IDeserializer* _args[5];
        Deserializer<V1> _a1;
        Deserializer<V2> _a2;
        Deserializer<V3> _a3;
        Deserializer<V4> _a4;
        Serializer<RV> _r;
};


template < typename R,
           class C,
           typename A1,
           typename A2,
           typename A3>
class BasicServiceProcedure<R, C, A1, A2, A3,
                            cxxtools::Void,
                            cxxtools::Void> : public ServiceProcedure
{
    public:
        typedef Callable<R, A1, A2, A3> CallableT;

        BasicServiceProcedure( const CallableT& cb )
        : ServiceProcedure()
        , _cb(0)
        {
            _cb = cb.clone();

            _args[0] = &_a1;
            _args[1] = &_a2;
            _args[2] = &_a3;
            _args[3] = 0;
        }

        ~BasicServiceProcedure()
        {
            delete _cb;
        }

        ServiceProcedure* clone() const
        {
            return new BasicServiceProcedure(*_cb);
        }

        IDeserializer** beginCall()
        {
            _a1.begin(_v1);
            _a2.begin(_v2);
            _a3.begin(_v3);

            return _args;
        }

        ISerializer* endCall()
        {
            _rv = _cb->call(_v1, _v2, _v3);
            _r.begin(_rv);
            return &_r;
        }

    private:
        typedef typename TypeTraits<A1>::Value V1;
        typedef typename TypeTraits<A2>::Value V2;
        typedef typename TypeTraits<A3>::Value V3;

        typedef typename TypeTraits<R>::Value RV;

        CallableT* _cb;
        RV _rv;
        V1 _v1;
        V2 _v2;
        V3 _v3;

        IDeserializer* _args[4];
        Deserializer<V1> _a1;
        Deserializer<V2> _a2;
        Deserializer<V3> _a3;
        Serializer<RV> _r;
};


template < typename R,
           class C,
           typename A1,
           typename A2>
class BasicServiceProcedure<R, C, A1, A2,
                            cxxtools::Void,
                            cxxtools::Void,
                            cxxtools::Void> : public ServiceProcedure
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

        ServiceProcedure* clone() const
        {
            return new BasicServiceProcedure(*_cb);
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
class BasicServiceProcedure<R, C, A1,
                            cxxtools::Void,
                            cxxtools::Void,
                            cxxtools::Void,
                            cxxtools::Void> : public ServiceProcedure
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

        ServiceProcedure* clone() const
        {
            return new BasicServiceProcedure(*_cb);
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
class BasicServiceProcedure<R, C, cxxtools::Void,
                                  cxxtools::Void,
                                  cxxtools::Void,
                                  cxxtools::Void,
                                  cxxtools::Void> : public ServiceProcedure
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

        ServiceProcedure* clone() const
        {
            return new BasicServiceProcedure(*_cb);
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


class CXXTOOLS_XMLRPC_API Service : public http::Service
{
    public:
        Service();

        virtual ~Service();

        // TODO cache service procedures and clone on demand
        ServiceProcedure* getProcedure(const std::string& name);

        void releaseProcedure(ServiceProcedure* proc);

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

        template <typename R, class C, typename A1, typename A2, typename A3>
        void registerMethod(const std::string& name, C& obj, R (C::*method)(A1, A2, A3) )
        {
            ServiceProcedure* proc = new BasicServiceProcedure<R, C, A1, A2, A3>( callable(obj, method) );
            this->registerProcedure(name, proc);
        }

        template <typename R, class C, typename A1, typename A2, typename A3, typename A4>
        void registerMethod(const std::string& name, C& obj, R (C::*method)(A1, A2, A3, A4) )
        {
            ServiceProcedure* proc = new BasicServiceProcedure<R, C, A1, A2, A3, A4>( callable(obj, method) );
            this->registerProcedure(name, proc);
        }

        template <typename R, class C, typename A1, typename A2, typename A3, typename A4, typename A5>
        void registerMethod(const std::string& name, C& obj, R (C::*method)(A1, A2, A3, A4, A5) )
        {
            ServiceProcedure* proc = new BasicServiceProcedure<R, C, A1, A2, A3, A4, A5>( callable(obj, method) );
            this->registerProcedure(name, proc);
        }

        virtual http::Responder* createResponder(const http::Request&);

        virtual void releaseResponder(http::Responder* resp);

    protected:
        void registerProcedure(const std::string& name, ServiceProcedure* proc);

    private:
        typedef std::map<std::string, ServiceProcedure*> ProcedureMap;
        ProcedureMap _procedures;
};

}

}

#endif
