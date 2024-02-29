/*
 * Copyright (C) 2021 Tommi Maekitalo
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
#include "cxxtools/char.h"
#include <limits>

class CharTest : public cxxtools::unit::TestSuite
{
    public:
        CharTest()
        : cxxtools::unit::TestSuite("char")
        {
            cxxtools::unit::TestSuite::registerMethod( "testConstructor", *this, &CharTest::testConstructor );
            cxxtools::unit::TestSuite::registerMethod( "testNarrow", *this, &CharTest::testNarrow );
            cxxtools::unit::TestSuite::registerMethod( "testEof", *this, &CharTest::testEof );
        }

    private:
        void testConstructor();
        void testNarrow();
        void testEof();
};

cxxtools::unit::RegisterTest<CharTest> register_CharTest;

void CharTest::testConstructor()
{
    {
        cxxtools::Char ch(L'a');
        CXXTOOLS_UNIT_ASSERT_EQUALS(static_cast<int32_t>(ch.value()), static_cast<int32_t>(L'a'));
    }

    {
        cxxtools::Char ch(0x2764u);
        CXXTOOLS_UNIT_ASSERT_EQUALS(static_cast<uint32_t>(ch.value()), 0x2764u);
    }
}

void CharTest::testNarrow()
{
    {
        char ch = cxxtools::Char(0x2764u).narrow();
        CXXTOOLS_UNIT_ASSERT_EQUALS(ch, '?');
    }

    {
        char ch = cxxtools::Char(0x2764u).narrow('x');
        CXXTOOLS_UNIT_ASSERT_EQUALS(ch, 'x');
    }

    {
        char ch = cxxtools::Char(L'z').narrow();
        CXXTOOLS_UNIT_ASSERT_EQUALS(ch, 'z');
    }
}

void CharTest::testEof()
{
    auto eof = std::char_traits<cxxtools::Char>::eof();

    cxxtools::Char e(eof);
    CXXTOOLS_UNIT_ASSERT(e.value() != eof);

    cxxtools::Char min(std::numeric_limits<cxxtools::Char::value_type>::min());
    cxxtools::Char max(std::numeric_limits<cxxtools::Char::value_type>::max());

    CXXTOOLS_UNIT_ASSERT(!std::char_traits<cxxtools::Char>::eq_int_type(min, eof));
    CXXTOOLS_UNIT_ASSERT(!std::char_traits<cxxtools::Char>::eq_int_type(max, eof));
    CXXTOOLS_UNIT_ASSERT(!std::char_traits<cxxtools::Char>::eq_int_type(cxxtools::Char(L'\0').value(), eof));

    /*
    for (cxxtools::Char::value_type v = std::numeric_limits<cxxtools::Char::value_type>::min();
        v < std::numeric_limits<cxxtools::Char::value_type>::max();
        ++v)
    {
        CXXTOOLS_UNIT_ASSERT(!std::char_traits<cxxtools::Char>::eq_int_type(v, eof));
    }
    */
}
