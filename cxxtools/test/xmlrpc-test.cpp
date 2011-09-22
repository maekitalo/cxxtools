/*
 * Copyright (C) 2005-2006 by Marc Boris Duerner
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
#include "cxxtools/xmlrpc/service.h"
#include "cxxtools/xmlrpc/httpclient.h"
#include "cxxtools/xmlrpc/fault.h"
#include "cxxtools/xmlrpc/remoteprocedure.h"
#include "cxxtools/http/server.h"
#include "cxxtools/eventloop.h"
#include "cxxtools/log.h"
#include <stdlib.h>
#include <sstream>

log_define("cxxtools.test.xmlrpc")


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
    color.red = si.getValue<int>("red");
    color.green = si.getValue<int>("green");
    color.blue = si.getValue<int>("blue");
}


void operator <<=(cxxtools::SerializationInfo& si, const Color& color)
{
    si.addMember("red") <<= color.red;
    si.addMember("green") <<= color.green;
    si.addMember("blue") <<= color.blue;
}


class XmlRpcTest : public cxxtools::unit::TestSuite
{
    private:
        cxxtools::EventLoop* _loop;
        cxxtools::http::Server* _server;
        unsigned _count;
        unsigned short _port;

    public:
        XmlRpcTest()
        : cxxtools::unit::TestSuite("xmlrpc"),
          _port(8001)
        {
            registerMethod("Fault", *this, &XmlRpcTest::Fault);
            registerMethod("Exception", *this, &XmlRpcTest::Exception);
            registerMethod("CallbackException", *this, &XmlRpcTest::CallbackException);
            registerMethod("ConnectError", *this, &XmlRpcTest::ConnectError);
            registerMethod("Nothing", *this, &XmlRpcTest::Nothing);
            registerMethod("Boolean", *this, &XmlRpcTest::Boolean);
            registerMethod("Integer", *this, &XmlRpcTest::Integer);
            registerMethod("Double", *this, &XmlRpcTest::Double);
            registerMethod("String", *this, &XmlRpcTest::String);
            registerMethod("EmptyValues", *this, &XmlRpcTest::EmptyValues);
            registerMethod("Array", *this, &XmlRpcTest::Array);
            registerMethod("EmptyArray", *this, &XmlRpcTest::EmptyArray);
            registerMethod("Struct", *this, &XmlRpcTest::Struct);
            registerMethod("Set", *this, &XmlRpcTest::Set);
            registerMethod("Multiset", *this, &XmlRpcTest::Multiset);
            registerMethod("Map", *this, &XmlRpcTest::Map);
            registerMethod("Multimap", *this, &XmlRpcTest::Multimap);

            char* PORT = getenv("UTEST_PORT");
            if (PORT)
            {
                std::istringstream s;
                s >> _port;
            }
        }

        void failTest()
        {
            throw cxxtools::unit::Assertion("test timed out", CXXTOOLS_SOURCEINFO);
        }

        void setUp()
        {
            _loop = new cxxtools::EventLoop();
            _loop->setIdleTimeout(2000);
            connect(_loop->timeout, *this, &XmlRpcTest::failTest);
            connect(_loop->timeout, *_loop, &cxxtools::EventLoop::exit);

            _server = new cxxtools::http::Server(*_loop, "", _port);
        }

        void tearDown()
        {
            delete _loop;
            delete _server;
        }

        ////////////////////////////////////////////////////////////
        // Fault
        //
        void Fault()
        {
            cxxtools::xmlrpc::Service service;
            service.registerMethod("multiply", *this, &XmlRpcTest::throwFault);
            _server->addService("/calc", service);

            cxxtools::xmlrpc::HttpClient client(*_loop, "", _port, "/calc");
            cxxtools::xmlrpc::RemoteProcedure<bool> multiply(client, "multiply");
            connect( multiply.finished, *this, &XmlRpcTest::onFault );
            multiply.begin();

            _loop->run();
        }

        void onFault(const cxxtools::xmlrpc::Result<bool>& result)
        {
            try
            {
                result.get();
                CXXTOOLS_UNIT_ASSERT_MSG(false, "cxxtools::xmlrpc::Fault exception expected");
            }
            catch (const cxxtools::xmlrpc::Fault& e)
            {
                CXXTOOLS_UNIT_ASSERT_EQUALS(e.rc(), 7);
                CXXTOOLS_UNIT_ASSERT_EQUALS(e.text(), "Fault");
            }

            _loop->exit();
        }

        bool throwFault()
        {
            throw cxxtools::xmlrpc::Fault("Fault", 7);
            return false;
        }

        ////////////////////////////////////////////////////////////
        // Exception
        //
        void Exception()
        {
            cxxtools::xmlrpc::Service service;
            service.registerMethod("multiply", *this, &XmlRpcTest::throwException);
            _server->addService("/calc", service);

            cxxtools::xmlrpc::HttpClient client(*_loop, "", _port, "/calc");
            cxxtools::xmlrpc::RemoteProcedure<bool> multiply(client, "multiply");
            connect( multiply.finished, *this, &XmlRpcTest::onException );
            multiply.begin();

            _loop->run();
        }

        void onException(const cxxtools::xmlrpc::Result<bool>& result)
        {
            try
            {
                bool v = result.get();
                CXXTOOLS_UNIT_ASSERT(false);
            }
            catch (const cxxtools::xmlrpc::Fault& e)
            {
                CXXTOOLS_UNIT_ASSERT_EQUALS(e.rc(), 0);
                CXXTOOLS_UNIT_ASSERT_EQUALS(e.text(), "Exception");
            }

            _loop->exit();
        }

        bool throwException()
        {
            throw std::runtime_error("Exception");
            return false;
        }

        ////////////////////////////////////////////////////////////
        // Nothing
        //
        void Nothing()
        {
            cxxtools::xmlrpc::Service service;
            service.registerMethod("multiply", *this, &XmlRpcTest::multiplyNothing);
            _server->addService("/calc", service);

            cxxtools::xmlrpc::HttpClient client(*_loop, "", _port, "/calc");
            cxxtools::xmlrpc::RemoteProcedure<bool> multiply(client, "multiply");
            connect( multiply.finished, *this, &XmlRpcTest::onNothingFinished );

            multiply.begin();

            _loop->run();
        }

        void onNothingFinished(const cxxtools::xmlrpc::Result<bool>& r)
        {
            CXXTOOLS_UNIT_ASSERT_EQUALS(r.get(), false);

            _loop->exit();
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
            cxxtools::xmlrpc::Service service;
            service.registerMethod("multiply", *this, &XmlRpcTest::multiplyNothing);
            _server->addService("/calc", service);

            cxxtools::xmlrpc::HttpClient client(*_loop, "", _port, "/calc");
            cxxtools::xmlrpc::RemoteProcedure<bool> multiply(client, "multiply");
            connect( multiply.finished, *this, &XmlRpcTest::onExceptionCallback );

            multiply.begin();

            _count = 0;
            CXXTOOLS_UNIT_ASSERT_THROW(_loop->run(), std::runtime_error);
            CXXTOOLS_UNIT_ASSERT_EQUALS(_count, 1);
        }

        void onExceptionCallback(const cxxtools::xmlrpc::Result<bool>& r)
        {
            log_warn("exception callback");
            ++_count;
            _loop->exit();
            throw std::runtime_error("my error");
        }

        ////////////////////////////////////////////////////////////
        // ConnectError
        //
        void ConnectError()
        {
            log_trace("ConnectError");

            cxxtools::xmlrpc::HttpClient client(*_loop, "", _port, "/calc");
            cxxtools::xmlrpc::RemoteProcedure<bool> multiply(client, "multiply");
            connect( multiply.finished, *this, &XmlRpcTest::onConnectErrorCallback );

            multiply.begin();

            try
            {
                _loop->run();
            }
            catch (const std::exception& e)
            {
                log_error("loop exited with exception: " << e.what());
                CXXTOOLS_UNIT_ASSERT_MSG(false, std::string("unexpected exception ") + typeid(e).name() + ": " + e.what());
            }
        }

        void onConnectErrorCallback(const cxxtools::xmlrpc::Result<bool>& r)
        {
            log_debug("onConnectErrorCallback");
            _loop->exit();
            CXXTOOLS_UNIT_ASSERT_THROW(r.get(), std::exception);
        }

        ////////////////////////////////////////////////////////////
        // Boolean
        //
        void Boolean()
        {
            cxxtools::xmlrpc::Service service;
            service.registerMethod("multiply", *this, &XmlRpcTest::multiplyBoolean);
            _server->addService("/calc", service);

            cxxtools::xmlrpc::HttpClient client(*_loop, "", _port, "/calc");
            cxxtools::xmlrpc::RemoteProcedure<bool, bool, bool> multiply(client, "multiply");
            connect( multiply.finished, *this, &XmlRpcTest::onBooleanFinished );

            multiply.begin(true, true);

            _loop->run();
        }

        void onBooleanFinished(const cxxtools::xmlrpc::Result<bool>& r)
        {
            CXXTOOLS_UNIT_ASSERT_EQUALS(r.get(), true);

            _loop->exit();
        }

        bool multiplyBoolean(bool a, bool b)
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
            cxxtools::xmlrpc::Service service;
            service.registerMethod("multiply", *this, &XmlRpcTest::multiplyInt);
            _server->addService("/calc", service);

            cxxtools::xmlrpc::HttpClient client(*_loop, "", _port, "/calc");
            cxxtools::xmlrpc::RemoteProcedure<int, int, int> multiply(client, "multiply");
            connect( multiply.finished, *this, &XmlRpcTest::onIntegerFinished );

            multiply.begin(2, 3);

            _loop->run();
        }

        void onIntegerFinished(const cxxtools::xmlrpc::Result<int>& r)
        {
            CXXTOOLS_UNIT_ASSERT_EQUALS(r.get(), 6);

            _loop->exit();
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
            cxxtools::xmlrpc::Service service;
            service.registerMethod("multiply", *this, &XmlRpcTest::multiplyDouble);
            _server->addService("/calc", service);

            cxxtools::xmlrpc::HttpClient client(*_loop, "", _port, "/calc");
            cxxtools::xmlrpc::RemoteProcedure<double, double, double> multiply(client, "multiply");
            connect( multiply.finished, *this, &XmlRpcTest::onDoubleFinished );

            multiply.begin(2.0, 3.0);

            _loop->run();
        }

        void onDoubleFinished(const cxxtools::xmlrpc::Result<double>& r)
        {
            CXXTOOLS_UNIT_ASSERT_EQUALS(r.get(), 6.0);

            _loop->exit();
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
            cxxtools::xmlrpc::Service service;
            service.registerMethod("echoString", *this, &XmlRpcTest::echoString);
            _server->addService("/foo", service);

            cxxtools::xmlrpc::HttpClient client(*_loop, "", _port, "/foo");
            cxxtools::xmlrpc::RemoteProcedure<std::string, std::string> echo(client, "echoString");
            connect( echo.finished, *this, &XmlRpcTest::onStringEchoFinished );

            echo.begin("\xc3\xaf\xc2\xbb\xc2\xbf'\"&<> foo?");

            _loop->run();
        }

        void onStringEchoFinished(const cxxtools::xmlrpc::Result<std::string>& r)
        {
            CXXTOOLS_UNIT_ASSERT_EQUALS(r.get(), "\xc3\xaf\xc2\xbb\xc2\xbf'\"&<> foo?");

            _loop->exit();
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
            cxxtools::xmlrpc::Service service;
            service.registerMethod("multiply", *this, &XmlRpcTest::multiplyEmpty);
            _server->addService("/calc", service);

            cxxtools::xmlrpc::HttpClient client(*_loop, "", _port, "/calc");
            cxxtools::xmlrpc::RemoteProcedure<std::string, std::string, std::string> multiply(client, "multiply");
            connect( multiply.finished, *this, &XmlRpcTest::onEmptyFinished );

            multiply.begin("", "");

            _loop->run();
        }

        void onEmptyFinished(const cxxtools::xmlrpc::Result<std::string>& r)
        {
            CXXTOOLS_UNIT_ASSERT_EQUALS(r.get(), "4");
            _loop->exit();
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
            cxxtools::xmlrpc::Service service;
            service.registerMethod("multiply", *this, &XmlRpcTest::multiplyVector);
            _server->addService("/calc", service);

            cxxtools::xmlrpc::HttpClient client(*_loop, "", _port, "/calc");
            cxxtools::xmlrpc::RemoteProcedure< std::vector<int>, std::vector<int>, std::vector<int> > multiply(client, "multiply");
            connect( multiply.finished, *this, &XmlRpcTest::onArrayFinished );

            std::vector<int> vec;
            vec.push_back(10);
            vec.push_back(20);

            multiply.begin(vec, vec);

            _loop->run();
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

        void onArrayFinished(const cxxtools::xmlrpc::Result<std::vector<int> >& r)
        {
            CXXTOOLS_UNIT_ASSERT_EQUALS(r.get().size(), 2);
            CXXTOOLS_UNIT_ASSERT_EQUALS(r.get().at(0), 100);
            CXXTOOLS_UNIT_ASSERT_EQUALS(r.get().at(1), 400);

            _loop->exit();
        }

        ////////////////////////////////////////////////////////////
        // EmptyArray
        //
        void EmptyArray()
        {
            cxxtools::xmlrpc::Service service;
            service.registerMethod("multiply", *this, &XmlRpcTest::multiplyVector);
            _server->addService("/calc", service);

            cxxtools::xmlrpc::HttpClient client(*_loop, "", _port, "/calc");
            cxxtools::xmlrpc::RemoteProcedure< std::vector<int>, std::vector<int>, std::vector<int> > multiply(client, "multiply");
            connect( multiply.finished, *this, &XmlRpcTest::onEmptyArrayFinished );

            std::vector<int> vec;
            multiply.begin(vec, vec);

            _loop->run();
        }

        void onEmptyArrayFinished(const cxxtools::xmlrpc::Result<std::vector<int> >& r)
        {
            CXXTOOLS_UNIT_ASSERT_EQUALS(r.get().size(), 0);

            _loop->exit();
        }

        ////////////////////////////////////////////////////////////
        // Struct
        //
        void Struct()
        {
            cxxtools::xmlrpc::Service service;
            service.registerMethod("multiply", *this, &XmlRpcTest::multiplyColor);
            _server->addService("/calc", service);

            cxxtools::xmlrpc::HttpClient client(*_loop, "", _port, "/calc");
            cxxtools::xmlrpc::RemoteProcedure< Color, Color, Color > multiply(client, "multiply");
            connect( multiply.finished, *this, &XmlRpcTest::onStuctFinished );

            Color a;
            a.red = 2;
            a.green = 3;
            a.blue = 4;

            Color b;
            b.red = 3;
            b.green = 4;
            b.blue = 5;

            multiply.begin(a, b);

            _loop->run();
        }

        void onStuctFinished(const cxxtools::xmlrpc::Result<Color>& color)
        {
            CXXTOOLS_UNIT_ASSERT_EQUALS(color.get().red, 6);
            CXXTOOLS_UNIT_ASSERT_EQUALS(color.get().green, 12);
            CXXTOOLS_UNIT_ASSERT_EQUALS(color.get().blue, 20);

            _loop->exit();
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
            cxxtools::xmlrpc::Service service;
            service.registerMethod("multiplyset", *this, &XmlRpcTest::multiplySet);
            _server->addService("/test", service);

            cxxtools::xmlrpc::HttpClient client(*_loop, "", _port, "/test");
            cxxtools::xmlrpc::RemoteProcedure<IntSet, IntSet, int> multiply(client, "multiplyset");
            connect( multiply.finished, *this, &XmlRpcTest::onSetFinished );

            IntSet myset;
            myset.insert(4);
            myset.insert(5);
            myset.insert(11);
            myset.insert(5);

            multiply.begin(myset, 2);

            _loop->run();
        }

        void onSetFinished(const cxxtools::xmlrpc::Result<IntSet>& result)
        {
            const IntSet& v = result.get();
            CXXTOOLS_UNIT_ASSERT_EQUALS(v.size(), 3);
            CXXTOOLS_UNIT_ASSERT(v.find(8) != v.end());
            CXXTOOLS_UNIT_ASSERT(v.find(10) != v.end());
            CXXTOOLS_UNIT_ASSERT(v.find(22) != v.end());

            _loop->exit();
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
            cxxtools::xmlrpc::Service service;
            service.registerMethod("multiplyset", *this, &XmlRpcTest::multiplyMultiset);
            _server->addService("/test", service);

            cxxtools::xmlrpc::HttpClient client(*_loop, "", _port, "/test");
            cxxtools::xmlrpc::RemoteProcedure<IntMultiset, IntMultiset, int> multiply(client, "multiplyset");
            connect( multiply.finished, *this, &XmlRpcTest::onMultisetFinished );

            IntMultiset myset;
            myset.insert(4);
            myset.insert(5);
            myset.insert(11);
            myset.insert(5);

            multiply.begin(myset, 2);

            _loop->run();
        }

        void onMultisetFinished(const cxxtools::xmlrpc::Result<IntMultiset>& result)
        {
            const IntMultiset& v = result.get();
            CXXTOOLS_UNIT_ASSERT_EQUALS(v.size(), 4);
            CXXTOOLS_UNIT_ASSERT_EQUALS(v.count(8), 1);
            CXXTOOLS_UNIT_ASSERT_EQUALS(v.count(10), 2);
            CXXTOOLS_UNIT_ASSERT_EQUALS(v.count(22), 1);

            _loop->exit();
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
            cxxtools::xmlrpc::Service service;
            service.registerMethod("multiplymap", *this, &XmlRpcTest::multiplyMap);

            _server->addService("/test", service);

            cxxtools::xmlrpc::HttpClient client(*_loop, "", _port, "/test");
            cxxtools::xmlrpc::RemoteProcedure<IntMap, IntMap, int> multiply(client, "multiplymap");
            connect( multiply.finished, *this, &XmlRpcTest::onMultiplyMapFinished );

            IntMap mymap;
            mymap[2] = 4;
            mymap[7] = 7;
            mymap[1] = -1;

            multiply.begin(mymap, 2);

            _loop->run();
        }

        void onMultiplyMapFinished(const cxxtools::xmlrpc::Result<IntMap>& result)
        {
            const IntMap& v = result.get();
            CXXTOOLS_UNIT_ASSERT_EQUALS(v.size(), 3);
            CXXTOOLS_UNIT_ASSERT(v.find(2) != v.end());
            CXXTOOLS_UNIT_ASSERT_EQUALS(v.find(2)->second, 8);
            CXXTOOLS_UNIT_ASSERT(v.find(7) != v.end());
            CXXTOOLS_UNIT_ASSERT_EQUALS(v.find(7)->second, 14);
            CXXTOOLS_UNIT_ASSERT(v.find(1) != v.end());
            CXXTOOLS_UNIT_ASSERT_EQUALS(v.find(1)->second, -2);

            _loop->exit();
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
            cxxtools::xmlrpc::Service service;
            service.registerMethod("multiplymultimap", *this, &XmlRpcTest::multiplyMultimap);

            _server->addService("/test", service);

            cxxtools::xmlrpc::HttpClient client(*_loop, "", _port, "/test");
            cxxtools::xmlrpc::RemoteProcedure<IntMultimap, IntMultimap, int> multiply(client, "multiplymultimap");
            connect( multiply.finished, *this, &XmlRpcTest::onMultiplyMultimapFinished );

            IntMultimap mymap;
            mymap.insert(IntMultimap::value_type(2, 4));
            mymap.insert(IntMultimap::value_type(7, 7));
            mymap.insert(IntMultimap::value_type(7, 8));
            mymap.insert(IntMultimap::value_type(1, -1));

            multiply.begin(mymap, 2);

            _loop->run();
        }

        void onMultiplyMultimapFinished(const cxxtools::xmlrpc::Result<IntMultimap>& result)
        {
            const IntMultimap& v = result.get();
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

            _loop->exit();
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

};

cxxtools::unit::RegisterTest<XmlRpcTest> register_XmlRpcTest;
