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

#include "cxxtools/date.h"

#include "cxxtools/serializationinfo.h"

#include "cxxtools/unit/testsuite.h"
#include "cxxtools/unit/registertest.h"

class DateTest : public cxxtools::unit::TestSuite
{
    public:
        DateTest()
        : cxxtools::unit::TestSuite("date")
        {
            registerMethod("fromString", *this, &DateTest::fromString);
            registerMethod("fromStringMonthname", *this, &DateTest::fromStringMonthname);
            registerMethod("fixDigit", *this, &DateTest::fixDigit);
            registerMethod("toString", *this, &DateTest::toString);
            registerMethod("names", *this, &DateTest::names);
            registerMethod("serialization", *this, &DateTest::serialization);
            registerMethod("isValid", *this, &DateTest::isValid);
            registerMethod("weeknumber", *this, &DateTest::weeknumber);
        }

        void fromString()
        {
            int year;
            unsigned month, day;

            cxxtools::Date dt;

            dt = cxxtools::Date("2013-05-03");

            dt.get(year, month, day);
            CXXTOOLS_UNIT_ASSERT_EQUALS(year, 2013);
            CXXTOOLS_UNIT_ASSERT_EQUALS(month, 5u);
            CXXTOOLS_UNIT_ASSERT_EQUALS(day, 3u);

            dt = cxxtools::Date("2013 5 3", "%Y %m %d");

            dt.get(year, month, day);
            CXXTOOLS_UNIT_ASSERT_EQUALS(year, 2013);
            CXXTOOLS_UNIT_ASSERT_EQUALS(month, 5u);
            CXXTOOLS_UNIT_ASSERT_EQUALS(day, 3u);

            dt = cxxtools::Date("20140802", "%Y%m%d");

            dt.get(year, month, day);
            CXXTOOLS_UNIT_ASSERT_EQUALS(year, 2014);
            CXXTOOLS_UNIT_ASSERT_EQUALS(month, 8u);
            CXXTOOLS_UNIT_ASSERT_EQUALS(day, 2u);

            dt = cxxtools::Date("1975H11#12", "%Y?%m?%d");

            dt.get(year, month, day);
            CXXTOOLS_UNIT_ASSERT_EQUALS(year, 1975);
            CXXTOOLS_UNIT_ASSERT_EQUALS(month, 11u);
            CXXTOOLS_UNIT_ASSERT_EQUALS(day, 12u);

            dt = cxxtools::Date("1893 foobar 10 hi there 30", "%Y*%m*%d");

            dt.get(year, month, day);
            CXXTOOLS_UNIT_ASSERT_EQUALS(year, 1893);
            CXXTOOLS_UNIT_ASSERT_EQUALS(month, 10u);
            CXXTOOLS_UNIT_ASSERT_EQUALS(day, 30u);

        }

        void fromStringMonthname()
        {
            int year;
            unsigned month, day;
            cxxtools::Date dt;

            CXXTOOLS_UNIT_ASSERT_NOTHROW(dt = cxxtools::Date("Wed Apr 11 2018", "??? %O %d %Y"));
            dt.get(year, month, day);

            CXXTOOLS_UNIT_ASSERT_EQUALS(year, 2018);
            CXXTOOLS_UNIT_ASSERT_EQUALS(month, 4u);
            CXXTOOLS_UNIT_ASSERT_EQUALS(day, 11u);

            static const char* monthnames[12] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
            for (unsigned m = 0; m < 12; ++m)
            {
              CXXTOOLS_UNIT_ASSERT_NOTHROW(dt = cxxtools::Date(monthnames[m], "%O"));
              dt.get(year, month, day);
              CXXTOOLS_UNIT_ASSERT_EQUALS(month, m + 1);
            }
        }

        void fixDigit()
        {
            CXXTOOLS_UNIT_ASSERT_NOTHROW(cxxtools::Date("3 9", "%m %d"));
            CXXTOOLS_UNIT_ASSERT_THROW(cxxtools::Date("3 9", "%2m %2d"), cxxtools::InvalidDate);
            CXXTOOLS_UNIT_ASSERT_NOTHROW(cxxtools::Date("03 09", "%2m %2d"));
        }

