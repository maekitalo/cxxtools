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

#include "cxxtools/convert.h"
#include "cxxtools/unit/testsuite.h"
#include "cxxtools/unit/registertest.h"
#include "cxxtools/string.h"
#include <limits>
#include <string.h>

class ConvertTest : public cxxtools::unit::TestSuite
{
    public:
        ConvertTest()
        : cxxtools::unit::TestSuite("convert")
        {
            registerMethod("successTest", *this, &ConvertTest::successTest);
            registerMethod("failTest", *this, &ConvertTest::failTest);
            registerMethod("nanTest", *this, &ConvertTest::nanTest);
            registerMethod("infTest", *this, &ConvertTest::infTest);
            registerMethod("emptyTest", *this, &ConvertTest::emptyTest);
        }

        void successTest()
        {
          std::string s(" -15 ");
          int n = 0;
          CXXTOOLS_UNIT_ASSERT_NOTHROW(n = cxxtools::convert<int>(s));
          CXXTOOLS_UNIT_ASSERT_EQUALS(n, -15);

          cxxtools::String S(L" -42 ");
          CXXTOOLS_UNIT_ASSERT_NOTHROW(n = cxxtools::convert<int>(S));
          CXXTOOLS_UNIT_ASSERT_EQUALS(n, -42);

        }

        void failTest()
        {
          std::string s(" -15 a");
          int n = 0;
          CXXTOOLS_UNIT_ASSERT_THROW(n = cxxtools::convert<int>(s), cxxtools::ConversionError);

          cxxtools::String S(L" -42 a");
          CXXTOOLS_UNIT_ASSERT_THROW(n = cxxtools::convert<int>(S), cxxtools::ConversionError);

        }

        void nanTest()
        {
          // test string to number

          double d = cxxtools::convert<double>(std::string("NaN"));
          CXXTOOLS_UNIT_ASSERT(d != d);

          float f = cxxtools::convert<float>(std::string("NaN"));
          CXXTOOLS_UNIT_ASSERT(f != f);

          d = cxxtools::convert<double>(cxxtools::String(L"NaN"));
          CXXTOOLS_UNIT_ASSERT(d != d);

          f = cxxtools::convert<float>(cxxtools::String(L"NaN"));
          CXXTOOLS_UNIT_ASSERT(f != f);

          // test number to string

          std::string s = cxxtools::convert<std::string>(d);
          CXXTOOLS_UNIT_ASSERT_EQUALS(s, "nan");

          s = cxxtools::convert<std::string>(f);
          CXXTOOLS_UNIT_ASSERT_EQUALS(s, "nan");

          cxxtools::String ss = cxxtools::convert<cxxtools::String>(d);
          CXXTOOLS_UNIT_ASSERT_EQUALS(ss.narrow(), "nan");

          ss = cxxtools::convert<cxxtools::String>(f);
          CXXTOOLS_UNIT_ASSERT_EQUALS(ss.narrow(), "nan");

        }

        void infTest()
        {
          // test string to number

          double d = cxxtools::convert<double>(std::string("inf"));
          CXXTOOLS_UNIT_ASSERT_EQUALS(d, std::numeric_limits<double>::infinity());

          d = cxxtools::convert<double>(std::string("infinity"));
          CXXTOOLS_UNIT_ASSERT_EQUALS(d, std::numeric_limits<double>::infinity());

          float f = cxxtools::convert<float>(std::string("inf"));
          CXXTOOLS_UNIT_ASSERT_EQUALS(d, std::numeric_limits<float>::infinity());

          d = cxxtools::convert<double>(cxxtools::String(L"iNf"));
          CXXTOOLS_UNIT_ASSERT_EQUALS(d, std::numeric_limits<double>::infinity());

          f = cxxtools::convert<float>(cxxtools::String(L"inF"));
          CXXTOOLS_UNIT_ASSERT_EQUALS(d, std::numeric_limits<float>::infinity());

          // test number to string

          std::string s = cxxtools::convert<std::string>(d);
          CXXTOOLS_UNIT_ASSERT_EQUALS(s, "inf");

          s = cxxtools::convert<std::string>(f);
          CXXTOOLS_UNIT_ASSERT(strcasecmp(s.c_str(), "inf") == 0);

          cxxtools::String ss = cxxtools::convert<cxxtools::String>(d);
          CXXTOOLS_UNIT_ASSERT(strcasecmp(ss.narrow().c_str(), "inf") == 0);

          ss = cxxtools::convert<cxxtools::String>(f);
          CXXTOOLS_UNIT_ASSERT(strcasecmp(ss.narrow().c_str(), "inf") == 0);

          // negative inf

          // string to number
          d = cxxtools::convert<double>(std::string("-inf"));
          CXXTOOLS_UNIT_ASSERT_EQUALS(d, -std::numeric_limits<double>::infinity());

          f = cxxtools::convert<float>(std::string("-inf"));
          CXXTOOLS_UNIT_ASSERT_EQUALS(d, -std::numeric_limits<float>::infinity());

          d = cxxtools::convert<double>(cxxtools::String(L"-iNf"));
          CXXTOOLS_UNIT_ASSERT_EQUALS(d, -std::numeric_limits<double>::infinity());

          f = cxxtools::convert<float>(cxxtools::String(L"-INF"));
          CXXTOOLS_UNIT_ASSERT_EQUALS(d, -std::numeric_limits<float>::infinity());

          // test number to string
          s = cxxtools::convert<std::string>(d);
          CXXTOOLS_UNIT_ASSERT_EQUALS(s, "-inf");

          s = cxxtools::convert<std::string>(f);
          CXXTOOLS_UNIT_ASSERT(strcasecmp(s.c_str(), "-inf") == 0);

          ss = cxxtools::convert<cxxtools::String>(d);
          CXXTOOLS_UNIT_ASSERT(strcasecmp(ss.narrow().c_str(), "-inf") == 0);

          ss = cxxtools::convert<cxxtools::String>(f);
          CXXTOOLS_UNIT_ASSERT(strcasecmp(ss.narrow().c_str(), "-inf") == 0);

        }

        void emptyTest()
        {
          std::string emptyString;
          CXXTOOLS_UNIT_ASSERT_THROW(cxxtools::convert<int>(std::string()), cxxtools::ConversionError);
          CXXTOOLS_UNIT_ASSERT_THROW(cxxtools::convert<int>(cxxtools::String()), cxxtools::ConversionError);
          CXXTOOLS_UNIT_ASSERT_THROW(cxxtools::convert<unsigned>(std::string()), cxxtools::ConversionError);
          CXXTOOLS_UNIT_ASSERT_THROW(cxxtools::convert<unsigned>(cxxtools::String()), cxxtools::ConversionError);
          CXXTOOLS_UNIT_ASSERT_THROW(cxxtools::convert<double>(std::string()), cxxtools::ConversionError);
          CXXTOOLS_UNIT_ASSERT_THROW(cxxtools::convert<double>(cxxtools::String()), cxxtools::ConversionError);
        }

};

cxxtools::unit::RegisterTest<ConvertTest> register_ConvertTest;
