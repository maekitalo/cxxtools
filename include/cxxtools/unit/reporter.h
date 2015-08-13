/*
 * Copyright (C) 2005-2006 by Dr. Marc Boris Duerner
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
#ifndef CXXTOOLS_UNIT_REPORTER_H
#define CXXTOOLS_UNIT_REPORTER_H

#include <cxxtools/unit/assertion.h>
#include <cxxtools/unit/testcontext.h>
#include <cxxtools/signal.h>
#include <iosfwd>
#include <stdexcept>

namespace cxxtools
{

namespace unit
{

/** @brief Test event reporter

    This class is the base class for all reporters for test events. It
    lets the implementor override several virtual methods that are called
    on perticular events during the test. Reporters can be made to print
    information to the console or write XML logs.
*/
class Reporter
{
        Reporter(const Reporter&) { }
        Reporter& operator=(const Reporter&) { return *this; }

    public:
        /** @brief Destructor
        */
        virtual ~Reporter()
        { destroyed.send(*this);}

        /** @brief Start notification

            This method is called when a test has started. Every test sends
            this signal at startup.

            @param test The started test
        */
        virtual void reportStart(const TestContext& test)
        { }

        /** @brief Finished notification

            This method is called when a test has finished. Every test sends
            this signal at its end no matter if it failed or succeeded.

            @param test The finished test
        */
        virtual void reportFinish(const TestContext& test)
        { }

        /** @brief Message notification

            This method is called when a test has produced an informational
            message.

            @param msg The message
        */
        virtual void reportMessage(const std::string& msg)
        { }

        /** @brief Success notification

            This method is called when a test was successful.

            @param test The succeeded test
        */
        virtual void reportSuccess(const TestContext& test)
        { }

        /** @brief Assertion notification

            This method is called when a an assertion failed during a test. an
            assertion fails when a user defined condition is not met.

            @param test The failed test
        */
        virtual void reportAssertion(const TestContext& test, const Assertion& a)
        { }

        /** @brief Exception notification

            This method is called when a an exception failed during a test. An
            exception usually means that an error occured that was even u
            nexpected in a test scenario

            @param test The failed test
        */
        virtual void reportException(const TestContext& test, const std::exception& ex)
        { }

        /** @brief Error notification

            This method is called when a an unknown error occurs during a
            test.

            @param test The failed test
        */
        virtual void reportError(const TestContext& test)
        { }

        /** @brief Skipped notification

            This method is called when was skipped.

            @param test The skipped test
        */
        virtual void reportSkip(const TestContext& test)
        { }

        Signal<Reporter&> destroyed;

    protected:
        /** @brief Constructs a reporter
        */
        Reporter()
        {}
};


class BriefReporter : public Reporter
{
    public:
        explicit BriefReporter(std::ostream* out = &std::cout);

        virtual ~BriefReporter();

        void setOutput(std::ostream& out);

        virtual void reportStart(const TestContext& test);

        virtual void reportFinish(const TestContext& test);

        virtual void reportMessage(const std::string& msg);

        virtual void reportSuccess(const TestContext& test);

        virtual void reportAssertion(const TestContext& test, const Assertion& a);

        virtual void reportException(const TestContext& test, const std::exception& ex);

        virtual void reportError(const TestContext& test);

        virtual void reportSkip(const TestContext& test);

    private:
        /** @brief Ostream to print output to
        */
        std::ostream* _out;
};

} // namespace unit

} // namespace cxxtools

#endif
