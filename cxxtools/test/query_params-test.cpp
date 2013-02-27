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
#include "cxxtools/query_params.h"

class QueryParamsTest : public cxxtools::unit::TestSuite
{
    public:
        QueryParamsTest()
            : cxxtools::unit::TestSuite("queryparams")
        {
            registerMethod("testQueryParams", *this, &QueryParamsTest::testQueryParams);
            registerMethod("testCopy", *this, &QueryParamsTest::testCopy);
            registerMethod("testParseUrl", *this, &QueryParamsTest::testParseUrl);
            registerMethod("testParseUrlSpecialChar", *this, &QueryParamsTest::testParseUrlSpecialChar);
            registerMethod("testCount", *this, &QueryParamsTest::testCount);
            registerMethod("testCombine", *this, &QueryParamsTest::testCombine);
            registerMethod("testIterator", *this, &QueryParamsTest::testIterator);
            registerMethod("testGetUrl", *this, &QueryParamsTest::testGetUrl);
        }

        void testQueryParams()
        {
            cxxtools::QueryParams q;
            q.add("p1", "value1");
            q.add("p2", "value2");
            q.add("value3");
            CXXTOOLS_UNIT_ASSERT(q.has("p1"));
            CXXTOOLS_UNIT_ASSERT(q.has("p2"));
            CXXTOOLS_UNIT_ASSERT(!q.has("p3"));
            CXXTOOLS_UNIT_ASSERT_EQUALS(q["p1"], "value1");
            CXXTOOLS_UNIT_ASSERT_EQUALS(q["p2"], "value2");
            CXXTOOLS_UNIT_ASSERT_EQUALS(q[0], "value3");
        }

        void testCopy()
        {
            cxxtools::QueryParams q;
            q.add("p1", "value1");
            q.add("p2", "value2");
            q.add("value3");
            cxxtools::QueryParams q2 = q;
            CXXTOOLS_UNIT_ASSERT(q2.has("p1"));
            CXXTOOLS_UNIT_ASSERT(q2.has("p2"));
            CXXTOOLS_UNIT_ASSERT(!q2.has("p3"));
            CXXTOOLS_UNIT_ASSERT_EQUALS(q2["p1"], "value1");
            CXXTOOLS_UNIT_ASSERT_EQUALS(q2["p2"], "value2");
            CXXTOOLS_UNIT_ASSERT_EQUALS(q2[0], "value3");
        }

        void testParseUrl()
        {
            cxxtools::QueryParams q;
            q.parse_url("p2=value2&value3&p1=value1");
            CXXTOOLS_UNIT_ASSERT(q.has("p1"));
            CXXTOOLS_UNIT_ASSERT(q.has("p2"));
            CXXTOOLS_UNIT_ASSERT(!q.has("p3"));
            CXXTOOLS_UNIT_ASSERT_EQUALS(q["p1"], "value1");
            CXXTOOLS_UNIT_ASSERT_EQUALS(q["p2"], "value2");
            CXXTOOLS_UNIT_ASSERT_EQUALS(q[0], "value3");
        }

        void testParseUrlSpecialChar()
        {
            cxxtools::QueryParams q;
            q.parse_url("p1=value+with%20spaces&m%a4kitalo=tommi+");
            CXXTOOLS_UNIT_ASSERT(q.has("p1"));
            CXXTOOLS_UNIT_ASSERT(q.has("m\xa4kitalo"));
            CXXTOOLS_UNIT_ASSERT_EQUALS(q["p1"], "value with spaces");
            CXXTOOLS_UNIT_ASSERT_EQUALS(q["m\xa4kitalo"], "tommi ");
        }

        void testCount()
        {
            cxxtools::QueryParams q;
            q.add("p1", "value1");
            q.add("p2", "value2");
            q.add("p2", "value3");
            q.add("value4");
            q.add("value5");
            CXXTOOLS_UNIT_ASSERT_EQUALS(q.paramcount(), 2);
            CXXTOOLS_UNIT_ASSERT_EQUALS(q.paramcount("p1"), 1);
            CXXTOOLS_UNIT_ASSERT_EQUALS(q.paramcount("p2"), 2);
        }

        void testCombine()
        {
            cxxtools::QueryParams q;
            cxxtools::QueryParams q2;
            q.add("p1", "value1");
            q2.add("p2", "value2");
            q2.add("value3");
            q.add(q2);
        }

        void testIterator()
        {
            cxxtools::QueryParams q;
            q.add("p1", "value1");
            q.add("p2", "value2");
            q.add("p2", "value3");
            q.add("value4");
            q.add("value5");

            cxxtools::QueryParams::const_iterator it;

            it = q.begin();
            CXXTOOLS_UNIT_ASSERT(it != q.end());
            CXXTOOLS_UNIT_ASSERT_EQUALS(*it, "value4");
            ++it;
            CXXTOOLS_UNIT_ASSERT(it != q.end());
            CXXTOOLS_UNIT_ASSERT_EQUALS(*it, "value5");
            ++it;
            CXXTOOLS_UNIT_ASSERT(it == q.end());

            it = q.begin("p1");
            CXXTOOLS_UNIT_ASSERT(it != q.end());
            CXXTOOLS_UNIT_ASSERT_EQUALS(*it, "value1");
            ++it;
            CXXTOOLS_UNIT_ASSERT(it == q.end());

            it = q.begin("p2");
            CXXTOOLS_UNIT_ASSERT(it != q.end());
            CXXTOOLS_UNIT_ASSERT_EQUALS(*it, "value2");
            ++it;
            CXXTOOLS_UNIT_ASSERT(it != q.end());
            CXXTOOLS_UNIT_ASSERT_EQUALS(*it, "value3");
            ++it;
            CXXTOOLS_UNIT_ASSERT(it == q.end());
        }

        void testGetUrl()
        {
            cxxtools::QueryParams q;
            q.add("first name", "Tommi");
            q.add("last name", "M\xa4kitalo");
            q.add("some value");

            std::string url = q.getUrl();
            CXXTOOLS_UNIT_ASSERT_EQUALS(url, "first+name=Tommi&last+name=M%A4kitalo&some+value");
        }
};

cxxtools::unit::RegisterTest<QueryParamsTest> register_QueryParamsTest;
