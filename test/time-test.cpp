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

#include "cxxtools/time.h"

#include "cxxtools/serializationinfo.h"

#include "cxxtools/unit/testsuite.h"
#include "cxxtools/unit/registertest.h"

class TimeTest : public cxxtools::unit::TestSuite
{
    public:
        TimeTest()
        : cxxtools::unit::TestSuite("time")
        {
            registerMethod("access", *this, &TimeTest::access);
            registerMethod("diff", *this, &TimeTest::diff);
            registerMethod("fromString", *this, &TimeTest::fromString);
            registerMethod("fixDigit", *this, &TimeTest::fixDigit);
            registerMethod("toString", *this, &TimeTest::toString);
            registerMethod("serialization", *this, &TimeTest::serialization);
            registerMethod("getSetTotal", *this, &TimeTest::getSetTotal);
        }

        void access()
        {
            cxxtools::Time t(17, 1, 14, 300);
            CXXTOOLS_UNIT_ASSERT_EQUALS(t.hour(), 17);
            CXXTOOLS_UNIT_ASSERT_EQUALS(t.minute(), 1);
            CXXTOOLS_UNIT_ASSERT_EQUALS(t.second(), 14);
            CXXTOOLS_UNIT_ASSERT_EQUALS(t.msec(), 300);
            CXXTOOLS_UNIT_ASSERT_EQUALS(t.usec(), 300000);

            t.set(19, 55, 49, 345);
            CXXTOOLS_UNIT_ASSERT_EQUALS(t.hour(), 19);
            CXXTOOLS_UNIT_ASSERT_EQUALS(t.minute(), 55);
            CXXTOOLS_UNIT_ASSERT_EQUALS(t.second(), 49);
            CXXTOOLS_UNIT_ASSERT_EQUALS(t.msec(), 345);
            CXXTOOLS_UNIT_ASSERT_EQUALS(t.usec(), 345000);
        }

        void diff()
        {
            cxxtools::Time t1(17, 1, 2, 300);
            cxxtools::Time t2(16, 4, 5, 600);

            cxxtools::Timespan diff = t1 - t2;
            CXXTOOLS_UNIT_ASSERT_EQUALS(diff, cxxtools::Milliseconds(3416700));

            diff = t2 - t1;
            CXXTOOLS_UNIT_ASSERT_EQUALS(diff, cxxtools::Milliseconds(-3416700));

            CXXTOOLS_UNIT_ASSERT_THROW(t1 - cxxtools::Hours(18), std::overflow_error);
            CXXTOOLS_UNIT_ASSERT_THROW(t1 + cxxtools::Hours(8), std::overflow_error);
        }

        void fromString()
        {
            unsigned hours, minutes, seconds, milliseconds, microseconds;

            cxxtools::Time dt;

            CXXTOOLS_UNIT_ASSERT_NOTHROW(dt = cxxtools::Time("17:01:14.3428"));

            dt.get(hours, minutes, seconds, milliseconds, microseconds);
            CXXTOOLS_UNIT_ASSERT_EQUALS(hours, 17);
            CXXTOOLS_UNIT_ASSERT_EQUALS(minutes, 1);
            CXXTOOLS_UNIT_ASSERT_EQUALS(seconds, 14);
            CXXTOOLS_UNIT_ASSERT_EQUALS(milliseconds, 342);
            CXXTOOLS_UNIT_ASSERT_EQUALS(microseconds, 342800);

            dt = cxxtools::Time("17 1 14.3", "%H %M %S%J");

            dt.get(hours, minutes, seconds, milliseconds, microseconds);
            CXXTOOLS_UNIT_ASSERT_EQUALS(hours, 17);
            CXXTOOLS_UNIT_ASSERT_EQUALS(minutes, 1);
            CXXTOOLS_UNIT_ASSERT_EQUALS(seconds, 14);
            CXXTOOLS_UNIT_ASSERT_EQUALS(milliseconds, 300);
            CXXTOOLS_UNIT_ASSERT_EQUALS(microseconds, 300000);

            dt = cxxtools::Time("17 1 14", "%H %M %S%J");

            dt.get(hours, minutes, seconds, milliseconds, microseconds);
            CXXTOOLS_UNIT_ASSERT_EQUALS(hours, 17);
            CXXTOOLS_UNIT_ASSERT_EQUALS(minutes, 1);
            CXXTOOLS_UNIT_ASSERT_EQUALS(seconds, 14);
            CXXTOOLS_UNIT_ASSERT_EQUALS(milliseconds, 0);
            CXXTOOLS_UNIT_ASSERT_EQUALS(microseconds, 0);

            dt = cxxtools::Time("5:01:14 pm", "%H:%M:%S %p");

            dt.get(hours, minutes, seconds, milliseconds, microseconds);
            CXXTOOLS_UNIT_ASSERT_EQUALS(hours, 17);
            CXXTOOLS_UNIT_ASSERT_EQUALS(minutes, 1);
            CXXTOOLS_UNIT_ASSERT_EQUALS(seconds, 14);
            CXXTOOLS_UNIT_ASSERT_EQUALS(milliseconds, 0);
            CXXTOOLS_UNIT_ASSERT_EQUALS(microseconds, 0);

            dt = cxxtools::Time("9    13 b 54  xx  ", "%H*%M*%S*");

            dt.get(hours, minutes, seconds, milliseconds, microseconds);
            CXXTOOLS_UNIT_ASSERT_EQUALS(hours, 9);
            CXXTOOLS_UNIT_ASSERT_EQUALS(minutes, 13);
            CXXTOOLS_UNIT_ASSERT_EQUALS(seconds, 54);
            CXXTOOLS_UNIT_ASSERT_EQUALS(milliseconds, 0);
            CXXTOOLS_UNIT_ASSERT_EQUALS(microseconds, 0);

        }

