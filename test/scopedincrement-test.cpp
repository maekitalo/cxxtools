/*
 * Copyright (C) 2014 Tommi Maekitalo
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

#include "cxxtools/scopedincrement.h"
#include "cxxtools/unit/testsuite.h"
#include "cxxtools/unit/registertest.h"

class ScopedIncrementTest : public cxxtools::unit::TestSuite
{
    public:
        ScopedIncrementTest()
        : cxxtools::unit::TestSuite("scopedincrement")
        {
            registerMethod("testIncrementInt", *this, &ScopedIncrementTest::testIncrementInt);
            registerMethod("testIncrementAtomic", *this, &ScopedIncrementTest::testIncrementAtomic);
        }

        void testIncrementInt()
        {
            int value = 7;

            {
                cxxtools::ScopedIncrement<int> inc(value);
                CXXTOOLS_UNIT_ASSERT_EQUALS(value, 8);
            }
            CXXTOOLS_UNIT_ASSERT_EQUALS(value, 7);

            {
                cxxtools::ScopedIncrement<int> inc(value, 5);
                CXXTOOLS_UNIT_ASSERT_EQUALS(value, 12);
            }
            CXXTOOLS_UNIT_ASSERT_EQUALS(value, 7);

            {
                cxxtools::ScopedIncrement<int> dec(value, -9);
                CXXTOOLS_UNIT_ASSERT_EQUALS(value, -2);
            }
            CXXTOOLS_UNIT_ASSERT_EQUALS(value, 7);

            try
            {
                cxxtools::ScopedIncrement<int> inc(value, 2);
                CXXTOOLS_UNIT_ASSERT_EQUALS(value, 9);
                throw 1;
            }
            catch (int)
            {
            }

            CXXTOOLS_UNIT_ASSERT_EQUALS(value, 7);
        }

        void testIncrementAtomic()
        {
          cxxtools::atomic_t value = 7;

            {
                cxxtools::ScopedIncrement<cxxtools::atomic_t> inc(value);
                CXXTOOLS_UNIT_ASSERT_EQUALS(cxxtools::atomicGet(value), 8);
            }
            CXXTOOLS_UNIT_ASSERT_EQUALS(cxxtools::atomicGet(value), 7);

            {
                cxxtools::ScopedIncrement<cxxtools::atomic_t> inc(value, 5);
                CXXTOOLS_UNIT_ASSERT_EQUALS(cxxtools::atomicGet(value), 12);
            }
            CXXTOOLS_UNIT_ASSERT_EQUALS(cxxtools::atomicGet(value), 7);

            {
                cxxtools::ScopedIncrement<cxxtools::atomic_t> dec(value, -9);
                CXXTOOLS_UNIT_ASSERT_EQUALS(cxxtools::atomicGet(value), -2);
            }
            CXXTOOLS_UNIT_ASSERT_EQUALS(cxxtools::atomicGet(value), 7);

            try
            {
                cxxtools::ScopedIncrement<cxxtools::atomic_t> inc(value, 2);
                CXXTOOLS_UNIT_ASSERT_EQUALS(cxxtools::atomicGet(value), 9);
                throw 1;
            }
            catch (int)
            {
            }

            CXXTOOLS_UNIT_ASSERT_EQUALS(cxxtools::atomicGet(value), 7);
        }

};

cxxtools::unit::RegisterTest<ScopedIncrementTest> register_ScopedIncrementTest;
