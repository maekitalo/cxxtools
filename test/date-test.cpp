/*
 * Copyright (C) 2013 Tommi Maekitalo
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

#include <iostream>
#include "cxxtools/date.h"
#include "cxxtools/unit/testsuite.h"
#include "cxxtools/unit/registertest.h"

class DateTest : public cxxtools::unit::TestSuite
{
    public:
        DateTest()
        : cxxtools::unit::TestSuite("date")
        {
            registerMethod("fromString", *this, &DateTest::fromString);
            registerMethod("toString", *this, &DateTest::toString);
        }

        void fromString()
        {
            int year;
            unsigned month, day;

            cxxtools::Date dt;

            dt = cxxtools::Date("2013-05-03");

            dt.get(year, month, day);
            CXXTOOLS_UNIT_ASSERT_EQUALS(year, 2013);
            CXXTOOLS_UNIT_ASSERT_EQUALS(month, 5);
            CXXTOOLS_UNIT_ASSERT_EQUALS(day, 3);

            dt = cxxtools::Date("2013 5 3", "%Y %m %d");

            dt.get(year, month, day);
            CXXTOOLS_UNIT_ASSERT_EQUALS(year, 2013);
            CXXTOOLS_UNIT_ASSERT_EQUALS(month, 5);
            CXXTOOLS_UNIT_ASSERT_EQUALS(day, 3);

        }

        void toString()
        {
          cxxtools::Date dt(2013, 5, 3);

          std::string str = dt.toString();
          CXXTOOLS_UNIT_ASSERT_EQUALS(str, "2013-05-03");

          str = dt.toString("%Y %m %d");
          CXXTOOLS_UNIT_ASSERT_EQUALS(str, "2013 05 03");

          str = dt.toString("%w %W");
          CXXTOOLS_UNIT_ASSERT_EQUALS(str, "5 5");

          dt = cxxtools::Date(2013, 5, 5);
          str = dt.toString("%w %W");
          CXXTOOLS_UNIT_ASSERT_EQUALS(str, "0 7");
        }

};

cxxtools::unit::RegisterTest<DateTest> register_DateTest;