        void fixDigit()
        {
            CXXTOOLS_UNIT_ASSERT_NOTHROW(cxxtools::Time("6 7 2", "%H %M %S"));
            CXXTOOLS_UNIT_ASSERT_THROW(cxxtools::Time("6 7 2", "%2H %2M %2S"), cxxtools::InvalidTime);
            CXXTOOLS_UNIT_ASSERT_NOTHROW(cxxtools::Time("06 07 02", "%2H %2M %2S"));
        }

        void toString()
        {
          cxxtools::Time dt(17, 1, 14, 342, 800);

          std::string str = dt.toString();
          CXXTOOLS_UNIT_ASSERT_EQUALS(str, "17:01:14.3428");

          str = dt.toString("%H %M %S%J");
          CXXTOOLS_UNIT_ASSERT_EQUALS(str, "17 01 14.3428");

          dt = cxxtools::Time(17, 1, 14);

          str = dt.toString("%H %M %S%J");
          CXXTOOLS_UNIT_ASSERT_EQUALS(str, "17 01 14.0");

          str = dt.toString("%H %M %S%K");
          CXXTOOLS_UNIT_ASSERT_EQUALS(str, "17 01 14.000");

          str = dt.toString("%H %M %S %k");
          CXXTOOLS_UNIT_ASSERT_EQUALS(str, "17 01 14 000");

          str = dt.toString("%H %M %S%U");
          CXXTOOLS_UNIT_ASSERT_EQUALS(str, "17 01 14.000000");

          str = dt.toString("%H %M %S %u");
          CXXTOOLS_UNIT_ASSERT_EQUALS(str, "17 01 14 000000");

          str = dt.toString("%H %M %S%j");
          CXXTOOLS_UNIT_ASSERT_EQUALS(str, "17 01 14");

          str = dt.toString("%I %M %S%j %p");
          CXXTOOLS_UNIT_ASSERT_EQUALS(str, "05 01 14 pm");

        }

        void serialization()
        {
            cxxtools::Time t1(17, 1, 14, 765);

            cxxtools::SerializationInfo si;
            si <<= t1;

            cxxtools::Time t2;
            si >>= t2;

            CXXTOOLS_UNIT_ASSERT(t1 == t2);
        }

        void getSetTotal()
        {
            cxxtools::Time t1(23, 5, 17, 123);
            uint64_t msecs = t1.totalMSecs();
            cxxtools::Time t2;
            t2.setTotalMSecs(msecs);

            CXXTOOLS_UNIT_ASSERT_EQUALS(t1.hour(), t2.hour());
            CXXTOOLS_UNIT_ASSERT_EQUALS(t1.minute(), t2.minute());
            CXXTOOLS_UNIT_ASSERT_EQUALS(t1.second(), t2.second());
            CXXTOOLS_UNIT_ASSERT_EQUALS(t1.msec(), t2.msec());
            CXXTOOLS_UNIT_ASSERT_EQUALS(t1.usec(), t2.usec());
            CXXTOOLS_UNIT_ASSERT(t1 == t2);

            t2 = cxxtools::Time();

            uint64_t usecs = t1.totalUSecs();
            t2.setTotalUSecs(usecs);

            CXXTOOLS_UNIT_ASSERT_EQUALS(t1.hour(), t2.hour());
            CXXTOOLS_UNIT_ASSERT_EQUALS(t1.minute(), t2.minute());
            CXXTOOLS_UNIT_ASSERT_EQUALS(t1.second(), t2.second());
            CXXTOOLS_UNIT_ASSERT_EQUALS(t1.msec(), t2.msec());
            CXXTOOLS_UNIT_ASSERT_EQUALS(t1.usec(), t2.usec());
            CXXTOOLS_UNIT_ASSERT(t1 == t2);

        }
};

cxxtools::unit::RegisterTest<TimeTest> register_TimeTest;
