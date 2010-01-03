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
#ifndef CXXTOOLS_UNIT_REGISTERTEST_H
#define CXXTOOLS_UNIT_REGISTERTEST_H

#include <cxxtools/api.h>
#include <cxxtools/unit/application.h>


namespace cxxtools {

namespace unit {

    /**
        @param TestT The type of test to register
    */
    template <class TestT>
    /** @brief Registers tests to an application

        Tests can be registered easily with the RegisterTest<> class
        template to an Unit::Application at program initialisation.
        A typical example looks like this:

        @code
            class MyTest : public Unit::TestCase
            { ... };

            RegisterTest<MyTest> _registerMyTest;
        @endcode

        The constructor of the RegisterTest class template will
        register an instance of its template parameter to the
        application.
    */
    struct RegisterTest
    {
        RegisterTest()
        {
            static TestT test;
            Application::tests().push_back(&test);
        }
    };

} // namespace unit

} // namespace cxxtools

#endif
