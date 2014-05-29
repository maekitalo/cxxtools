/*
 * Copyright (C) 2012 by Tommi Maekitalo
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
#include "cxxtools/unit/testsuite.h"
#include "cxxtools/unit/registertest.h"
#include "cxxtools/json/httpservice.h"
#include "cxxtools/json/httpclient.h"
#include "cxxtools/remoteexception.h"
#include "cxxtools/remoteprocedure.h"
#include "cxxtools/http/server.h"
#include "cxxtools/eventloop.h"
#include "cxxtools/log.h"
#include "cxxtools/ioerror.h"
#include "cxxtools/net/uri.h"
#include "cxxtools/net/addrinfo.h"
#include <stdlib.h>
#include <sstream>

log_define("cxxtools.test.jsonrpchttp")

namespace
{
    struct Color
    {
        int red;
        int green;
        int blue;
    };


    typedef std::set<int> IntSet;
    typedef std::multiset<int> IntMultiset;
    typedef std::map<int, int> IntMap;
    typedef std::multimap<int, int> IntMultimap;


    void operator >>=(const cxxtools::SerializationInfo& si, Color& color)
    {
        si.getMember("red") >>= color.red;
        si.getMember("green") >>= color.green;
        si.getMember("blue") >>= color.blue;
    }


    void operator <<=(cxxtools::SerializationInfo& si, const Color& color)
    {
        si.addMember("red") <<= color.red;
        si.addMember("green") <<= color.green;
        si.addMember("blue") <<= color.blue;
    }

}


class JsonRpcHttpTest : public cxxtools::unit::TestSuite
{
    private:
        cxxtools::EventLoop _loop;
        cxxtools::http::Server* _server;
        unsigned _count;
        unsigned short _port;

    public:
        JsonRpcHttpTest()
        : cxxtools::unit::TestSuite("jsonrpchttp"),
          _port(8001)
        {
            registerMethod("Nothing", *this, &JsonRpcHttpTest::Nothing);
            registerMethod("Boolean", *this, &JsonRpcHttpTest::Boolean);
            registerMethod("Integer", *this, &JsonRpcHttpTest::Integer);
            registerMethod("Double", *this, &JsonRpcHttpTest::Double);
            registerMethod("String", *this, &JsonRpcHttpTest::String);
            registerMethod("EmptyValues", *this, &JsonRpcHttpTest::EmptyValues);
            registerMethod("Array", *this, &JsonRpcHttpTest::Array);
            registerMethod("EmptyArray", *this, &JsonRpcHttpTest::EmptyArray);
            registerMethod("Struct", *this, &JsonRpcHttpTest::Struct);
            registerMethod("Set", *this, &JsonRpcHttpTest::Set);
            registerMethod("Multiset", *this, &JsonRpcHttpTest::Multiset);
            registerMethod("Map", *this, &JsonRpcHttpTest::Map);
            registerMethod("Multimap", *this, &JsonRpcHttpTest::Multimap);
            registerMethod("UnknownMethod", *this, &JsonRpcHttpTest::UnknownMethod);
            registerMethod("Fault", *this, &JsonRpcHttpTest::Fault);
            registerMethod("Exception", *this, &JsonRpcHttpTest::Exception);
            registerMethod("CallbackException", *this, &JsonRpcHttpTest::CallbackException);
            registerMethod("ConnectError", *this, &JsonRpcHttpTest::ConnectError);
            registerMethod("BigRequest", *this, &JsonRpcHttpTest::BigRequest);
            registerMethod("PrepareConnect", *this, &JsonRpcHttpTest::PrepareConnect);
            registerMethod("Connect", *this, &JsonRpcHttpTest::Connect);
            registerMethod("Multiple", *this, &JsonRpcHttpTest::Multiple);

            char* PORT = getenv("UTEST_PORT");
            if (PORT)
            {
                std::istringstream s(PORT);
                s >> _port;
            }

            _loop.setIdleTimeout(2000);
            connect(_loop.timeout, *this, &JsonRpcHttpTest::failTest);
            connect(_loop.timeout, _loop, &cxxtools::EventLoop::exit);

        }

        void failTest()
        {
            throw cxxtools::unit::Assertion("test timed out", CXXTOOLS_SOURCEINFO);
        }

        void setUp()
        {
            _server = new cxxtools::http::Server(_loop, _port);
            _server->minThreads(1);
        }

        void tearDown()
        {
            delete _server;
        }

        ////////////////////////////////////////////////////////////
        // Nothing
        //
        void Nothing()
        {
            cxxtools::json::HttpService service;
            service.registerMethod("multiply", *this, &JsonRpcHttpTest::multiplyNothing);
            _server->addService("/calc", service);

            cxxtools::json::HttpClient client(_loop, "", _port, "/calc");
            cxxtools::RemoteProcedure<bool> multiply(client, "multiply");

            multiply.begin();
            CXXTOOLS_UNIT_ASSERT_EQUALS(multiply.end(2000), false);
        }

        bool multiplyNothing()
        {
            return false;
        }

        ////////////////////////////////////////////////////////////
        // CallbackException
        //
        void CallbackException()
        {
            cxxtools::json::HttpService service;
            service.registerMethod("multiply", *this, &JsonRpcHttpTest::multiplyNothing);
            _server->addService("/calc", service);

            cxxtools::json::HttpClient client(_loop, "", _port, "/calc");
            cxxtools::RemoteProcedure<bool> multiply(client, "multiply");
            connect( multiply.finished, *this, &JsonRpcHttpTest::onExceptionCallback );

            multiply.begin();

            _count = 0;
            CXXTOOLS_UNIT_ASSERT_THROW(_loop.run(), std::runtime_error);
            CXXTOOLS_UNIT_ASSERT_EQUALS(_count, 1);
        }

        void onExceptionCallback(const cxxtools::RemoteResult<bool>& r)
        {
            log_warn("exception callback");
            ++_count;
            _loop.exit();
            throw std::runtime_error("my error");
        }

        ////////////////////////////////////////////////////////////
        // ConnectError
        //
        void ConnectError()
        {
            log_trace("ConnectError");

            cxxtools::json::HttpClient client(_loop, "", _port + 1, "/calc");
            cxxtools::RemoteProcedure<bool> multiply(client, "multiply");
            connect( multiply.finished, *this, &JsonRpcHttpTest::onConnectErrorCallback );

            multiply.begin();

            try
            {
                _loop.run();
            }
            catch (const std::exception& e)
            {
                log_error("loop exited with exception: " << e.what());
                CXXTOOLS_UNIT_ASSERT_MSG(false, std::string("unexpected exception ") + typeid(e).name() + ": " + e.what());
            }
        }

        void onConnectErrorCallback(const cxxtools::RemoteResult<bool>& r)
        {
            log_debug("onConnectErrorCallback");
            _loop.exit();
            CXXTOOLS_UNIT_ASSERT_THROW(r.get(), std::exception);
        }

        ////////////////////////////////////////////////////////////
        // Boolean
        //
        void Boolean()
        {
            cxxtools::json::HttpService service;
            service.registerMethod("boolean", *this, &JsonRpcHttpTest::boolean);
            _server->addService("/calc", service);

            cxxtools::json::HttpClient client(_loop, "", _port, "/calc");
            cxxtools::RemoteProcedure<bool, bool, bool> boolean(client, "boolean");
            boolean.begin(true, true);
            bool r = boolean.end(2000);
            CXXTOOLS_UNIT_ASSERT_EQUALS(r, true);
        }

        bool boolean(bool a, bool b)
        {
            CXXTOOLS_UNIT_ASSERT_EQUALS(a, true);
            CXXTOOLS_UNIT_ASSERT_EQUALS(b, true);
            return true;
        }

        ////////////////////////////////////////////////////////////
        // Integer
        //
        void Integer()
        {
            cxxtools::json::HttpService service;
            service.registerMethod("multiply", *this, &JsonRpcHttpTest::multiplyInt);
            _server->addService("/calc", service);

            cxxtools::json::HttpClient client(_loop, "", _port, "/calc");
            cxxtools::RemoteProcedure<int, int, int> multiply(client, "multiply");

            multiply.begin(2, 3);
            CXXTOOLS_UNIT_ASSERT_EQUALS(multiply.end(2000), 6);
        }

        int multiplyInt(int a, int b)
        {
            return a*b;
        }

        ////////////////////////////////////////////////////////////
        // Double
        //
        void Double()
        {
            cxxtools::json::HttpService service;
            service.registerMethod("multiply", *this, &JsonRpcHttpTest::multiplyDouble);
            _server->addService("/calc", service);

            cxxtools::json::HttpClient client(_loop, "", _port, "/calc");
            cxxtools::RemoteProcedure<double, double, double> multiply(client, "multiply");

            multiply.begin(2.0, 3.0);
            CXXTOOLS_UNIT_ASSERT_EQUALS(multiply.end(2000), 6.0);
        }

        double multiplyDouble(double a, double b)
        {
            return a*b;
        }

        ////////////////////////////////////////////////////////////
        // String
        //
        void String()
        {
            cxxtools::json::HttpService service;
            service.registerMethod("echoString", *this, &JsonRpcHttpTest::echoString);
            _server->addService("/foo", service);

            cxxtools::json::HttpClient client(_loop, "", _port, "/foo");
            cxxtools::RemoteProcedure<std::string, std::string> echo(client, "echoString");

            echo.begin("\xc3\xaf\xc2\xbb\xc2\xbf'\"&<> foo?");
            CXXTOOLS_UNIT_ASSERT_EQUALS(echo.end(2000), "\xc3\xaf\xc2\xbb\xc2\xbf'\"&<> foo?");
        }

        std::string echoString(std::string a)
        {
            return a;
        }

        ////////////////////////////////////////////////////////////
        // EmptyValues
        //
        void EmptyValues()
        {
            cxxtools::json::HttpService service;
            service.registerMethod("multiply", *this, &JsonRpcHttpTest::multiplyEmpty);
            _server->addService("/calc", service);

            cxxtools::json::HttpClient client(_loop, "", _port, "/calc");
            cxxtools::RemoteProcedure<std::string, std::string, std::string> multiply(client, "multiply");

            multiply.begin("", "");
            CXXTOOLS_UNIT_ASSERT_EQUALS(multiply.end(2000), "4");
        }

        std::string multiplyEmpty(std::string a, std::string b)
        {
            CXXTOOLS_UNIT_ASSERT_EQUALS(a, "");
            CXXTOOLS_UNIT_ASSERT_EQUALS(b, "");
            return "4";
        }

        ////////////////////////////////////////////////////////////
        // Array
        //
        void Array()
        {
            cxxtools::json::HttpService service;
            service.registerMethod("multiply", *this, &JsonRpcHttpTest::multiplyVector);
            _server->addService("/calc", service);

            cxxtools::json::HttpClient client(_loop, "", _port, "/calc");
            cxxtools::RemoteProcedure< std::vector<int>, std::vector<int>, std::vector<int> > multiply(client, "multiply");

            std::vector<int> vec;
            vec.push_back(10);
            vec.push_back(20);

            multiply.begin(vec, vec);
            std::vector<int> r = multiply.end(2000);
            CXXTOOLS_UNIT_ASSERT_EQUALS(r.size(), 2);
            CXXTOOLS_UNIT_ASSERT_EQUALS(r.at(0), 100);
            CXXTOOLS_UNIT_ASSERT_EQUALS(r.at(1), 400);
        }

        std::vector<int> multiplyVector(const std::vector<int>& a, const std::vector<int>& b)
        {
            std::vector<int> r;
            if( a.size() )
            {
                r.push_back( a.at(0) * b.at(0) );
                r.push_back( a.at(1) * b.at(1) );
            }

            return r;
        }

        ////////////////////////////////////////////////////////////
        // EmptyArray
        //
        void EmptyArray()
        {
            cxxtools::json::HttpService service;
            service.registerMethod("multiply", *this, &JsonRpcHttpTest::multiplyVector);
            _server->addService("/calc", service);

            cxxtools::json::HttpClient client(_loop, "", _port, "/calc");
            cxxtools::RemoteProcedure< std::vector<int>, std::vector<int>, std::vector<int> > multiply(client, "multiply");

            std::vector<int> vec;
            multiply.begin(vec, vec);
            CXXTOOLS_UNIT_ASSERT_EQUALS(multiply.end(2000).size(), 0);
        }

        ////////////////////////////////////////////////////////////
        // Struct
        //
        void Struct()
        {
            cxxtools::json::HttpService service;
            service.registerMethod("multiply", *this, &JsonRpcHttpTest::multiplyColor);
            _server->addService("/calc", service);

            cxxtools::json::HttpClient client(_loop, "", _port, "/calc");
            cxxtools::RemoteProcedure< Color, Color, Color > multiply(client, "multiply");

            Color a;
            a.red = 2;
            a.green = 3;
            a.blue = 4;

            Color b;
            b.red = 3;
            b.green = 4;
            b.blue = 5;

            multiply.begin(a, b);
            Color r = multiply.end(2000);
            CXXTOOLS_UNIT_ASSERT_EQUALS(r.red, 6);
            CXXTOOLS_UNIT_ASSERT_EQUALS(r.green, 12);
            CXXTOOLS_UNIT_ASSERT_EQUALS(r.blue, 20);
        }

        Color multiplyColor(const Color& a, const Color& b)
        {
            Color color;
            color.red = a.red * b.red;
            color.green = a.green * b.green;
            color.blue = a.blue * b.blue;
            return color;
        }

        ////////////////////////////////////////////////////////////
        // Set
        //
        void Set()
        {
            cxxtools::json::HttpService service;
            service.registerMethod("multiplyset", *this, &JsonRpcHttpTest::multiplySet);
            _server->addService("/test", service);

            cxxtools::json::HttpClient client(_loop, "", _port, "/test");
            cxxtools::RemoteProcedure<IntSet, IntSet, int> multiply(client, "multiplyset");

            IntSet myset;
            myset.insert(4);
            myset.insert(5);
            myset.insert(11);
            myset.insert(5);

            multiply.begin(myset, 2);
            const IntSet& v = multiply.end(2000);
            CXXTOOLS_UNIT_ASSERT_EQUALS(v.size(), 3);
            CXXTOOLS_UNIT_ASSERT(v.find(8) != v.end());
            CXXTOOLS_UNIT_ASSERT(v.find(10) != v.end());
            CXXTOOLS_UNIT_ASSERT(v.find(22) != v.end());
        }

        IntSet multiplySet(const IntSet& s, int f)
        {
            IntSet ret;
            for (IntSet::const_iterator it = s.begin(); it != s.end(); ++it)
                ret.insert(*it * f);
            return ret;
        }

        ////////////////////////////////////////////////////////////
        // Multiset
        //
        void Multiset()
        {
            cxxtools::json::HttpService service;
            service.registerMethod("multiplyset", *this, &JsonRpcHttpTest::multiplyMultiset);
            _server->addService("/test", service);

            cxxtools::json::HttpClient client(_loop, "", _port, "/test");
            cxxtools::RemoteProcedure<IntMultiset, IntMultiset, int> multiply(client, "multiplyset");

            IntMultiset myset;
            myset.insert(4);
            myset.insert(5);
            myset.insert(11);
            myset.insert(5);

            multiply.begin(myset, 2);
            const IntMultiset& v = multiply.end(2000);
            CXXTOOLS_UNIT_ASSERT_EQUALS(v.size(), 4);
            CXXTOOLS_UNIT_ASSERT_EQUALS(v.count(8), 1);
            CXXTOOLS_UNIT_ASSERT_EQUALS(v.count(10), 2);
            CXXTOOLS_UNIT_ASSERT_EQUALS(v.count(22), 1);
        }

        IntMultiset multiplyMultiset(const IntMultiset& s, int f)
        {
            IntMultiset ret;
            for (IntMultiset::const_iterator it = s.begin(); it != s.end(); ++it)
                ret.insert(*it * f);
            return ret;
        }

        ////////////////////////////////////////////////////////////
        // Map
        //
        void Map()
        {
            cxxtools::json::HttpService service;
            service.registerMethod("multiplymap", *this, &JsonRpcHttpTest::multiplyMap);

            _server->addService("/test", service);

            cxxtools::json::HttpClient client(_loop, "", _port, "/test");
            cxxtools::RemoteProcedure<IntMap, IntMap, int> multiply(client, "multiplymap");

            IntMap mymap;
            mymap[2] = 4;
            mymap[7] = 7;
            mymap[1] = -1;

            multiply.begin(mymap, 2);
            const IntMap& v = multiply.end(2000);

            CXXTOOLS_UNIT_ASSERT_EQUALS(v.size(), 3);
            CXXTOOLS_UNIT_ASSERT(v.find(2) != v.end());
            CXXTOOLS_UNIT_ASSERT_EQUALS(v.find(2)->second, 8);
            CXXTOOLS_UNIT_ASSERT(v.find(7) != v.end());
            CXXTOOLS_UNIT_ASSERT_EQUALS(v.find(7)->second, 14);
            CXXTOOLS_UNIT_ASSERT(v.find(1) != v.end());
            CXXTOOLS_UNIT_ASSERT_EQUALS(v.find(1)->second, -2);
        }

        IntMap multiplyMap(const IntMap& m, int f)
        {
            IntMap ret;
            for (IntMap::const_iterator it = m.begin(); it != m.end(); ++it)
            {
                ret[it->first] = it->second * f;
            }

            return ret;
        }

        ////////////////////////////////////////////////////////////
        // Multimap
        //
        void Multimap()
        {
            cxxtools::json::HttpService service;
            service.registerMethod("multiplymultimap", *this, &JsonRpcHttpTest::multiplyMultimap);

            _server->addService("/test", service);

            cxxtools::json::HttpClient client(_loop, "", _port, "/test");
            cxxtools::RemoteProcedure<IntMultimap, IntMultimap, int> multiply(client, "multiplymultimap");

            IntMultimap mymap;
            mymap.insert(IntMultimap::value_type(2, 4));
            mymap.insert(IntMultimap::value_type(7, 7));
            mymap.insert(IntMultimap::value_type(7, 8));
            mymap.insert(IntMultimap::value_type(1, -1));

            multiply.begin(mymap, 2);
            const IntMultimap& v = multiply.end(200);

            CXXTOOLS_UNIT_ASSERT_EQUALS(v.size(), 4);
            CXXTOOLS_UNIT_ASSERT(v.lower_bound(2) != v.end());
            CXXTOOLS_UNIT_ASSERT_EQUALS(v.lower_bound(2)->second, 8);
            CXXTOOLS_UNIT_ASSERT(v.lower_bound(7) != v.end());
            CXXTOOLS_UNIT_ASSERT_EQUALS(v.lower_bound(7)->second, 14);
            IntMultimap::const_iterator it = v.lower_bound(7);
            ++it;
            CXXTOOLS_UNIT_ASSERT(it != v.end());
            CXXTOOLS_UNIT_ASSERT_EQUALS(it->first, 7);
            CXXTOOLS_UNIT_ASSERT_EQUALS(it->second, 16);
            CXXTOOLS_UNIT_ASSERT(v.lower_bound(1) != v.end());
            CXXTOOLS_UNIT_ASSERT_EQUALS(v.lower_bound(1)->second, -2);
        }

        IntMultimap multiplyMultimap(const IntMultimap& m, int f)
        {
            IntMultimap ret;
            for (IntMultimap::const_iterator it = m.begin(); it != m.end(); ++it)
            {
                ret.insert(IntMultimap::value_type(it->first, it->second * f));
            }

            return ret;
        }

        ////////////////////////////////////////////////////////////
        // UnknownMethod
        //
        void UnknownMethod()
        {
            cxxtools::json::HttpClient client(_loop, "", _port, "/test");
            cxxtools::json::HttpService service;
            _server->addService("/test", service);
            cxxtools::RemoteProcedure<bool, bool, bool> unknownMethod(client, "unknownMethod");
            unknownMethod.begin(true, true);
            CXXTOOLS_UNIT_ASSERT_THROW(unknownMethod.end(2000), cxxtools::RemoteException);
        }

        ////////////////////////////////////////////////////////////
        // Fault
        //
        void Fault()
        {
            cxxtools::json::HttpService service;
            service.registerMethod("multiply", *this, &JsonRpcHttpTest::throwFault);
            _server->addService("/calc", service);

            cxxtools::json::HttpClient client(_loop, "", _port, "/calc");
            cxxtools::RemoteProcedure<bool> multiply(client, "multiply");
            multiply.begin();

            try
            {
                multiply.end(2000);
                CXXTOOLS_UNIT_ASSERT_MSG(false, "cxxtools::RemoteException exception expected");
            }
            catch (const cxxtools::RemoteException& e)
            {
                CXXTOOLS_UNIT_ASSERT_EQUALS(e.rc(), 7);
                CXXTOOLS_UNIT_ASSERT_EQUALS(e.text(), "Fault");
            }
        }

        bool throwFault()
        {
            throw cxxtools::RemoteException("Fault", 7);
            return false;
        }

        ////////////////////////////////////////////////////////////
        // Exception
        //
        void Exception()
        {
            cxxtools::json::HttpService service;
            service.registerMethod("multiply", *this, &JsonRpcHttpTest::throwException);
            _server->addService("/calc", service);

            cxxtools::json::HttpClient client(_loop, "", _port, "/calc");
            cxxtools::RemoteProcedure<bool> multiply(client, "multiply");

            multiply.begin();

            try
            {
                multiply.end(2000);
                CXXTOOLS_UNIT_ASSERT(false);
            }
            catch (const cxxtools::RemoteException& e)
            {
                CXXTOOLS_UNIT_ASSERT_EQUALS(e.text(), "Exception");
            }
        }

        bool throwException()
        {
            throw std::runtime_error("Exception");
            return false;
        }

        ////////////////////////////////////////////////////////////
        // BigRequest
        //
        void BigRequest()
        {
            cxxtools::json::HttpService service;
            service.registerMethod("countSize", *this, &JsonRpcHttpTest::countSize);
            _server->addService("/rpc", service);

            cxxtools::json::HttpClient client(_loop, "", _port, "/rpc");
            cxxtools::RemoteProcedure<unsigned, std::vector<int> > countSize(client, "countSize");

            std::vector<int> v;
            v.resize(5000);

            countSize.begin(v);

            try
            {
                CXXTOOLS_UNIT_ASSERT_EQUALS(countSize.end(2000), 5000);
            }
            catch (const std::exception& e)
            {
                log_error("loop exited with exception: " << e.what());
                CXXTOOLS_UNIT_ASSERT_MSG(false, std::string("unexpected exception ") + typeid(e).name() + ": " + e.what());
            }
        }

        unsigned countSize(const std::vector<int>& v)
        {
            return v.size();
        }

        ////////////////////////////////////////////////////////////
        // PrepareConnect
        //
        void PrepareConnect()
        {
            log_trace("PrepareConnect");

            cxxtools::json::HttpService service;
            service.registerMethod("boolean", *this, &JsonRpcHttpTest::boolean);
            _server->addService("/rpc", service);

            // test connect using cxxtools::net::AddrInfo
            {
                cxxtools::json::HttpClient client(_loop);
                cxxtools::RemoteProcedure<bool, bool, bool> boolean(client, "boolean");

                client.prepareConnect(cxxtools::net::AddrInfo("", _port), "/rpc");
                boolean.begin(true, true);
                CXXTOOLS_UNIT_ASSERT_EQUALS(boolean.end(2000), true);
            }

            // test connect using host and port
            {
                cxxtools::json::HttpClient client(_loop);
                cxxtools::RemoteProcedure<bool, bool, bool> boolean(client, "boolean");

                client.prepareConnect("", _port, "/rpc");
                boolean.begin(true, true);
                CXXTOOLS_UNIT_ASSERT_EQUALS(boolean.end(2000), true);
            }

            // test connect using uri
            {
                cxxtools::json::HttpClient client(_loop);
                cxxtools::RemoteProcedure<bool, bool, bool> boolean(client, "boolean");

                std::ostringstream uri;
                uri << "http://localhost:" << _port << "/rpc";
                client.prepareConnect(cxxtools::net::Uri(uri.str()));
                boolean.begin(true, true);
                CXXTOOLS_UNIT_ASSERT_EQUALS(boolean.end(2000), true);
            }

            // test failing connect in connect
            {
                cxxtools::json::HttpClient client(_loop);
                cxxtools::RemoteProcedure<bool, bool, bool> boolean(client, "boolean");

                CXXTOOLS_UNIT_ASSERT_NOTHROW(client.prepareConnect("", _port + 1, "/rpc"));
                CXXTOOLS_UNIT_ASSERT_THROW(client.connect(), cxxtools::IOError);
            }

            // test failing connect when calling function
            {
                cxxtools::json::HttpClient client(_loop);
                cxxtools::RemoteProcedure<bool, bool, bool> boolean(client, "boolean");
                CXXTOOLS_UNIT_ASSERT_NOTHROW(client.prepareConnect("", _port + 1, "/rpc"));

                boolean.begin(true, true);
                CXXTOOLS_UNIT_ASSERT_THROW(boolean.end(2000), cxxtools::IOError);
            }

        }

        ////////////////////////////////////////////////////////////
        // Connect
        //
        void Connect()
        {
            cxxtools::json::HttpService service;
            service.registerMethod("boolean", *this, &JsonRpcHttpTest::boolean);
            _server->addService("/rpc", service);

            // test connect using cxxtools::net::AddrInfo
            {
                cxxtools::json::HttpClient client(_loop);
                cxxtools::RemoteProcedure<bool, bool, bool> boolean(client, "boolean");

                client.connect(cxxtools::net::AddrInfo("", _port), "/rpc");
                boolean.begin(true, true);
                CXXTOOLS_UNIT_ASSERT_EQUALS(boolean.end(2000), true);
            }

            // test connect using host and port
            {
                cxxtools::json::HttpClient client(_loop);
                cxxtools::RemoteProcedure<bool, bool, bool> boolean(client, "boolean");

                client.connect("", _port, "/rpc");
                boolean.begin(true, true);
                CXXTOOLS_UNIT_ASSERT_EQUALS(boolean.end(2000), true);
            }

            // test connect using uri
            {
                cxxtools::json::HttpClient client(_loop);
                cxxtools::RemoteProcedure<bool, bool, bool> boolean(client, "boolean");

                std::ostringstream uri;
                uri << "http://localhost:" << _port << "/rpc";
                client.connect(cxxtools::net::Uri(uri.str()));
                boolean.begin(true, true);
                CXXTOOLS_UNIT_ASSERT_EQUALS(boolean.end(2000), true);
            }

            // test failing connect
            {
                cxxtools::json::HttpClient client(_loop);
                cxxtools::RemoteProcedure<bool, bool, bool> boolean(client, "boolean");

                CXXTOOLS_UNIT_ASSERT_THROW(client.connect("", _port + 1, "/rpc"), cxxtools::IOError);
            }

        }

        ////////////////////////////////////////////////////////////
        // Multiple calls
        //
        void Multiple()
        {
            cxxtools::json::HttpService service;
            service.registerMethod("multiply", *this, &JsonRpcHttpTest::multiplyDouble);
            _server->addService("/rpc", service);

            typedef cxxtools::RemoteProcedure<double, double, double> Multiply;

            std::vector<cxxtools::json::HttpClient> clients;
            std::vector<Multiply> procs;

            clients.reserve(16);
            procs.reserve(16);

            for (unsigned i = 0; i < 16; ++i)
            {
                clients.push_back(cxxtools::json::HttpClient(_loop, "", _port, "/rpc"));
                procs.push_back(Multiply(clients.back(), "multiply"));
                procs.back().begin(i, i);
            }

            for (unsigned i = 0; i < 16; ++i)
            {
                CXXTOOLS_UNIT_ASSERT_EQUALS(procs[i].end(2000), i*i);
            }

        }

};

cxxtools::unit::RegisterTest<JsonRpcHttpTest> register_JsonRpcHttpTest;
