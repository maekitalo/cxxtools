/*
 * Copyright (C) 2011 Tommi Maekitalo
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

#include "cxxtools/cache.h"
#include "cxxtools/unit/testsuite.h"
#include "cxxtools/unit/registertest.h"

class CacheTest : public cxxtools::unit::TestSuite
{
    public:
        CacheTest()
        : cxxtools::unit::TestSuite("cxxtools-cache-Test")
        {
            registerMethod("cacheTest", *this, &CacheTest::cacheTest);
        }

        void cacheTest()
        {
          cxxtools::Cache<int, int> cache(6);

          cache.put(1, 10);
          cache.put(2, 20);
          cache.put(3, 30);
          cache.put(4, 40);
          cache.put(5, 50);
          cache.put(6, 60);
          cache.put(7, 70);
          cache.put(8, 80);
          cache.put(9, 90);
          cache.put(10, 100);

          std::pair<bool, int> result;

          result = cache.getx(1);
          CXXTOOLS_UNIT_ASSERT(result.first);
          CXXTOOLS_UNIT_ASSERT_EQUALS(result.second, 10);

          result = cache.getx(8);
          CXXTOOLS_UNIT_ASSERT(result.first);
          CXXTOOLS_UNIT_ASSERT_EQUALS(result.second, 80);

          cache.put_top(11, 110);
          cache.put_top(12, 120);
          cache.put_top(13, 130);
          cache.put_top(14, 140);

          result = cache.getx(10);
          CXXTOOLS_UNIT_ASSERT(!result.first);

          result = cache.getx(11);
          CXXTOOLS_UNIT_ASSERT(result.first);
          CXXTOOLS_UNIT_ASSERT_EQUALS(result.second, 110);
        }

};

cxxtools::unit::RegisterTest<CacheTest> register_CacheTest;