        void toString()
        {
          cxxtools::Date dt(2013, 5, 3);

          std::string str = dt.toString();
          CXXTOOLS_UNIT_ASSERT_EQUALS(str, "2013-05-03");

          str = dt.toString("%Y %m %d");
          CXXTOOLS_UNIT_ASSERT_EQUALS(str, "2013 05 03");

          str = dt.toString("%Y%m%d");
          CXXTOOLS_UNIT_ASSERT_EQUALS(str, "20130503");

          str = dt.toString("%w %W");
          CXXTOOLS_UNIT_ASSERT_EQUALS(str, "5 5");

          dt = cxxtools::Date(2013, 5, 5);
          str = dt.toString("%w %W");
          CXXTOOLS_UNIT_ASSERT_EQUALS(str, "0 7");
        }

        void names()
        {
          cxxtools::Date dt(2013, 5, 3);
          std::string str = dt.toString("%O %N");
          CXXTOOLS_UNIT_ASSERT_EQUALS(str, "May Fri");

          dt.set(2013, 6, 2);
          str = dt.toString("%O %N");
          CXXTOOLS_UNIT_ASSERT_EQUALS(str, "Jun Sun");
        }

        void serialization()
        {
            cxxtools::Date d1(2014, 5, 3);

            cxxtools::SerializationInfo si;
            si <<= d1;

            cxxtools::Date d2;
            si >>= d2;

            CXXTOOLS_UNIT_ASSERT(d1 == d2);
        }

        void isValid()
        {
            CXXTOOLS_UNIT_ASSERT(cxxtools::Date::isValid(1, 1, 1));

            CXXTOOLS_UNIT_ASSERT(cxxtools::Date::isValid(2000, 12, 1));
            CXXTOOLS_UNIT_ASSERT(!cxxtools::Date::isValid(2000, 13, 1));
            CXXTOOLS_UNIT_ASSERT(!cxxtools::Date::isValid(2000, 0, 1));

            CXXTOOLS_UNIT_ASSERT(cxxtools::Date::isValid(2015, 4, 30));
            CXXTOOLS_UNIT_ASSERT(!cxxtools::Date::isValid(2015, 4, 31));

            CXXTOOLS_UNIT_ASSERT(cxxtools::Date::isValid(2015, 5, 31));
            CXXTOOLS_UNIT_ASSERT(!cxxtools::Date::isValid(2015, 5, 32));

            CXXTOOLS_UNIT_ASSERT(cxxtools::Date::isValid(2015, 2, 28));
            CXXTOOLS_UNIT_ASSERT(!cxxtools::Date::isValid(2015, 2, 29));
            CXXTOOLS_UNIT_ASSERT(cxxtools::Date::isValid(2012, 2, 29));
            CXXTOOLS_UNIT_ASSERT(cxxtools::Date::isValid(2000, 2, 29));
            CXXTOOLS_UNIT_ASSERT(cxxtools::Date::isValid(1900, 2, 28));
            CXXTOOLS_UNIT_ASSERT(!cxxtools::Date::isValid(1900, 2, 29));
        }

        void weeknumber()
        {
            CXXTOOLS_UNIT_ASSERT_EQUALS(cxxtools::Date(2020, 1, 1).weeknumber(), 1u);
            CXXTOOLS_UNIT_ASSERT_EQUALS(cxxtools::Date(2020, 1, 4).weeknumber(), 1u);
            CXXTOOLS_UNIT_ASSERT_EQUALS(cxxtools::Date(2020, 1, 5).weeknumber(), 2u);
            CXXTOOLS_UNIT_ASSERT_EQUALS(cxxtools::Date(2020, 2, 1).weeknumber(), 5u);
            CXXTOOLS_UNIT_ASSERT_EQUALS(cxxtools::Date(2020, 12, 31).weeknumber(), 53u);
            CXXTOOLS_UNIT_ASSERT_EQUALS(cxxtools::Date(2019, 1, 5).weeknumber(), 1u);
            CXXTOOLS_UNIT_ASSERT_EQUALS(cxxtools::Date(2019, 1, 6).weeknumber(), 2u);
        }
};

cxxtools::unit::RegisterTest<DateTest> register_DateTest;
