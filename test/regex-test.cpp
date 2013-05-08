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

#include "cxxtools/unit/testsuite.h"
#include "cxxtools/unit/registertest.h"
#include "cxxtools/regex.h"

class RegexTest : public cxxtools::unit::TestSuite
{
    public:
        RegexTest()
            : cxxtools::unit::TestSuite("regex")
        {
            registerMethod("testRegex", *this, &RegexTest::testRegex);
            registerMethod("testSubexpression", *this, &RegexTest::testSubexpression);
            registerMethod("testEmptyRegex", *this, &RegexTest::testEmptyRegex);
        }

        void testRegex()
        {
            cxxtools::Regex r("^hel*o");
            CXXTOOLS_UNIT_ASSERT(r.match("hello world"));
            CXXTOOLS_UNIT_ASSERT(!r.match(" hello world"));
            CXXTOOLS_UNIT_ASSERT(r.match("hellllo world"));
            CXXTOOLS_UNIT_ASSERT(r.match("heo world"));
        }

        void testSubexpression()
        {
            cxxtools::Regex r("([0-9]+)\\.([0-9]+)");
            cxxtools::RegexSMatch s;
            CXXTOOLS_UNIT_ASSERT(r.match("hello 6.7 world", s));
            CXXTOOLS_UNIT_ASSERT_EQUALS(s.size(), 3);
            CXXTOOLS_UNIT_ASSERT_EQUALS(s[0], "6.7");
            CXXTOOLS_UNIT_ASSERT_EQUALS(s[1], "6");
            CXXTOOLS_UNIT_ASSERT_EQUALS(s[2], "7");
        }

        void testEmptyRegex()
        {
            cxxtools::Regex r("");
            CXXTOOLS_UNIT_ASSERT(r.match("hello world"));
            CXXTOOLS_UNIT_ASSERT(r.match(" hello world"));
            CXXTOOLS_UNIT_ASSERT(r.match("hellllo world"));
            CXXTOOLS_UNIT_ASSERT(r.match("heo world"));
        }

};

cxxtools::unit::RegisterTest<RegexTest> register_RegexTest;
