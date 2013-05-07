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
#ifndef CXXTOOLS_UNIT_TESTCASE_H
#define CXXTOOLS_UNIT_TESTCASE_H

#include <cxxtools/unit/test.h>
#include <cxxtools/unit/testfixture.h>
#include <string>

namespace cxxtools {

namespace unit {

    /** @brief Single test with setup and teardown
        @ingroup UnitTests

        A %TestCase can be used for simple tests that require a initialization
        and deinitialization of resources. The implementor is supposed to
        implement the abstract method 'test' and the methods 'setUp' and
        'tearDown' for resource management. When the test is run, 'setUp'
        will be called first, then 'test' and finally 'tearDown'.

        @code
            class MyTest : public TestCase
            {
                public:
                    MyTest()
                    : TestCase("MyTest")
                    {}

                    virtual void setUp()
                    {
                        // init resource
                    }

                    virtual void tearDown()
                    {
                        // release resource
                    }

                    void test()
                    {
                        // test code using a resourc
                    }
            };
        @endcode

        Once the test is written it can be registered to an application by
        using the RegisterTest class template.
    */
    class TestCase : public Test
                   , public TestFixture
    {
        private:
            class Context : public TestContext
            {
                public:
                    Context(TestFixture& fixture, TestCase& test)
                    : TestContext(fixture, test)
                    , _test(test)
                    {}

                protected:
                    virtual void exec()
                    { _test.test(); }

                private:
                    TestCase& _test;
            };

        public:
            /** @brief Construct by name

                Constructs a %TestCase with the passed name.

                @param name Name of the test
            */
            TestCase(const std::string& name);

            /** @brief Runs the test
                When the test is run, 'setUp' will be called first, then
                'test' and finally 'tearDown'. Signals inherited from
                Unit::Test are sent appropriatly.
            */
            virtual void run();

            /** \brief Set up conText before running a test.

                This function is called before each registered tester function
                is invoked. It is meant to initialize any required resources.
            */
            virtual void setUp();

            /** \brief Clean up after the test run.

                This function is called after each registered tester function
                is invoked. It is meant to remove any resources previously
                initialized in TestCase::setUp.
            */
            virtual void tearDown();

        protected:
            /** @brief Implements the actual test procedure
                When the test is run, this method will be called after setUp
                and before tearDown.
            */
            virtual void test() = 0;
    };

} // namespace unit

} // namespace cxxtools

#endif
