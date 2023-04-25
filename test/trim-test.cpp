/*
 * Copyright (C) 2010 Tommi Maekitalo
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

#include "cxxtools/trim.h"
#include "cxxtools/unit/testsuite.h"
#include "cxxtools/unit/registertest.h"
#include "cxxtools/string.h"

static const std::string ws = " \t";
static const cxxtools::String wsu = L" \t";

class TrimTest : public cxxtools::unit::TestSuite
{
    public:
        TrimTest()
        : cxxtools::unit::TestSuite("trim")
        {
            registerMethod("ltrimTest", *this, &TrimTest::ltrimTest);
            registerMethod("rtrimTest", *this, &TrimTest::rtrimTest);
            registerMethod("trimTest", *this, &TrimTest::trimTest);
            registerMethod("trimTestU", *this, &TrimTest::trimTestU);
            registerMethod("ltrimiTest", *this, &TrimTest::ltrimiTest);
            registerMethod("rtrimiTest", *this, &TrimTest::rtrimiTest);
            registerMethod("trimiTest", *this, &TrimTest::trimiTest);
        }

        void ltrimTest()
        {
            CXXTOOLS_UNIT_ASSERT_EQUALS(cxxtools::ltrim(std::string("foo bar")), "foo bar");
            CXXTOOLS_UNIT_ASSERT_EQUALS(cxxtools::ltrim(std::string(" \t foo bar ")), "foo bar ");
            CXXTOOLS_UNIT_ASSERT_EQUALS(cxxtools::ltrim(std::string(" \t\n foo bar ")), "foo bar ");
            CXXTOOLS_UNIT_ASSERT_EQUALS(cxxtools::ltrim(std::string(" \t\n foo bar "), ws), "\n foo bar ");
            CXXTOOLS_UNIT_ASSERT_EQUALS(cxxtools::ltrim(std::string(" \t ")), "");
        }

        void rtrimTest()
        {
            CXXTOOLS_UNIT_ASSERT_EQUALS(cxxtools::rtrim(std::string("foo bar")), "foo bar");
            CXXTOOLS_UNIT_ASSERT_EQUALS(cxxtools::rtrim(std::string(" \t foo bar ")), " \t foo bar");
            CXXTOOLS_UNIT_ASSERT_EQUALS(cxxtools::rtrim(std::string(" \t\n foo bar \n ")), " \t\n foo bar");
            CXXTOOLS_UNIT_ASSERT_EQUALS(cxxtools::rtrim(std::string(" \t\n foo bar \n "), ws), " \t\n foo bar \n");
            CXXTOOLS_UNIT_ASSERT_EQUALS(cxxtools::rtrim(std::string(" \t ")), "");
        }

        void trimTest()
        {
            CXXTOOLS_UNIT_ASSERT_EQUALS(cxxtools::trim(std::string("foo bar")), "foo bar");
            CXXTOOLS_UNIT_ASSERT_EQUALS(cxxtools::trim(std::string(" \t foo bar ")), "foo bar");
            CXXTOOLS_UNIT_ASSERT_EQUALS(cxxtools::trim(std::string(" \t\n foo bar \n ")), "foo bar");
            CXXTOOLS_UNIT_ASSERT_EQUALS(cxxtools::trim(std::string(" \t\n foo bar \n "), ws), "\n foo bar \n");
            CXXTOOLS_UNIT_ASSERT_EQUALS(cxxtools::trim(std::string(" \t ")), "");
        }

        void trimTestU()
        {
            CXXTOOLS_UNIT_ASSERT(cxxtools::trim(cxxtools::String("foo bar")) == L"foo bar");
            CXXTOOLS_UNIT_ASSERT(cxxtools::trim(cxxtools::String(" \t foo bar ")) == L"foo bar");
            CXXTOOLS_UNIT_ASSERT(cxxtools::trim(cxxtools::String(" \t\n foo bar \n ")) == L"foo bar");
            CXXTOOLS_UNIT_ASSERT(cxxtools::trim(cxxtools::String(" \t\n foo bar \n "), wsu) == L"\n foo bar \n");
            CXXTOOLS_UNIT_ASSERT(cxxtools::trim(cxxtools::String(" \t ")) == L"");
        }

        void ltrimiTest()
        {
            std::string s;

            s = "foo bar";
            cxxtools::ltrimi(s);
            CXXTOOLS_UNIT_ASSERT_EQUALS(s, "foo bar");

            s = " \t foo bar ";
            cxxtools::ltrimi(s);
            CXXTOOLS_UNIT_ASSERT_EQUALS(s, "foo bar ");

            s = " \t\n foo bar ";
            cxxtools::ltrimi(s);
            CXXTOOLS_UNIT_ASSERT_EQUALS(s, "foo bar ");

            s = " \t\n foo bar ";
            cxxtools::ltrimi(s, ws);
            CXXTOOLS_UNIT_ASSERT_EQUALS(s, "\n foo bar ");

            s = " \t ";
            cxxtools::ltrimi(s);
            CXXTOOLS_UNIT_ASSERT_EQUALS(s, "");
        }

        void rtrimiTest()
        {
            std::string s;

            s = "foo bar";
            cxxtools::rtrimi(s);
            CXXTOOLS_UNIT_ASSERT_EQUALS(s, "foo bar");

            s = " \t foo bar ";
            cxxtools::rtrimi(s);
            CXXTOOLS_UNIT_ASSERT_EQUALS(s, " \t foo bar");

            s = " \t\n foo bar \n ";
            cxxtools::rtrimi(s);
            CXXTOOLS_UNIT_ASSERT_EQUALS(s, " \t\n foo bar");

            s = " \t\n foo bar \n ";
            cxxtools::rtrimi(s, ws);
            CXXTOOLS_UNIT_ASSERT_EQUALS(s, " \t\n foo bar \n");

            s = " \t ";
            cxxtools::rtrimi(s);
            CXXTOOLS_UNIT_ASSERT_EQUALS(s, "");
        }

        void trimiTest()
        {
            std::string s;

            s = "foo bar";
            cxxtools::trimi(s);
            CXXTOOLS_UNIT_ASSERT_EQUALS(s, "foo bar");

            s = " \t foo bar ";
            cxxtools::trimi(s);
            CXXTOOLS_UNIT_ASSERT_EQUALS(s, "foo bar");

            s = " \t\n foo bar \n ";
            cxxtools::trimi(s);
            CXXTOOLS_UNIT_ASSERT_EQUALS(s, "foo bar");

            s = " \t\n foo bar \n ";
            cxxtools::trimi(s, ws);
            CXXTOOLS_UNIT_ASSERT_EQUALS(s, "\n foo bar \n");

            s = " \t ";
            cxxtools::trimi(s);
            CXXTOOLS_UNIT_ASSERT_EQUALS(s, "");
        }

        void trimiTestU()
        {
            cxxtools::String s;

            s = L"foo bar";
            cxxtools::trimi(s);
            CXXTOOLS_UNIT_ASSERT_EQUALS(s, cxxtools::String(L"foo bar"));

            s = L" \t foo bar ";
            cxxtools::trimi(s);
            CXXTOOLS_UNIT_ASSERT_EQUALS(s, cxxtools::String(L"foo bar"));

            s = L" \t\n foo bar \n ";
            cxxtools::trimi(s);
            CXXTOOLS_UNIT_ASSERT_EQUALS(s, cxxtools::String(L"foo bar"));

            s = L" \t\n foo bar \n ";
            cxxtools::trimi(s, wsu);
            CXXTOOLS_UNIT_ASSERT_EQUALS(s, cxxtools::String(L"\n foo bar \n"));

            s = L" \t ";
            cxxtools::trimi(s);
            CXXTOOLS_UNIT_ASSERT_EQUALS(s, cxxtools::String(L""));
        }

};

cxxtools::unit::RegisterTest<TrimTest> register_TrimTest;
