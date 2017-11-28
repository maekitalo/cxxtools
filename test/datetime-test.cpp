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

#include "cxxtools/datetime.h"

#include "cxxtools/serializationinfo.h"

#include "cxxtools/unit/testsuite.h"
#include "cxxtools/unit/registertest.h"

class DateTimeTest : public cxxtools::unit::TestSuite
{
    public:
        DateTimeTest()
        : cxxtools::unit::TestSuite("datetime")
        {
            registerMethod("diff", *this, &DateTimeTest::diff);
            registerMethod("arithmetic", *this, &DateTimeTest::arithmetic);
            registerMethod("fromString", *this, &DateTimeTest::fromString);
            registerMethod("fixDigit", *this, &DateTimeTest::fixDigit);
            registerMethod("toString", *this, &DateTimeTest::toString);
            registerMethod("serialization", *this, &DateTimeTest::serialization);
        }

        void diff()
        {
            cxxtools::DateTime dt1 = cxxtools::DateTime("2013-05-03 17:01:14.342");
            cxxtools::DateTime dt2 = cxxtools::DateTime("2013-05-04 17:01:14.342");
            cxxtools::Timespan dt = dt2 - dt1;
            CXXTOOLS_UNIT_ASSERT_EQUALS(cxxtools::Days(dt), 1.0);

            dt1 = cxxtools::DateTime("2013-05-03 17:01:14.000");
            dt2 = cxxtools::DateTime("2013-05-03 17:01:14.342");
            dt = dt2 - dt1;
            CXXTOOLS_UNIT_ASSERT_EQUALS(cxxtools::Microseconds(dt), 342000);
        }

        void arithmetic()
        {
            cxxtools::DateTime dt1(2014, 5, 3, 17, 1, 14);

            cxxtools::DateTime dt2 = dt1 - cxxtools::Days(1);
            CXXTOOLS_UNIT_ASSERT(dt2 == cxxtools::DateTime(2014, 5, 2, 17, 1, 14));

            dt2 = dt1 + cxxtools::Hours(8);
            CXXTOOLS_UNIT_ASSERT(dt2 == cxxtools::DateTime(2014, 5, 4, 1, 1, 14));

            dt2 = dt1 - cxxtools::Hours(18);
            CXXTOOLS_UNIT_ASSERT(dt2 == cxxtools::DateTime(2014, 5, 2, 23, 1, 14));

            dt1 = cxxtools::DateTime(2012, 2, 28, 1, 2, 3);
            dt2 = dt1 + cxxtools::Days(3);
            CXXTOOLS_UNIT_ASSERT(dt2 == cxxtools::DateTime(2012, 3, 2, 1, 2, 3));

            dt1 = cxxtools::DateTime(2012, 2, 20, 10, 0, 0);
            dt2 = dt1 + cxxtools::Hours(14);
            CXXTOOLS_UNIT_ASSERT(dt2 == cxxtools::DateTime(2012, 2, 21, 0, 0, 0));

        }

