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
            registerMethod("muldiv", *this, &TimespanTest::muldiv);
            registerMethod("microseconds", *this, &TimespanTest::microseconds);
            registerMethod("milliseconds", *this, &TimespanTest::milliseconds);
            registerMethod("seconds", *this, &TimespanTest::seconds);
            registerMethod("minutes", *this, &TimespanTest::minutes);
            registerMethod("hours", *this, &TimespanTest::hours);
            registerMethod("days", *this, &TimespanTest::days);
            registerMethod("serialize", *this, &TimespanTest::serialize);
            registerMethod("serializeunits", *this, &TimespanTest::serializeunits);
            registerMethod("deserializeunits", *this, &TimespanTest::deserializeunits);
            registerMethod("deserializeweaktimespan", *this, &TimespanTest::deserializeweaktimespan);
            registerMethod("ostream", *this, &TimespanTest::ostream);
            registerMethod("istream", *this, &TimespanTest::istream);
            registerMethod("istreamunits", *this, &TimespanTest::istreamunits);
            registerMethod("ceil", *this, &TimespanTest::ceil);
            registerMethod("parts", *this, &TimespanTest::parts);
            registerMethod("toString", *this, &TimespanTest::toString);
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

            t = -t1;
            CXXTOOLS_UNIT_ASSERT_EQUALS(t, cxxtools::Timespan(-4000));
        }

        void muldiv()
        {
            cxxtools::Seconds t1(2500);

            cxxtools::Timespan d = t1 * 2.0;
            CXXTOOLS_UNIT_ASSERT_EQUALS(d, cxxtools::Seconds(5000));

            d = 3.0 * t1;
            CXXTOOLS_UNIT_ASSERT_EQUALS(d, cxxtools::Seconds(7500));

            d = t1 / 2.0;
            CXXTOOLS_UNIT_ASSERT_EQUALS(d, cxxtools::Seconds(1250));
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

        void serializeunits()
        {
            {
                cxxtools::Microseconds t(12345678l);
                cxxtools::SerializationInfo si;
                si <<= t;
                cxxtools::Timespan t2;
                si >>= t2;
                CXXTOOLS_UNIT_ASSERT_EQUALS(t, t2);
                CXXTOOLS_UNIT_ASSERT_EQUALS(si.typeName(), "microseconds");

                long number = 0;
                si >>= number;
                CXXTOOLS_UNIT_ASSERT_EQUALS(number, 12345678l);
            }

            {
                cxxtools::Milliseconds t(17.875);
                cxxtools::SerializationInfo si;
                si <<= t;
                cxxtools::Timespan t2;
                si >>= t2;
                CXXTOOLS_UNIT_ASSERT_EQUALS(t, t2);
                CXXTOOLS_UNIT_ASSERT_EQUALS(si.typeName(), "milliseconds");

                double number = 0;
                si >>= number;
                CXXTOOLS_UNIT_ASSERT_EQUALS(number, 17.875);
            }

            {
                cxxtools::Seconds t(17.875);
                cxxtools::SerializationInfo si;
                si <<= t;
                cxxtools::Timespan t2;
                si >>= t2;
                CXXTOOLS_UNIT_ASSERT_EQUALS(t, t2);
                CXXTOOLS_UNIT_ASSERT_EQUALS(si.typeName(), "seconds");

                double number = 0;
                si >>= number;
                CXXTOOLS_UNIT_ASSERT_EQUALS(number, 17.875);
            }

            {
                cxxtools::Minutes t(17.875);
                cxxtools::SerializationInfo si;
                si <<= t;
                cxxtools::Timespan t2;
                si >>= t2;
                CXXTOOLS_UNIT_ASSERT_EQUALS(t, t2);
                CXXTOOLS_UNIT_ASSERT_EQUALS(si.typeName(), "minutes");

                double number = 0;
                si >>= number;
                CXXTOOLS_UNIT_ASSERT_EQUALS(number, 17.875);
            }

            {
                cxxtools::Hours t(17.875);
                cxxtools::SerializationInfo si;
                si <<= t;
                cxxtools::Timespan t2;
                si >>= t2;
                CXXTOOLS_UNIT_ASSERT_EQUALS(t, t2);
                CXXTOOLS_UNIT_ASSERT_EQUALS(si.typeName(), "hours");

                double number = 0;
                si >>= number;
                CXXTOOLS_UNIT_ASSERT_EQUALS(number, 17.875);
            }

            {
                cxxtools::Days t(17.875);
                cxxtools::SerializationInfo si;
                si <<= t;
                cxxtools::Timespan t2;
                si >>= t2;
                CXXTOOLS_UNIT_ASSERT_EQUALS(t, t2);
                CXXTOOLS_UNIT_ASSERT_EQUALS(si.typeName(), "days");

                double number = 0;
                si >>= number;
                CXXTOOLS_UNIT_ASSERT_EQUALS(number, 17.875);
            }

        }

        void deserializeunits()
        {
            {
                cxxtools::SerializationInfo si;
                si <<= "2us";
                cxxtools::Timespan t;
                si >>= t;
                CXXTOOLS_UNIT_ASSERT_EQUALS(t, cxxtools::Microseconds(2));
            }

            {
                cxxtools::SerializationInfo si;
                si <<= "2.5ms";
                cxxtools::Timespan t;
                si >>= t;
                CXXTOOLS_UNIT_ASSERT_EQUALS(t, cxxtools::Milliseconds(2.5));
            }

            {
                cxxtools::SerializationInfo si;
                si <<= "2.5s";
                cxxtools::Timespan t;
                si >>= t;
                CXXTOOLS_UNIT_ASSERT_EQUALS(t, cxxtools::Seconds(2.5));
            }

            {
                cxxtools::SerializationInfo si;
                si <<= "2.5min";
                cxxtools::Timespan t;
                si >>= t;
                CXXTOOLS_UNIT_ASSERT_EQUALS(t, cxxtools::Minutes(2.5));
            }

            {
                cxxtools::SerializationInfo si;
                si <<= "2.5ho";
                cxxtools::Timespan t;
                si >>= t;
                CXXTOOLS_UNIT_ASSERT_EQUALS(t, cxxtools::Hours(2.5));
            }

            {
                cxxtools::SerializationInfo si;
                si <<= "2.5days";
                cxxtools::Timespan t;
                si >>= t;
                CXXTOOLS_UNIT_ASSERT_EQUALS(t, cxxtools::Days(2.5));
            }

        }

        void deserializeweaktimespan()
        {
            {
                cxxtools::SerializationInfo si;
                si <<= "2";
                cxxtools::Timespan t;
                si >>= t;
                CXXTOOLS_UNIT_ASSERT_EQUALS(t, cxxtools::Microseconds(2));
            }

            {
                cxxtools::SerializationInfo si;
                si <<= "2.5";
                cxxtools::Milliseconds t;
                si >>= t;
                CXXTOOLS_UNIT_ASSERT_EQUALS(t, cxxtools::Milliseconds(2.5));
            }

            {
                cxxtools::SerializationInfo si;
                si <<= "2.5";
                cxxtools::Seconds t;
                si >>= t;
                CXXTOOLS_UNIT_ASSERT_EQUALS(t, cxxtools::Seconds(2.5));
            }

            {
                cxxtools::SerializationInfo si;
                si <<= "2.5";
                cxxtools::Minutes t;
                si >>= t;
                CXXTOOLS_UNIT_ASSERT_EQUALS(t, cxxtools::Minutes(2.5));
            }

            {
                cxxtools::SerializationInfo si;
                si <<= "2.5";
                cxxtools::Hours t;
                si >>= t;
                CXXTOOLS_UNIT_ASSERT_EQUALS(t, cxxtools::Hours(2.5));
            }

            {
                cxxtools::SerializationInfo si;
                si <<= "2.5";
                cxxtools::Days t;
                si >>= t;
                CXXTOOLS_UNIT_ASSERT_EQUALS(t, cxxtools::Days(2.5));
            }

        }


        void ostream()
        {
            {
                std::ostringstream s;
                s << cxxtools::Microseconds(823);
                CXXTOOLS_UNIT_ASSERT_EQUALS(s.str(), "823us");
            }

            {
                std::ostringstream s;
                s << cxxtools::Milliseconds(823);
                CXXTOOLS_UNIT_ASSERT_EQUALS(s.str(), "823ms");
            }

            {
                std::ostringstream s;
                s << cxxtools::Seconds(823);
                CXXTOOLS_UNIT_ASSERT_EQUALS(s.str(), "823s");
            }

            {
                std::ostringstream s;
                s << cxxtools::Minutes(823);
                CXXTOOLS_UNIT_ASSERT_EQUALS(s.str(), "823min");
            }

            {
                std::ostringstream s;
                s << cxxtools::Hours(823);
                CXXTOOLS_UNIT_ASSERT_EQUALS(s.str(), "823h");
            }

            {
                std::ostringstream s;
                s << cxxtools::Days(823);
                CXXTOOLS_UNIT_ASSERT_EQUALS(s.str(), "823d");
            }

        }

        void istream()
        {
            {
                std::istringstream s("823");
                cxxtools::Microseconds ts;
                s >> ts;
                CXXTOOLS_UNIT_ASSERT(!s.fail());
                CXXTOOLS_UNIT_ASSERT_EQUALS(ts, cxxtools::Microseconds(823));
            }

            {
                std::istringstream s("823");
                cxxtools::Milliseconds ts;
                s >> ts;
                CXXTOOLS_UNIT_ASSERT(s);
                CXXTOOLS_UNIT_ASSERT_EQUALS(ts, cxxtools::Milliseconds(823));
            }

            {
                std::istringstream s("823");
                cxxtools::Seconds ts;
                s >> ts;
                CXXTOOLS_UNIT_ASSERT(s);
                CXXTOOLS_UNIT_ASSERT_EQUALS(ts, cxxtools::Seconds(823));
            }

            {
                std::istringstream s("823");
                cxxtools::Minutes ts;
                s >> ts;
                CXXTOOLS_UNIT_ASSERT(s);
                CXXTOOLS_UNIT_ASSERT_EQUALS(ts, cxxtools::Minutes(823));
            }

            {
                std::istringstream s("823");
                cxxtools::Hours ts;
                s >> ts;
                CXXTOOLS_UNIT_ASSERT(s);
                CXXTOOLS_UNIT_ASSERT_EQUALS(ts, cxxtools::Hours(823));
            }

            {
                std::istringstream s("823");
                cxxtools::Days ts;
                s >> ts;
                CXXTOOLS_UNIT_ASSERT(s);
                CXXTOOLS_UNIT_ASSERT_EQUALS(ts, cxxtools::Days(823));
            }
        }

        void istreamunits()
        {
            {
                std::istringstream s("823u");
                cxxtools::Timespan ts;
                s >> ts;
                CXXTOOLS_UNIT_ASSERT(!s.fail());
                CXXTOOLS_UNIT_ASSERT_EQUALS(ts, cxxtools::Microseconds(823));
            }

            {
                std::istringstream s("823ms");
                cxxtools::Seconds ts;
                s >> ts;
                CXXTOOLS_UNIT_ASSERT(s);
                CXXTOOLS_UNIT_ASSERT_EQUALS(ts, cxxtools::Milliseconds(823));
            }

            {
                std::istringstream s("823s");
                cxxtools::Timespan ts;
                s >> ts;
                CXXTOOLS_UNIT_ASSERT(s);
                CXXTOOLS_UNIT_ASSERT_EQUALS(ts, cxxtools::Seconds(823));
            }

            {
                std::istringstream s("823.5m");
                cxxtools::Timespan ts;
                s >> ts;
                CXXTOOLS_UNIT_ASSERT(s);
                CXXTOOLS_UNIT_ASSERT_EQUALS(ts, cxxtools::Minutes(823.5));
            }

            {
                std::istringstream s("823mi");
                cxxtools::Timespan ts;
                s >> ts;
                CXXTOOLS_UNIT_ASSERT(s);
                CXXTOOLS_UNIT_ASSERT_EQUALS(ts, cxxtools::Minutes(823));
            }

            {
                std::istringstream s("823h");
                cxxtools::Timespan ts;
                s >> ts;
                CXXTOOLS_UNIT_ASSERT(s);
                CXXTOOLS_UNIT_ASSERT_EQUALS(ts, cxxtools::Hours(823));
            }

            {
                std::istringstream s("823d");
                cxxtools::Timespan ts;
                s >> ts;
                CXXTOOLS_UNIT_ASSERT(s);
                CXXTOOLS_UNIT_ASSERT_EQUALS(ts, cxxtools::Days(823));
            }
        }

        void ceil()
        {
            CXXTOOLS_UNIT_ASSERT_EQUALS(cxxtools::Milliseconds(4).ceil(), 4);
            CXXTOOLS_UNIT_ASSERT_EQUALS(cxxtools::Milliseconds(4.1).ceil(), 5);
            CXXTOOLS_UNIT_ASSERT_EQUALS(cxxtools::Milliseconds(-1).ceil(), -1);
            CXXTOOLS_UNIT_ASSERT_EQUALS(cxxtools::Milliseconds(-.9).ceil(), 0);
            CXXTOOLS_UNIT_ASSERT_EQUALS(cxxtools::Milliseconds(-9).ceil(), -9);
            CXXTOOLS_UNIT_ASSERT_EQUALS(cxxtools::Milliseconds(-11.1).ceil(), -11);
        }

        void parts()
        {
            cxxtools::Timespan ts = cxxtools::Hours(822)
                                  + cxxtools::Minutes(17)
                                  + cxxtools::Seconds(42)
                                  + cxxtools::Milliseconds(453)
                                  + cxxtools::Microseconds(85);
            CXXTOOLS_UNIT_ASSERT_EQUALS(ts.hours(), 822u);
            CXXTOOLS_UNIT_ASSERT_EQUALS(ts.minutes(), 17u);
            CXXTOOLS_UNIT_ASSERT_EQUALS(ts.seconds(), 42u);
            CXXTOOLS_UNIT_ASSERT_EQUALS(ts.milliseconds(), 453u);
            CXXTOOLS_UNIT_ASSERT_EQUALS(ts.microseconds(), 453085u);
        }

        void toString()
        {
            CXXTOOLS_UNIT_ASSERT_EQUALS(cxxtools::Milliseconds(4).toString(), "0.004");
            CXXTOOLS_UNIT_ASSERT_EQUALS(cxxtools::Hours(22).toString(), "22:00:00");
            CXXTOOLS_UNIT_ASSERT_EQUALS(cxxtools::Milliseconds(1200).toString(), "1.2");
            CXXTOOLS_UNIT_ASSERT_EQUALS(cxxtools::Microseconds(140).toString(), "0.00014");
            cxxtools::Timespan ts = cxxtools::Hours(823)
                                  + cxxtools::Minutes(17)
                                  + cxxtools::Seconds(42)
                                  + cxxtools::Milliseconds(453)
                                  + cxxtools::Microseconds(850);
            CXXTOOLS_UNIT_ASSERT_EQUALS(ts.toString(), "823:17:42.45385");
        }
};

cxxtools::unit::RegisterTest<TimespanTest> register_TimespanTest;
