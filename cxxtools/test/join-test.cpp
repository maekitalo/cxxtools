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

#include "cxxtools/join.h"
#include "cxxtools/unit/testsuite.h"
#include "cxxtools/unit/registertest.h"

class JoinTest : public cxxtools::unit::TestSuite
{
  public:
    JoinTest()
    : cxxtools::unit::TestSuite("join")
    {
      registerMethod("joinString", *this, &JoinTest::joinString);
      registerMethod("joinInt", *this, &JoinTest::joinInt);
      registerMethod("emptyJoin", *this, &JoinTest::emptyJoin);
    }

    void joinString()
    {
      std::vector<std::string> d;
      d.push_back("Hello");
      d.push_back("World");
      d.push_back("!");
      std::string result = cxxtools::join(d.begin(), d.end(), " ");
      CXXTOOLS_UNIT_ASSERT_EQUALS(result, "Hello World !");
    }

    void joinInt()
    {
      std::vector<int> d;
      d.push_back(4);
      d.push_back(17);
      d.push_back(-12);
      std::string result = cxxtools::join(d.begin(), d.end(), ", ");
      CXXTOOLS_UNIT_ASSERT_EQUALS(result, "4, 17, -12");
    }

    void emptyJoin()
    {
      std::vector<std::string> d;
      std::string result = cxxtools::join(d.begin(), d.end(), ", ");
      CXXTOOLS_UNIT_ASSERT_EQUALS(result, "");
    }

};

cxxtools::unit::RegisterTest<JoinTest> register_JoinTest;
