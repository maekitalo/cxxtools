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

#include "cxxtools/pool.h"
#include "cxxtools/unit/testsuite.h"
#include "cxxtools/unit/registertest.h"

class PoolTest : public cxxtools::unit::TestSuite
{
    struct Object
    {
      static unsigned instCount;
      static unsigned ctorCount;

      Object()
      { ++instCount; ++ctorCount; }
      ~Object()
      { --instCount; }
    };

  public:
    PoolTest()
    : cxxtools::unit::TestSuite("pool")
    {
      registerMethod("poolTest", *this, &PoolTest::poolTest);
      registerMethod("maxspareTest", *this, &PoolTest::maxspareTest);
    }

    void setUp()
    {
      Object::instCount = 0;
      Object::ctorCount = 0;
    }

    void poolTest()
    {
      {
        typedef cxxtools::Pool<Object> PoolType;
        PoolType pool;

        {
          PoolType::Ptr p = pool.get();
          CXXTOOLS_UNIT_ASSERT_EQUALS(pool.size(), 0);
          CXXTOOLS_UNIT_ASSERT_EQUALS(Object::ctorCount, 1);
        }

        CXXTOOLS_UNIT_ASSERT_EQUALS(pool.size(), 1);

        {
          PoolType::Ptr p = pool.get();
          CXXTOOLS_UNIT_ASSERT_EQUALS(pool.size(), 0);
        }

        CXXTOOLS_UNIT_ASSERT_EQUALS(pool.size(), 1);
        CXXTOOLS_UNIT_ASSERT_EQUALS(Object::ctorCount, 1);
      }

      CXXTOOLS_UNIT_ASSERT_EQUALS(Object::instCount, 0);
    }

    void maxspareTest()
    {
      typedef cxxtools::Pool<Object> PoolType;
      PoolType pool(3);

      {
        std::vector<PoolType::Ptr> p;
        while (p.size() < 10)
          p.push_back(pool.get());
        CXXTOOLS_UNIT_ASSERT_EQUALS(pool.size(), 0);
        CXXTOOLS_UNIT_ASSERT_EQUALS(Object::ctorCount, 10);
        p.pop_back();
        CXXTOOLS_UNIT_ASSERT_EQUALS(pool.size(), 1);
        p.pop_back();
        CXXTOOLS_UNIT_ASSERT_EQUALS(pool.size(), 2);
      }

      CXXTOOLS_UNIT_ASSERT_EQUALS(pool.size(), 3);

      {
        PoolType::Ptr b = pool.get();
        CXXTOOLS_UNIT_ASSERT_EQUALS(pool.size(), 2);
      }
      CXXTOOLS_UNIT_ASSERT_EQUALS(pool.size(), 3);

      CXXTOOLS_UNIT_ASSERT_EQUALS(Object::ctorCount, 10);

      pool.drop(1);
      CXXTOOLS_UNIT_ASSERT_EQUALS(pool.size(), 1);
      pool.drop();
      CXXTOOLS_UNIT_ASSERT_EQUALS(pool.size(), 0);

      CXXTOOLS_UNIT_ASSERT_EQUALS(Object::instCount, 0);
    }
};

unsigned PoolTest::Object::instCount = 0;
unsigned PoolTest::Object::ctorCount = 0;

cxxtools::unit::RegisterTest<PoolTest> register_PoolTest;
