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
#ifndef CXXTOOLS_UNIT_APPLICATION_H
#define CXXTOOLS_UNIT_APPLICATION_H

#include <cxxtools/unit/reporter.h>
#include <cxxtools/unit/test.h>
#include <sstream>

namespace cxxtools {

namespace unit {

    /** @brief Run registered tests

        The application class serves as an environment for a number of tests
        to be run. An application object is usually created in the main loop
        of a program and the return value of Unit::Application::run returned.
        A reporter can be set for the application to process test events.
        Reporters can be made to print information to the console or write
        XML logs. A typical example may look like this:

        @code
            int main()
            {
                Pt::Unit::Reporter reporter;
                Pt::Unit::Application app;
                app.setReporter(reporter);
                return app.run();
            }
        @endcode

        The TestMain.h include already defines a main loop with an application
        for the common use case.
    */
    class Application : public Test
    {
        public:
            /** @brief Default Constructor
            */
            Application();

            /** @brief Destructor
            */
            virtual ~Application();

            static Application& instance();

            /** @brief Find a test by name

                Returns a pointer to the found test or 0 if not found.
            */
            Test* findTest(const std::string& testname);

            /** @brief Add reporter for test events

                Adds the reporter \a r to report test events.
            */
            void attachReporter(Reporter& r);

            /** @brief Add reporter for test events

                Adds the reporter \a r to report test events of the test
                name \a testname.
            */
            void attachReporter(Reporter& r, const std::string& testname);

            /** @brief Run test by name

                This method will run a previously registered test. Use the
                RegisterTest<T> template to register a test to the application.

                @param testName name of the test to be run
            */
            void run(const std::string& testName);

            /** @brief Run all tests

                This method will run all tests that have been registered
                previously. Use the RegisterTest<T> template to register
                a test to the application.
            */
            virtual void run();

            //! @brief Returns the number of errors which occured during a run
            unsigned errors() const
            { return _errors; }

            /** @brief Returns a list of all registered test
                TODO: find another way to query available tests
                @return Reference to the registered tests.
            */
            static std::list<Test*>& tests();

            /** @brief Register a test

                Registers the test \a test to the application. The application
                will not own the test and the caller has to make sure it exists
                as long as the application object. Tests can be deregistered
                by calling %deregisterTest.
            */
            void registerTest(Test& test);

            void deregisterTest(Test& test);

        private:
            static Application* _app;

            /** @brief Number of errors that occured during a run
            */
            unsigned _errors;
    };

} // namespace unit

} // namespace cxxtools

#endif
