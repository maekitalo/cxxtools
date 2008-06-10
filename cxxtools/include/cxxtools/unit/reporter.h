/***************************************************************************
 *   Copyright (C) 2005-2006 by Dr. Marc Boris Duerner                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this program; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef CXXTOOLS_UNIT_REPORTER_H
#define CXXTOOLS_UNIT_REPORTER_H

#include <cxxtools/unit/assertion.h>
#include <cxxtools/unit/testcontext.h>
#include <cxxtools/signals.h>
#include <cxxtools/noncopyable.h>
#include <iosfwd>
#include <stdexcept>

namespace cxxtools {

namespace unit {

/** @brief Test event reporter

    This class is the base class for all reporters for test events. It
    lets the implementor override several virtual methods that are called
    on perticular events during the test. Reporters can be made to print
    information to the console or write XML logs.
*/
class Reporter : protected NonCopyable
{
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
        virtual void reportStart(const TestContext& test) = 0;

        /** @brief Finished notification

            This method is called when a test has finished. Every test sends
            this signal at its end no matter if it failed or succeeded.

            @param test The finished test
        */
        virtual void reportFinish(const TestContext& test) = 0;

        /** @brief Message notification

            This method is called when a test has produced an informational
            message.

            @param msg The message
        */
        virtual void reportMessage(const std::string& msg) = 0;

        /** @brief Success notification

            This method is called when a test was successful.

            @param test The succeeded test
        */
        virtual void reportSuccess(const TestContext& test) = 0;

        /** @brief Assertion notification

            This method is called when a an assertion failed during a test. an
            assertion fails when a user defined condition is not met.

            @param test The failed test
        */
        virtual void reportAssertion(const TestContext& test, const Assertion& a) = 0;

        /** @brief Exception notification

            This method is called when a an exception failed during a test. An
            exception usually means that an error occured that was even u
            nexpected in a test scenario

            @param test The failed test
        */
        virtual void reportException(const TestContext& test, const std::exception& ex) = 0;

        /** @brief Error notification

            This method is called when a an unknown error occurs during a
            test.

            @param test The failed test
        */
        virtual void reportError(const TestContext& test) = 0;

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

    private:
        /** @brief Ostream to print output to
        */
        std::ostream* _out;
};

} // namespace unit

} // namespace cxxtools

#endif
