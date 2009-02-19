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
