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
#include "cxxtools/timespan.h"
#include "cxxtools/unit/testsuite.h"
#include "cxxtools/unit/registertest.h"
#include "cxxtools/serializationinfo.h"

class TimespanTest : public cxxtools::unit::TestSuite
{
    public:
        TimespanTest()
        : cxxtools::unit::TestSuite("timespan")
        {
            registerMethod("compare", *this, &TimespanTest::compare);
            registerMethod("arithmetic", *this, &TimespanTest::arithmetic);
            registerMethod("microseconds", *this, &TimespanTest::microseconds);
            registerMethod("milliseconds", *this, &TimespanTest::milliseconds);
            registerMethod("seconds", *this, &TimespanTest::seconds);
            registerMethod("minutes", *this, &TimespanTest::minutes);
            registerMethod("hours", *this, &TimespanTest::hours);
            registerMethod("days", *this, &TimespanTest::days);
            registerMethod("serialize", *this, &TimespanTest::serialize);
        }

        void compare()
        {
            cxxtools::Timespan t(1234567);
            cxxtools::Timespan t1(1234567);
            cxxtools::Timespan t2(1234568);

            CXXTOOLS_UNIT_ASSERT(t == t1);
            CXXTOOLS_UNIT_ASSERT(!(t == t2));

            CXXTOOLS_UNIT_ASSERT(!(t != t1));
            CXXTOOLS_UNIT_ASSERT(t != t2);

            CXXTOOLS_UNIT_ASSERT(t1 < t2);
            CXXTOOLS_UNIT_ASSERT(!(t2 < t1));
            CXXTOOLS_UNIT_ASSERT(!(t2 <= t1));
            CXXTOOLS_UNIT_ASSERT(t1 <= t1);
            CXXTOOLS_UNIT_ASSERT(t1 <= t2);

            CXXTOOLS_UNIT_ASSERT(t2 > t1);
            CXXTOOLS_UNIT_ASSERT(!(t1 > t2));
            CXXTOOLS_UNIT_ASSERT(!(t1 >= t2));
            CXXTOOLS_UNIT_ASSERT(t2 >= t2);
            CXXTOOLS_UNIT_ASSERT(t2 >= t1);

        }

        void arithmetic()
        {
            cxxtools::Timespan t1(4000);
            cxxtools::Timespan t2(100);

            cxxtools::Timespan sum = t1 + t2;
            CXXTOOLS_UNIT_ASSERT_EQUALS(sum, cxxtools::Timespan(4100));

            cxxtools::Timespan diff = t1 - t2;
            CXXTOOLS_UNIT_ASSERT_EQUALS(diff, cxxtools::Timespan(3900));

            cxxtools::Timespan t;

            t = t1;
            t += t2;
            CXXTOOLS_UNIT_ASSERT_EQUALS(t, cxxtools::Timespan(4100));

            t = t1;
            t -= t2;
            CXXTOOLS_UNIT_ASSERT_EQUALS(t, cxxtools::Timespan(3900));

        }

        void microseconds()
        {
            cxxtools::Timespan t = cxxtools::Microseconds(1234567);
            CXXTOOLS_UNIT_ASSERT_EQUALS(t.totalUSecs(), 1234567);
            // due to rounding errors msecs are just nearly equal:
            CXXTOOLS_UNIT_ASSERT(t.totalMSecs()/1234.567 > 0.999999 && t.totalMSecs()/1234.567 < 1.000001);
            CXXTOOLS_UNIT_ASSERT_EQUALS(cxxtools::Microseconds(t), 1234567);
        }

        void milliseconds()
        {
            cxxtools::Timespan t = cxxtools::Milliseconds(1234567);
            CXXTOOLS_UNIT_ASSERT_EQUALS(t.totalUSecs(), 1234567000);
            CXXTOOLS_UNIT_ASSERT_EQUALS(t.totalMSecs(), 1234567);
            CXXTOOLS_UNIT_ASSERT_EQUALS(cxxtools::Milliseconds(t), 1234567);
        }

        void seconds()
        {
            cxxtools::Timespan t = cxxtools::Seconds(1234);
            CXXTOOLS_UNIT_ASSERT_EQUALS(t.totalUSecs(), 1234000000);
            CXXTOOLS_UNIT_ASSERT_EQUALS(t.totalMSecs(), 1234000);
            CXXTOOLS_UNIT_ASSERT_EQUALS(t.totalSeconds(), 1234);
            CXXTOOLS_UNIT_ASSERT_EQUALS(cxxtools::Seconds(t), 1234);
        }

        void minutes()
        {
            cxxtools::Timespan t = cxxtools::Minutes(17.5);
            CXXTOOLS_UNIT_ASSERT_EQUALS(t.totalUSecs(), (17*60+30)*1000000);
            CXXTOOLS_UNIT_ASSERT_EQUALS(t.totalMSecs(), (17*60+30)*1000);
            CXXTOOLS_UNIT_ASSERT_EQUALS(t.totalSeconds(), 17*60+30);
            CXXTOOLS_UNIT_ASSERT_EQUALS(cxxtools::Minutes(t), 17.5);
        }

        void hours()
        {
            cxxtools::Timespan t = cxxtools::Hours(17.5);
            CXXTOOLS_UNIT_ASSERT_EQUALS(t.totalSeconds(), 17*3600+1800);
            CXXTOOLS_UNIT_ASSERT_EQUALS(cxxtools::Hours(t), 17.5);
        }

        void days()
        {
            cxxtools::Timespan t = cxxtools::Days(17.5);
            CXXTOOLS_UNIT_ASSERT_EQUALS(t.totalSeconds(), (17*3600+1800)*24);
            CXXTOOLS_UNIT_ASSERT_EQUALS(cxxtools::Days(t), 17.5);
        }

        void serialize()
        {
            cxxtools::Timespan t(1234567);
            cxxtools::SerializationInfo si;
            si <<= t;
            cxxtools::Timespan t2;
            si >>= t2;
            CXXTOOLS_UNIT_ASSERT_EQUALS(t, t2);
        }

};

cxxtools::unit::RegisterTest<TimespanTest> register_TimespanTest;
