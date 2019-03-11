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

#include "cxxtools/split.h"
#include "cxxtools/unit/testsuite.h"
#include "cxxtools/unit/registertest.h"
#include <iterator>

class SplitTest : public cxxtools::unit::TestSuite
{
  public:
    SplitTest()
    : cxxtools::unit::TestSuite("split")
    {
      registerMethod("splitString", *this, &SplitTest::splitString);
      registerMethod("emptySplit", *this, &SplitTest::emptySplit);
      registerMethod("lastEmpty", *this, &SplitTest::lastEmpty);
      registerMethod("charsetSplit", *this, &SplitTest::charsetSplit);
      registerMethod("regexSplit", *this, &SplitTest::regexSplit);
      registerMethod("regexEmptySplit", *this, &SplitTest::regexEmptySplit);
    }

    void splitString()
    {
      std::vector<std::string> d;
      std::string t = "Hello:World:!";
      cxxtools::split(':', t, std::back_inserter(d));
      CXXTOOLS_UNIT_ASSERT_EQUALS(d.size(), 3);
      CXXTOOLS_UNIT_ASSERT_EQUALS(d[0], "Hello");
      CXXTOOLS_UNIT_ASSERT_EQUALS(d[1], "World");
      CXXTOOLS_UNIT_ASSERT_EQUALS(d[2], "!");
    }

    void emptySplit()
    {
      std::vector<std::string> d;
      std::string t;
      cxxtools::split(':', t, std::back_inserter(d));
      CXXTOOLS_UNIT_ASSERT_EQUALS(d.size(), 0);
    }

    void lastEmpty()
    {
      std::vector<std::string> d;
      std::string t = "Hello:World:";
      cxxtools::split(':', t, std::back_inserter(d));
      CXXTOOLS_UNIT_ASSERT_EQUALS(d.size(), 3);
      CXXTOOLS_UNIT_ASSERT_EQUALS(d[0], "Hello");
      CXXTOOLS_UNIT_ASSERT_EQUALS(d[1], "World");
      CXXTOOLS_UNIT_ASSERT_EQUALS(d[2], "");
    }

    void charsetSplit()
    {
      std::vector<std::string> d;
      std::string t = "Hello:World;!";
      cxxtools::split(";:,.", t, std::back_inserter(d));
      CXXTOOLS_UNIT_ASSERT_EQUALS(d.size(), 3);
      CXXTOOLS_UNIT_ASSERT_EQUALS(d[0], "Hello");
      CXXTOOLS_UNIT_ASSERT_EQUALS(d[1], "World");
      CXXTOOLS_UNIT_ASSERT_EQUALS(d[2], "!");
    }

    void regexSplit()
    {
      std::vector<std::string> d;
      std::string t = "Hello:World)!";
      cxxtools::split(cxxtools::Regex("[:)]"), t, std::back_inserter(d));
      CXXTOOLS_UNIT_ASSERT_EQUALS(d.size(), 3);
      CXXTOOLS_UNIT_ASSERT_EQUALS(d[0], "Hello");
      CXXTOOLS_UNIT_ASSERT_EQUALS(d[1], "World");
      CXXTOOLS_UNIT_ASSERT_EQUALS(d[2], "!");
    }

    void regexEmptySplit()
    {
      std::vector<std::string> d;
      std::string t;
      cxxtools::split(cxxtools::Regex("[:)]"), t, std::back_inserter(d));
      CXXTOOLS_UNIT_ASSERT_EQUALS(d.size(), 0);
    }

};

cxxtools::unit::RegisterTest<SplitTest> register_SplitTest;
