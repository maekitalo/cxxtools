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
#ifndef CXXTOOLS_UNIT_TESTPROTOCOL_H
#define CXXTOOLS_UNIT_TESTPROTOCOL_H

#include <cxxtools/unit/test.h>
#include <cxxtools/unit/assertion.h>

namespace cxxtools {

namespace unit {

    class TestSuite;

    /** @brief Protocol for test suites
        @ingroup UnitTests

        This is the base class for protocols that can be used to run a test
        suite. The default implementation will simply run each registered
        test of the test suite without passing it any data. Implementors
        need to override the method TestProtocol::run.
    */
    class TestProtocol
    {
        public:
            /** @brief Destructor
            */
            virtual ~TestProtocol()
            {}

            /** @brief Executes the protocol

                This method can be overriden to specify a custom protocol
                for a test suite. The default implementation will simply
                call each registered method of the test suite. Implementors
                will most likely call TestSuite::runTest to resolve a test
                method by name and pass it required arguments.

                @param test The test suite to apply the protocol
            */
            virtual void run(TestSuite& test);
    };

} // namespace onit

} // namespace cxxtools

#endif // for header

