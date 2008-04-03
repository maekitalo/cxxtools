/***************************************************************************
 *   Copyright (C) 2005-2006 by Dr. Marc Boris Drner                      *
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
#ifndef CXXTOOLS_UNIT_TESTFIXTURE_H
#define CXXTOOLS_UNIT_TESTFIXTURE_H

#include <cxxtools/unit/api.h>

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
