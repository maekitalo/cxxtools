/*
 * Copyright (C) 2012 Tommi Maekitalo
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

#include "cxxtools/unit/testsuite.h"
#include "cxxtools/unit/registertest.h"
#include "cxxtools/log.h"
#include "cxxtools/clock.h"
#include "cxxtools/timespan.h"

#include <thread>

log_define("cxxtools.test.clock")

class ClockTest : public cxxtools::unit::TestSuite
{
    public:
        ClockTest()
            : cxxtools::unit::TestSuite("clock")
        {
            registerMethod("testClock", *this, &ClockTest::testClock);
            // test for one second commented out since sleeping one second would slow down unit testing
            //registerMethod("testClockS", *this, &ClockTest::testClockS);
        }

        void testClock()
        {
            cxxtools::Clock cl;
            cl.start();
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            cxxtools::Timespan t = cl.stop();
            log_debug("timespan=" << t.totalMSecs() << "ms");
            CXXTOOLS_UNIT_ASSERT(t.totalMSecs() >= 1);
            // lets assume, that the test is below 1 second since the sleep is just one millisecond
            // without that test we wouldn't detect overflows in the implementation
            CXXTOOLS_UNIT_ASSERT(t.totalMSecs() < 1000);
        }

        void testClockS()
        {
            cxxtools::Clock cl;
            cl.start();
            std::this_thread::sleep_for(std::chrono::milliseconds(1001));
            cxxtools::Timespan t = cl.stop();
            log_debug("timespan=" << t.totalMSecs() << "ms");
            CXXTOOLS_UNIT_ASSERT(t.totalMSecs() > 1000);
            CXXTOOLS_UNIT_ASSERT(t.totalMSecs() < 2000);
        }

};

cxxtools::unit::RegisterTest<ClockTest> register_ClockTest;