        void fromString()
        {
            int year;
            unsigned month, day, hours, minutes, seconds, milliseconds, microseconds;

            cxxtools::DateTime dt;

            CXXTOOLS_UNIT_ASSERT_NOTHROW(dt = cxxtools::DateTime("2013-05-03 17:01:14.3428"));

            dt.get(year, month, day, hours, minutes, seconds, milliseconds, microseconds);
            CXXTOOLS_UNIT_ASSERT_EQUALS(year, 2013);
            CXXTOOLS_UNIT_ASSERT_EQUALS(month, 5);
            CXXTOOLS_UNIT_ASSERT_EQUALS(day, 3);
            CXXTOOLS_UNIT_ASSERT_EQUALS(hours, 17);
            CXXTOOLS_UNIT_ASSERT_EQUALS(minutes, 1);
            CXXTOOLS_UNIT_ASSERT_EQUALS(seconds, 14);
            CXXTOOLS_UNIT_ASSERT_EQUALS(milliseconds, 342);
            CXXTOOLS_UNIT_ASSERT_EQUALS(microseconds, 342800);

            dt = cxxtools::DateTime("2013 5 3 17 1 14.3", "%Y %m %d %H %M %S%J");

            dt.get(year, month, day, hours, minutes, seconds, milliseconds, microseconds);
            CXXTOOLS_UNIT_ASSERT_EQUALS(year, 2013);
            CXXTOOLS_UNIT_ASSERT_EQUALS(month, 5);
            CXXTOOLS_UNIT_ASSERT_EQUALS(day, 3);
            CXXTOOLS_UNIT_ASSERT_EQUALS(hours, 17);
            CXXTOOLS_UNIT_ASSERT_EQUALS(minutes, 1);
            CXXTOOLS_UNIT_ASSERT_EQUALS(seconds, 14);
            CXXTOOLS_UNIT_ASSERT_EQUALS(milliseconds, 300);
            CXXTOOLS_UNIT_ASSERT_EQUALS(microseconds, 300000);

            dt = cxxtools::DateTime("2013 5 3 17 1 14", "%Y %m %d %H %M %S%J");

            dt.get(year, month, day, hours, minutes, seconds, milliseconds, microseconds);
            CXXTOOLS_UNIT_ASSERT_EQUALS(year, 2013);
            CXXTOOLS_UNIT_ASSERT_EQUALS(month, 5);
            CXXTOOLS_UNIT_ASSERT_EQUALS(day, 3);
            CXXTOOLS_UNIT_ASSERT_EQUALS(hours, 17);
            CXXTOOLS_UNIT_ASSERT_EQUALS(minutes, 1);
            CXXTOOLS_UNIT_ASSERT_EQUALS(seconds, 14);
            CXXTOOLS_UNIT_ASSERT_EQUALS(milliseconds, 0);
            CXXTOOLS_UNIT_ASSERT_EQUALS(microseconds, 0);

            dt = cxxtools::DateTime("2013-5-3 5:01:14 pm", "%Y-%m-%d %H:%M:%S %p");

            dt.get(year, month, day, hours, minutes, seconds, milliseconds, microseconds);
            CXXTOOLS_UNIT_ASSERT_EQUALS(year, 2013);
            CXXTOOLS_UNIT_ASSERT_EQUALS(month, 5);
            CXXTOOLS_UNIT_ASSERT_EQUALS(day, 3);
            CXXTOOLS_UNIT_ASSERT_EQUALS(hours, 17);
            CXXTOOLS_UNIT_ASSERT_EQUALS(minutes, 1);
            CXXTOOLS_UNIT_ASSERT_EQUALS(seconds, 14);
            CXXTOOLS_UNIT_ASSERT_EQUALS(milliseconds, 0);
            CXXTOOLS_UNIT_ASSERT_EQUALS(microseconds, 0);

            dt = cxxtools::DateTime("2013 foo 5 bar 3 5   01     14 pm", "%Y*%m*%d*%H*%M*%S %p");

            dt.get(year, month, day, hours, minutes, seconds, milliseconds, microseconds);
            CXXTOOLS_UNIT_ASSERT_EQUALS(year, 2013);
            CXXTOOLS_UNIT_ASSERT_EQUALS(month, 5);
            CXXTOOLS_UNIT_ASSERT_EQUALS(day, 3);
            CXXTOOLS_UNIT_ASSERT_EQUALS(hours, 17);
            CXXTOOLS_UNIT_ASSERT_EQUALS(minutes, 1);
            CXXTOOLS_UNIT_ASSERT_EQUALS(seconds, 14);
            CXXTOOLS_UNIT_ASSERT_EQUALS(milliseconds, 0);
            CXXTOOLS_UNIT_ASSERT_EQUALS(microseconds, 0);

            CXXTOOLS_UNIT_ASSERT_NOTHROW(dt = cxxtools::DateTime("5 3 9 1 6", "%m %d %H %M %S"));

            dt.get(year, month, day, hours, minutes, seconds, milliseconds, microseconds);
            CXXTOOLS_UNIT_ASSERT_EQUALS(month, 5);
            CXXTOOLS_UNIT_ASSERT_EQUALS(day, 3);
            CXXTOOLS_UNIT_ASSERT_EQUALS(hours, 9);
            CXXTOOLS_UNIT_ASSERT_EQUALS(minutes, 1);
            CXXTOOLS_UNIT_ASSERT_EQUALS(seconds, 6);

        }

