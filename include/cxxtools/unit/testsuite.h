/*
 * Copyright (C) 2005-2008 by Dr. Marc Boris Duerner
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
#ifndef CXXTOOLS_UNIT_TESTSUITE_H
#define CXXTOOLS_UNIT_TESTSUITE_H

#include <cxxtools/unit/test.h>
#include <cxxtools/unit/testfixture.h>
#include <cxxtools/unit/testmethod.h>
#include <cxxtools/unit/testprotocol.h>
#include <vector>
#include <utility>

namespace cxxtools {

    class SerializationInfo;

namespace unit {

    /** @brief Protocol and data driven testing
        @ingroup UnitTests

        The TestSuite is used to implement protocol and data driven tests.
        It inherits its ability to register methods and properties from
        %Reflectable. The implementor is supposed to write and register the
        required test methods on construction.

        @code
            class MyTest : public TestSuite
            {
                public:
                    MyTest()
                    : TestSuite("MyTest")
                    {
                        this->registerMethod("test1", *this, &MyTest::test1);
                    }

                    void test1();
            };
        @endcode

        Once the test is written it can be registered to an application by
        using the RegisterTest class template.

        The default protocol will run each registered test method when the
        test is run. Before each test method setUp is called and tearDown
        after each test. The TestProtocol can be replaced with a customised
        one and reflection can be used to call any method multiple times with
        the required data.
    */
    class TestSuite : public Test
                    , public TestFixture
    {
        private:
            class Context : public TestContext
            {
                public:
                    Context(TestFixture& fixture, TestMethod& test, const SerializationInfo* args, unsigned argCount)
                    : TestContext(fixture, test)
                    , _test(test)
                    , _args(args)
                    , _argCount(argCount)
                    {}

                    virtual ~Context()
                    {}

                protected:
                    virtual void exec()
                    {
                        _test.exec(_args, _argCount);
                    }

                private:
                    TestMethod& _test;
                    const SerializationInfo* _args;
                    unsigned _argCount;
            };

        public:
            /** @brief Construct by name and protocol

                Constructs a %TestCase with the passed name and optionally
                a custom protocol. The protocol is not owned by the TestSuite,
                but can be owned by the derived class.

                @param name Name of the test
                @param protocol Protocol for the test.
            */
            explicit TestSuite(const std::string& name, TestProtocol& protocol = TestSuite::defaultProtocol);

            ~TestSuite();

            //! @brief TODO: rename setParameter
            virtual void setParameter(const std::string& name, const cxxtools::SerializationInfo& value);

            /** @brief Sets the protocol.
                @param protocol Protocol for the test
            */
            void setProtocol(TestProtocol* protocol);

            /** \brief Set up conText before running a test.

                This function is called before each registered tester function
                is invoked. It is meant to initialize any required resources.
            */
            virtual void setUp();

            /** \brief Clean up after the test run.

                This function is called after each registered tester function
                is invoked. It is meant to remove any resources previously
                initialized in TestSuite::setUp.
            */
            virtual void tearDown();

            /** @brief Runs the test suite

                The TestProtocol associated with the test will be executed.
                The default protocol will simply call all registered tests.
            */
            virtual void run();

            /** @brief Runs a registered test

                A test method will be called by name and the given arguments
                are passe to it just like when the reflection API is used.
                The method 'setUp' will be called before, and the method
                tearDown after the test. Signals inherited from unit::Test
                are sent appropriatly.

                @param name Name of the method to be run
                @param args Arguments to invoke the method
            */
            void runTest( const std::string& name, const SerializationInfo* args = 0, size_t argCount = 0);

            void runAll();

        protected:
            template <class ParentT>
            void registerMethod(const std::string& name, ParentT& parent, void (ParentT::*method)() )
            {
                cxxtools::unit::TestMethod* test = new BasicTestMethod<ParentT>(this->name() + "::" + name, parent, method);
                this->registerTest(test);
            }

            template <class ParentT, typename A1>
            void registerMethod(const std::string& name, ParentT& parent, void (ParentT::*method)(A1) )
            {
                cxxtools::unit::TestMethod* test = new BasicTestMethod<ParentT, A1>(this->name() + "::" + name, parent, method);
                this->registerTest(test);
            }

            template <class ParentT, typename A1, typename A2>
            void registerMethod(const std::string& name, ParentT& parent, void (ParentT::*method)(A1, A2) )
            {
                cxxtools::unit::TestMethod* test = new BasicTestMethod<ParentT, A1, A2>(this->name() + "::" + name, parent, method);
                this->registerTest(test);
            }

            template <class ParentT, typename A1, typename A2, typename A3>
            void registerMethod(const std::string& name, ParentT& parent, void (ParentT::*method)(A1, A2, A3) )
            {
                cxxtools::unit::TestMethod* test = new BasicTestMethod<ParentT, A1, A2, A3>(this->name() + "::" + name, parent, method);
                this->registerTest(test);
            }

            template <class ParentT, typename A1, typename A2, typename A3, typename A4>
            void registerMethod(const std::string& name, ParentT& parent, void (ParentT::*method)(A1, A2, A3, A4) )
            {
                cxxtools::unit::TestMethod* test = new BasicTestMethod<ParentT, A1, A2, A3, A4>(this->name() + "::" + name, parent, method);
                this->registerTest(test);
            }

            template <class ParentT, typename A1, typename A2, typename A3, typename A4, typename A5>
            void registerMethod(const std::string& name, ParentT& parent, void (ParentT::*method)(A1, A2, A3, A4, A5) )
            {
                cxxtools::unit::TestMethod* test = new BasicTestMethod<ParentT, A1, A2, A3, A4, A5>(this->name() + "::" + name, parent, method);
                this->registerTest(test);
            }

        private:
            void registerTest(TestMethod* test);

            TestMethod* findTest(const std::string& name);

            /** @brief The associated test protocol
            */
            TestProtocol* _protocol;

            typedef std::vector<std::pair<std::string, TestMethod*> > Tests;
            Tests _tests;

        public:
            static TestProtocol defaultProtocol;
    };

} // namespace unit

} // namespace cxxtools

#endif // for header

