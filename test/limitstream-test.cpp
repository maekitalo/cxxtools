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

#include "cxxtools/limitstream.h"
#include "cxxtools/unit/testsuite.h"
#include "cxxtools/unit/registertest.h"
#include <sstream>

class LimitStreamTest : public cxxtools::unit::TestSuite
{
    public:
        LimitStreamTest()
        : cxxtools::unit::TestSuite("limitstream")
        {
            registerMethod("testIStream", *this, &LimitStreamTest::testIStream);
            registerMethod("testOStream", *this, &LimitStreamTest::testOStream);
        }

        void testIStream()
        {
            std::istringstream is("foobar");
            cxxtools::LimitIStream lis(is, 3);

            std::string s;
            lis >> s;
            CXXTOOLS_UNIT_ASSERT(lis.eof());
            CXXTOOLS_UNIT_ASSERT_EQUALS(s, "foo");

            lis.icount(5);
            lis.clear();
            lis >> s;
            CXXTOOLS_UNIT_ASSERT(lis.eof());
            CXXTOOLS_UNIT_ASSERT_EQUALS(lis.icount(), 2u);
            CXXTOOLS_UNIT_ASSERT_EQUALS(s, "bar");
        }

        void testOStream()
        {
            std::ostringstream os;
            cxxtools::LimitOStream los(os, 3);

            los << "foobar";
            CXXTOOLS_UNIT_ASSERT_EQUALS(os.str(), "foo");
            CXXTOOLS_UNIT_ASSERT_EQUALS(los.ocount(), 0u);

            los.ocount(5);
            los.clear();

            los << "bar";
            CXXTOOLS_UNIT_ASSERT_EQUALS(os.str(), "foobar");
            CXXTOOLS_UNIT_ASSERT_EQUALS(los.ocount(), 2u);

            los << "baz";
            CXXTOOLS_UNIT_ASSERT_EQUALS(os.str(), "foobarba");
            CXXTOOLS_UNIT_ASSERT_EQUALS(los.ocount(), 0u);
        }
};

cxxtools::unit::RegisterTest<LimitStreamTest> register_LimitStreamTest;