        void fixDigit()
        {
            CXXTOOLS_UNIT_ASSERT_NOTHROW(cxxtools::DateTime("3 9 9 1 6", "%m %d %H %M %S"));
            CXXTOOLS_UNIT_ASSERT_THROW(cxxtools::DateTime("3 9 9 1 6", "%2m %2d %2H %2M %2S"), cxxtools::InvalidDate);
            CXXTOOLS_UNIT_ASSERT_NOTHROW(cxxtools::DateTime("03 09 09 01 06", "%2m %2d %2H %2M %2S"));
        }

        void toString()
        {
          cxxtools::DateTime dt(2013, 5, 3, 17, 1, 14, 342, 800);

          std::string str = dt.toString();
          CXXTOOLS_UNIT_ASSERT_EQUALS(str, "2013-05-03 17:01:14.3428");

          str = dt.toString("%Y %m %d %H %M %S%J");
          CXXTOOLS_UNIT_ASSERT_EQUALS(str, "2013 05 03 17 01 14.3428");

          str = dt.toString("%Y %1m %1d %1H %1M %1S%J");
          CXXTOOLS_UNIT_ASSERT_EQUALS(str, "2013 5 3 17 1 14.3428");

          dt = cxxtools::DateTime(2013, 5, 3, 17, 1, 14);

          str = dt.toString("%Y %m %d %H %M %S%J");
          CXXTOOLS_UNIT_ASSERT_EQUALS(str, "2013 05 03 17 01 14.0");

          str = dt.toString("%Y %m %d %H %M %S%K");
          CXXTOOLS_UNIT_ASSERT_EQUALS(str, "2013 05 03 17 01 14.000");

          str = dt.toString("%Y %m %d %H %M %S %k");
          CXXTOOLS_UNIT_ASSERT_EQUALS(str, "2013 05 03 17 01 14 000");

          str = dt.toString("%Y %m %d %H %M %S%U");
          CXXTOOLS_UNIT_ASSERT_EQUALS(str, "2013 05 03 17 01 14.000000");

          str = dt.toString("%Y %m %d %H %M %S %u");
          CXXTOOLS_UNIT_ASSERT_EQUALS(str, "2013 05 03 17 01 14 000000");

          str = dt.toString("%Y %m %d %H %M %S%j");
          CXXTOOLS_UNIT_ASSERT_EQUALS(str, "2013 05 03 17 01 14");

          str = dt.toString("%Y %m %d %I %M %S%j %p");
          CXXTOOLS_UNIT_ASSERT_EQUALS(str, "2013 05 03 05 01 14 pm");

        }

        void serialization()
        {
            {
                cxxtools::DateTime dt1;

                cxxtools::SerializationInfo si;
                si <<= dt1;

                cxxtools::DateTime dt2;
                si >>= dt2;

                CXXTOOLS_UNIT_ASSERT(dt1 == dt2);
            }
            {
                cxxtools::DateTime dt1(2014, 5, 3, 17, 1, 14);

                cxxtools::SerializationInfo si;
                si <<= dt1;

                cxxtools::DateTime dt2;
                si >>= dt2;

                CXXTOOLS_UNIT_ASSERT(dt1 == dt2);
            }
        }
};

cxxtools::unit::RegisterTest<DateTimeTest> register_DateTimeTest;
