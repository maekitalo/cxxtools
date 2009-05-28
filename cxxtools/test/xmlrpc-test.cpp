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
#include "cxxtools/unit/testmain.h"
#include "cxxtools/xmlrpc/service.h"
#include "cxxtools/xmlrpc/client.h"
#include "cxxtools/xmlrpc/fault.h"
#include "cxxtools/xmlrpc/remoteprocedure.h"
#include "cxxtools/http/server.h"
#include "cxxtools/eventloop.h"
#include "cxxtools/thread.h"
#include "cxxtools/log.h"

log_define("cxxtools.test.xmlrpc")


struct Color
{
    int red;
    int green;
    int blue;
};


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
        cxxtools::AttachedThread* _serverThread;

    public:
        XmlRpcTest()
        : cxxtools::unit::TestSuite("cxxtools-xmlrpc-Test")
        {
            this->registerMethod("Fault", *this, &XmlRpcTest::Fault);
            this->registerMethod("Nothing", *this, &XmlRpcTest::Nothing);
            this->registerMethod("Boolean", *this, &XmlRpcTest::Boolean);
            this->registerMethod("Integer", *this, &XmlRpcTest::Integer);
            this->registerMethod("Double", *this, &XmlRpcTest::Double);
            this->registerMethod("String", *this, &XmlRpcTest::String);
            this->registerMethod("EchoString", *this, &XmlRpcTest::EchoString);
            this->registerMethod("EmptyValues", *this, &XmlRpcTest::EmptyValues);
            this->registerMethod("Array", *this, &XmlRpcTest::Array);
            this->registerMethod("EmptyArray", *this, &XmlRpcTest::EmptyArray);
            this->registerMethod("Struct", *this, &XmlRpcTest::Struct);
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

            _server = new cxxtools::http::Server("127.0.0.1", 8001);
            _serverThread = new cxxtools::AttachedThread( cxxtools::callable(*_server, &cxxtools::http::Server::run) );
        }

        void tearDown()
        {
            delete _loop;
            delete _server;
            delete _serverThread;
        }

        void Fault()
        {
            cxxtools::xmlrpc::Service service;
            service.registerMethod("multiply", *this, &XmlRpcTest::throwFault);
            _server->addService("/calc", service);

            _serverThread->start();
            cxxtools::Thread::sleep(500);

            cxxtools::xmlrpc::Client client(*_loop, "127.0.0.1", 8001, "/calc");
            cxxtools::xmlrpc::RemoteProcedure<bool> multiply(client, "multiply");
            connect( multiply.finished, *this, &XmlRpcTest::onFault );
            multiply.begin();

            _loop->run();
        }

        void onFault(const cxxtools::xmlrpc::Result<bool>& result)
        {
            try
            {
                bool v = result.get();
                CXXTOOLS_UNIT_ASSERT(false);
            }
            catch (const cxxtools::xmlrpc::Fault& e)
            {
                CXXTOOLS_UNIT_ASSERT_EQUALS(e.rc(), 7)
                CXXTOOLS_UNIT_ASSERT_EQUALS(e.text(), "Fault")
            }

            _loop->exit();
        }

        void Nothing()
        {
            cxxtools::xmlrpc::Service service;
            service.registerMethod("multiply", *this, &XmlRpcTest::multiplyNothing);
            _server->addService("/calc", service);

            _serverThread->start();
            cxxtools::Thread::sleep(500);

            cxxtools::xmlrpc::Client client(*_loop, "127.0.0.1", 8001, "/calc");
            cxxtools::xmlrpc::RemoteProcedure<bool> multiply(client, "multiply");
            connect( multiply.finished, *this, &XmlRpcTest::onNothingFinished );

            multiply.begin();

            _loop->run();
        }

        void onNothingFinished(const cxxtools::xmlrpc::Result<bool>& r)
        {
            CXXTOOLS_UNIT_ASSERT_EQUALS(r.get(), false)

            _loop->exit();
        }

        void Boolean()
        {
            cxxtools::xmlrpc::Service service;
            service.registerMethod("multiply", *this, &XmlRpcTest::multiplyBoolean);
            _server->addService("/calc", service);

            _serverThread->start();
            cxxtools::Thread::sleep(500);

            cxxtools::xmlrpc::Client client(*_loop, "127.0.0.1", 8001, "/calc");
            cxxtools::xmlrpc::RemoteProcedure<bool, bool, bool> multiply(client, "multiply");
            connect( multiply.finished, *this, &XmlRpcTest::onBooleanFinished );

            multiply.begin(true, true);

            _loop->run();
        }

        void onBooleanFinished(const cxxtools::xmlrpc::Result<bool>& r)
        {
            CXXTOOLS_UNIT_ASSERT_EQUALS(r.get(), true)

            _loop->exit();
        }

        void Integer()
        {
            cxxtools::xmlrpc::Service service;
            service.registerMethod("multiply", *this, &XmlRpcTest::multiplyInt);
            _server->addService("/calc", service);

            _serverThread->start();
            cxxtools::Thread::sleep(500);

            cxxtools::xmlrpc::Client client(*_loop, "127.0.0.1", 8001, "/calc");
            cxxtools::xmlrpc::RemoteProcedure<int, int, int> multiply(client, "multiply");
            connect( multiply.finished, *this, &XmlRpcTest::onIntegerFinished );

            multiply.begin(2, 3);

            _loop->run();
        }

        void onIntegerFinished(const cxxtools::xmlrpc::Result<int>& r)
        {
            CXXTOOLS_UNIT_ASSERT_EQUALS(r.get(), 6)

            _loop->exit();
        }

        void Double()
        {
            cxxtools::xmlrpc::Service service;
            service.registerMethod("multiply", *this, &XmlRpcTest::multiplyDouble);
            _server->addService("/calc", service);

            _serverThread->start();
            cxxtools::Thread::sleep(500);

            cxxtools::xmlrpc::Client client(*_loop, "127.0.0.1", 8001, "/calc");
            cxxtools::xmlrpc::RemoteProcedure<double, double, double> multiply(client, "multiply");
            connect( multiply.finished, *this, &XmlRpcTest::onDoubleFinished );

            multiply.begin(2.0, 3.0);

            _loop->run();
        }

        void onDoubleFinished(const cxxtools::xmlrpc::Result<double>& r)
        {
            CXXTOOLS_UNIT_ASSERT_EQUALS(r.get(), 6.0)

            _loop->exit();
        }

        void String()
        {
            cxxtools::xmlrpc::Service service;
            service.registerMethod("multiply", *this, &XmlRpcTest::multiplyString);
            _server->addService("/calc", service);

            _serverThread->start();
            cxxtools::Thread::sleep(500);

            cxxtools::xmlrpc::Client client(*_loop, "127.0.0.1", 8001, "/calc");
            cxxtools::xmlrpc::RemoteProcedure<std::string, std::string, std::string> multiply(client, "multiply");
            connect( multiply.finished, *this, &XmlRpcTest::onStringFinished );

            multiply.begin("2", "3");

            _loop->run();
        }

        void onStringFinished(const cxxtools::xmlrpc::Result<std::string>& r)
        {
            CXXTOOLS_UNIT_ASSERT_EQUALS(r.get(), "6")

            _loop->exit();
        }

        void EchoString()
        {
            cxxtools::xmlrpc::Service service;
            service.registerMethod("echoString", *this, &XmlRpcTest::echoString);
            _server->addService("/foo", service);

            _serverThread->start();
            cxxtools::Thread::sleep(500);

            cxxtools::xmlrpc::Client client(*_loop, "127.0.0.1", 8001, "/foo");
            cxxtools::xmlrpc::RemoteProcedure<std::string, std::string> echo(client, "echoString");
            connect( echo.finished, *this, &XmlRpcTest::onStringEchoFinished );

            echo.begin("foo?");

            _loop->run();
        }

        void onStringEchoFinished(const cxxtools::xmlrpc::Result<std::string>& r)
        {
            CXXTOOLS_UNIT_ASSERT_EQUALS(r.get(), "foo?")

            _loop->exit();
        }

        void EmptyValues()
        {
            cxxtools::xmlrpc::Service service;
            service.registerMethod("multiply", *this, &XmlRpcTest::multiplyEmpty);
            _server->addService("/calc", service);

            _serverThread->start();
            cxxtools::Thread::sleep(500);

            cxxtools::xmlrpc::Client client(*_loop, "127.0.0.1", 8001, "/calc");
            cxxtools::xmlrpc::RemoteProcedure<std::string, std::string, std::string> multiply(client, "multiply");
            connect( multiply.finished, *this, &XmlRpcTest::onEmptyFinished );

            multiply.begin("", "");

            _loop->run();
        }

        void onEmptyFinished(const cxxtools::xmlrpc::Result<std::string>& r)
        {
            CXXTOOLS_UNIT_ASSERT_EQUALS(r.get(), "4")
            _loop->exit();
        }

        void Array()
        {
            cxxtools::xmlrpc::Service service;
            service.registerMethod("multiply", *this, &XmlRpcTest::multiplyVector);
            _server->addService("/calc", service);

            _serverThread->start();
            cxxtools::Thread::sleep(500);

            cxxtools::xmlrpc::Client client(*_loop, "127.0.0.1", 8001, "/calc");
            cxxtools::xmlrpc::RemoteProcedure< std::vector<int>, std::vector<int>, std::vector<int> > multiply(client, "multiply");
            connect( multiply.finished, *this, &XmlRpcTest::onArrayFinished );

            std::vector<int> vec;
            vec.push_back(10);
            vec.push_back(20);

            multiply.begin(vec, vec);

            _loop->run();
        }

        void onArrayFinished(const cxxtools::xmlrpc::Result<std::vector<int> >& r)
        {
            CXXTOOLS_UNIT_ASSERT_EQUALS(r.get().size(), 2)
            CXXTOOLS_UNIT_ASSERT_EQUALS(r.get().at(0), 100)
            CXXTOOLS_UNIT_ASSERT_EQUALS(r.get().at(1), 400)

            _loop->exit();
        }

        void EmptyArray()
        {
            cxxtools::xmlrpc::Service service;
            service.registerMethod("multiply", *this, &XmlRpcTest::multiplyVector);
            _server->addService("/calc", service);

            _serverThread->start();
            cxxtools::Thread::sleep(500);

            cxxtools::xmlrpc::Client client(*_loop, "127.0.0.1", 8001, "/calc");
            cxxtools::xmlrpc::RemoteProcedure< std::vector<int>, std::vector<int>, std::vector<int> > multiply(client, "multiply");
            connect( multiply.finished, *this, &XmlRpcTest::onEmptyArrayFinished );

            std::vector<int> vec;
            multiply.begin(vec, vec);

            _loop->run();
        }

        void onEmptyArrayFinished(const cxxtools::xmlrpc::Result<std::vector<int> >& r)
        {
            CXXTOOLS_UNIT_ASSERT_EQUALS(r.get().size(), 0)

            _loop->exit();
        }

        void Struct()
        {
            cxxtools::xmlrpc::Service service;
            service.registerMethod("multiply", *this, &XmlRpcTest::multiplyColor);
            _server->addService("/calc", service);

            _serverThread->start();
            cxxtools::Thread::sleep(500);

            cxxtools::xmlrpc::Client client(*_loop, "127.0.0.1", 8001, "/calc");
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
            CXXTOOLS_UNIT_ASSERT_EQUALS(color.get().red, 6)
            CXXTOOLS_UNIT_ASSERT_EQUALS(color.get().green, 12)
            CXXTOOLS_UNIT_ASSERT_EQUALS(color.get().blue, 20)

            _loop->exit();
        }

        bool throwFault()
        {
            throw cxxtools::xmlrpc::Fault("Fault", 7);
            return false;
        }

        bool multiplyNothing()
        {
            return false;
        }

        bool multiplyBoolean(bool a, bool b)
        {
            CXXTOOLS_UNIT_ASSERT_EQUALS(a, true)
            CXXTOOLS_UNIT_ASSERT_EQUALS(b, true)
            return true;
        }

        int multiplyInt(int a, int b)
        {
            return a*b;
        }

        double multiplyDouble(double a, double b)
        {
            return a*b;
        }

        std::string multiplyString(std::string a, std::string b)
        {
            CXXTOOLS_UNIT_ASSERT_EQUALS(a, "2")
            CXXTOOLS_UNIT_ASSERT_EQUALS(b, "3")
            return "6";
        }

        std::string echoString(std::string a)
        {
            return a;
        }

        std::string multiplyEmpty(std::string a, std::string b)
        {
            CXXTOOLS_UNIT_ASSERT_EQUALS(a, "")
            CXXTOOLS_UNIT_ASSERT_EQUALS(b, "")
            return "4";
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

        Color multiplyColor(const Color& a, const Color& b)
        {
            Color color;
            color.red = a.red * b.red;
            color.green = a.green * b.green;
            color.blue = a.blue * b.blue;
            return color;
        }
};

cxxtools::unit::RegisterTest<XmlRpcTest> register_XmlRpcTest;
