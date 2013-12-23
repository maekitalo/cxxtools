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
#include "cxxtools/datetime.h"
#include "cxxtools/unit/testsuite.h"
#include "cxxtools/unit/registertest.h"

class DateTimeTest : public cxxtools::unit::TestSuite
{
    public:
        DateTimeTest()
        : cxxtools::unit::TestSuite("datetime")
        {
            registerMethod("diff", *this, &DateTimeTest::diff);
            registerMethod("fromString", *this, &DateTimeTest::fromString);
            registerMethod("toString", *this, &DateTimeTest::toString);
        }

        void diff()
        {
            cxxtools::DateTime dt1 = cxxtools::DateTime("2013-05-03 17:01:14.342");
            cxxtools::DateTime dt2 = cxxtools::DateTime("2013-05-04 17:01:14.342");
            cxxtools::Timespan dt = dt2 - dt1;
            CXXTOOLS_UNIT_ASSERT_EQUALS(dt.totalDays(), 1.0);
            CXXTOOLS_UNIT_ASSERT_EQUALS(dt.totalHours(), 24.0);
            CXXTOOLS_UNIT_ASSERT_EQUALS(dt.totalMinutes(), 24.0 * 60.0);
            CXXTOOLS_UNIT_ASSERT_EQUALS(dt.totalSeconds(), 24.0 * 60.0 * 60.0);
            CXXTOOLS_UNIT_ASSERT_EQUALS(dt.totalMSecs(), 24.0 * 60.0 * 60.0 * 1000);
            CXXTOOLS_UNIT_ASSERT_EQUALS(dt.totalUSecs(), 24.0 * 60.0 * 60.0 * 1000 * 1000);

            dt1 = cxxtools::DateTime("2013-05-03 17:01:14.000");
            dt2 = cxxtools::DateTime("2013-05-03 17:01:14.342");
            dt = dt2 - dt1;
            CXXTOOLS_UNIT_ASSERT_EQUALS(dt.totalUSecs(), 342000);
        }

        void fromString()
        {
            int year;
            unsigned month, day, hours, minutes, seconds, milliseconds;

            cxxtools::DateTime dt;

            dt = cxxtools::DateTime("2013-05-03 17:01:14.342");

            dt.get(year, month, day, hours, minutes, seconds, milliseconds);
            CXXTOOLS_UNIT_ASSERT_EQUALS(year, 2013);
            CXXTOOLS_UNIT_ASSERT_EQUALS(month, 5);
            CXXTOOLS_UNIT_ASSERT_EQUALS(day, 3);
            CXXTOOLS_UNIT_ASSERT_EQUALS(hours, 17);
            CXXTOOLS_UNIT_ASSERT_EQUALS(minutes, 1);
            CXXTOOLS_UNIT_ASSERT_EQUALS(seconds, 14);
            CXXTOOLS_UNIT_ASSERT_EQUALS(milliseconds, 342);

            dt = cxxtools::DateTime("2013 5 3 17 1 14.3", "%Y %m %d %H %M %S%J");

            dt.get(year, month, day, hours, minutes, seconds, milliseconds);
            CXXTOOLS_UNIT_ASSERT_EQUALS(year, 2013);
            CXXTOOLS_UNIT_ASSERT_EQUALS(month, 5);
            CXXTOOLS_UNIT_ASSERT_EQUALS(day, 3);
            CXXTOOLS_UNIT_ASSERT_EQUALS(hours, 17);
            CXXTOOLS_UNIT_ASSERT_EQUALS(minutes, 1);
            CXXTOOLS_UNIT_ASSERT_EQUALS(seconds, 14);
            CXXTOOLS_UNIT_ASSERT_EQUALS(milliseconds, 300);

            dt = cxxtools::DateTime("2013 5 3 17 1 14", "%Y %m %d %H %M %S%J");

            dt.get(year, month, day, hours, minutes, seconds, milliseconds);
            CXXTOOLS_UNIT_ASSERT_EQUALS(year, 2013);
            CXXTOOLS_UNIT_ASSERT_EQUALS(month, 5);
            CXXTOOLS_UNIT_ASSERT_EQUALS(day, 3);
            CXXTOOLS_UNIT_ASSERT_EQUALS(hours, 17);
            CXXTOOLS_UNIT_ASSERT_EQUALS(minutes, 1);
            CXXTOOLS_UNIT_ASSERT_EQUALS(seconds, 14);
            CXXTOOLS_UNIT_ASSERT_EQUALS(milliseconds, 0);

            dt = cxxtools::DateTime("2013-5-3 5:01:14 pm", "%Y-%m-%d %H:%M:%S %p");

            dt.get(year, month, day, hours, minutes, seconds, milliseconds);
            CXXTOOLS_UNIT_ASSERT_EQUALS(year, 2013);
            CXXTOOLS_UNIT_ASSERT_EQUALS(month, 5);
            CXXTOOLS_UNIT_ASSERT_EQUALS(day, 3);
            CXXTOOLS_UNIT_ASSERT_EQUALS(hours, 17);
            CXXTOOLS_UNIT_ASSERT_EQUALS(minutes, 1);
            CXXTOOLS_UNIT_ASSERT_EQUALS(seconds, 14);
            CXXTOOLS_UNIT_ASSERT_EQUALS(milliseconds, 0);

        }

        void toString()
        {
          cxxtools::DateTime dt(2013, 5, 3, 17, 1, 14, 342);

          std::string str = dt.toString();
          CXXTOOLS_UNIT_ASSERT_EQUALS(str, "2013-05-03 17:01:14.342");

          str = dt.toString("%Y %m %d %H %M %S%J");
          CXXTOOLS_UNIT_ASSERT_EQUALS(str, "2013 05 03 17 01 14.342");

          dt = cxxtools::DateTime(2013, 5, 3, 17, 1, 14);

          str = dt.toString("%Y %m %d %H %M %S%J");
          CXXTOOLS_UNIT_ASSERT_EQUALS(str, "2013 05 03 17 01 14.0");

          str = dt.toString("%Y %m %d %H %M %S%j");
          CXXTOOLS_UNIT_ASSERT_EQUALS(str, "2013 05 03 17 01 14");

          str = dt.toString("%Y %m %d %I %M %S%j %p");
          CXXTOOLS_UNIT_ASSERT_EQUALS(str, "2013 05 03 05 01 14 pm");

        }

};

cxxtools::unit::RegisterTest<DateTimeTest> register_DateTimeTest;
