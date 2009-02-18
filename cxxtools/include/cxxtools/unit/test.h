/***************************************************************************
 *   Copyright (C) 2005-2008 by Dr. Marc Boris Duerner                     *
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
#ifndef CXXTOOLS_UNIT_TEST_H
#define CXXTOOLS_UNIT_TEST_H

#include <cxxtools/unit/reporter.h>
#include <cxxtools/unit/assertion.h>
#include <cxxtools/connectable.h>
#include <cxxtools/noncopyable.h>
#include <string>

namespace cxxtools {

namespace unit {

    class TestContext;

    /** @brief Test base class
        @ingroup UnitTests

        This is the base class for all types of tests that can be registered
        and run in a test application. It provides a virtual method run that
        is overriden by the derived classes and signals to inform about
        events that occur while the test is run.
    */
    class Test : public Connectable,
                 protected NonCopyable
    {
        public:
            /** @brief Destructor
            */
            virtual ~Test()
            { }

            /** @brief Runs the test

                Derived test classes are supposed to implement this method
                to run the test procedure. A derived class should send the
                'started' signal at the begin of the test and send the
                'finished' signal at the end of the test. If the test was
                successful, the 'success' signal is sent, otheriwse one of the
                signals indicating a failrue. In case of a failed assertion,
                the signal 'assertion' is sent, if a regular std::exception was
                the cause of the error the signal 'exception' is sent and and
                the signal 'error' indicates an unknown exception or error.
                This method should not propagate any exceptions
            */
            virtual void run() = 0;

            const std::string& name() const;

            /** @brief Reports the start of a test
            */
            void reportStart(const TestContext& ctx);

            /** @brief Finished notification

                This signal is sent when the test finished. It does not
                indicate that the test was successful.
            */
            void reportFinish(const TestContext& ctx);

            /** @brief Success notification

                This signal is sent when the test was successful.
            */
            void reportSuccess(const TestContext& ctx);

            /** @brief Assertion notification

                This signal is sent when a assertion failed.
            */
            void reportAssertion(const TestContext& ctx, const Assertion& ass);

            /** @brief Exception notification

                This signal is sent when a regular std::exception occured.
            */
            void reportException(const TestContext& ctx, const std::exception& ex);

            /** @brief Error notification

                This signal is sent when an unknown error occured.
            */
            void reportError(const TestContext& ctx);

            /** @brief Message notification

                This signal can be sent to report informational messages.
            */
            void reportMessage(const std::string& msg);

            void setParent(Test* test);

            Test* parent();

            const Test* parent() const;

            /** @brief Add reporter for test events

                Adds the reporter \a r to report test events. The caller 
                owns the reporter and must make sure it lives as long as 
                the test.
            */
            void attachReporter(Reporter& r);

            void detachReporter(Reporter& r);

        protected:
            /** @brief Construct a test by name
                @param name Name of the test
            */
            explicit Test(const std::string& name)
            : _name(name)
            , _parent(0)
            { }

        private:
            std::string _name;
            Test* _parent;
            std::list<Reporter*> _reporter;
    };

} // namespace unit

} // namespace cxxtools

#endif
