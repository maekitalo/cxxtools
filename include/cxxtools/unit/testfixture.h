/*
 * Copyright (C) 2005-2006 by Dr. Marc Boris Drner
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
#ifndef CXXTOOLS_UNIT_TESTFIXTURE_H
#define CXXTOOLS_UNIT_TESTFIXTURE_H

namespace cxxtools {

namespace unit {

class TestFixture
{
    public:
        virtual ~TestFixture()
        {}

        /** \brief Set up conText before running a test.

            This function is called before each registered tester function
            is invoked. It is meant to initialize any required resources.
        */
        virtual void setUp()
        {}

        /** \brief Clean up after the test run.

            This function is called after each registered tester function
            is invoked. It is meant to remove any resources previously
            initialized in TestCase::setUp.
        */
        virtual void tearDown()
        {}
};

} // namespace init

} // namespace cxxtools

#endif
