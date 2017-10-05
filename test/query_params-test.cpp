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
#include "cxxtools/serializationinfo.h"
#include "cxxtools/log.h"
#include <vector>
#include <iterator>

log_define("cxxtools.test.queryparams")

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
            registerMethod("testParseDoublePercent", *this, &QueryParamsTest::testParseDoublePercent);
            registerMethod("testCount", *this, &QueryParamsTest::testCount);
            registerMethod("testCombine", *this, &QueryParamsTest::testCombine);
            registerMethod("testSet", *this, &QueryParamsTest::testSet);
            registerMethod("testIterator", *this, &QueryParamsTest::testIterator);
            registerMethod("testGetUrl", *this, &QueryParamsTest::testGetUrl);
            registerMethod("testGetNames", *this, &QueryParamsTest::testGetNames);
            registerMethod("testDeserialization", *this, &QueryParamsTest::testDeserialization);
            registerMethod("testDeserializeSpecial", *this, &QueryParamsTest::testDeserializeSpecial);
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

        void testParseDoublePercent()
        {
            cxxtools::QueryParams q;
            q.parse_url("%%=%%%");
            CXXTOOLS_UNIT_ASSERT(q.has("%%"));
            CXXTOOLS_UNIT_ASSERT_EQUALS(q["%%"], "%%%");
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

        void testSet()
        {
            cxxtools::QueryParams q;

            q.set("a", "Hi");
            CXXTOOLS_UNIT_ASSERT_EQUALS(q.getUrl(), "a=Hi");

            q.set("b", "foo");
            CXXTOOLS_UNIT_ASSERT_EQUALS(q.getUrl(), "a=Hi&b=foo");

            q.set("a", "There");
            CXXTOOLS_UNIT_ASSERT_EQUALS(q.getUrl(), "b=foo&a=There");

            q.clear();
            CXXTOOLS_UNIT_ASSERT_EQUALS(q.getUrl(), "");

            q.add("a", "Hi");
            CXXTOOLS_UNIT_ASSERT_EQUALS(q.getUrl(), "a=Hi");

            q.add("b", "foo");
            CXXTOOLS_UNIT_ASSERT_EQUALS(q.getUrl(), "a=Hi&b=foo");

            q.add("a", "There");
            CXXTOOLS_UNIT_ASSERT_EQUALS(q.getUrl(), "a=Hi&b=foo&a=There");

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

        void testGetNames()
        {
            cxxtools::QueryParams q;
            q.add("p1", "value1");
            q.add("p2", "value2");
            q.add("value3");

            std::vector<std::string> names;
            q.getNames(std::back_inserter(names));
            CXXTOOLS_UNIT_ASSERT_EQUALS(names.size(), 3);
            CXXTOOLS_UNIT_ASSERT_EQUALS(names[0], "p1");
            CXXTOOLS_UNIT_ASSERT_EQUALS(names[1], "p2");
            CXXTOOLS_UNIT_ASSERT_EQUALS(names[2], "");
        }

        void testDeserialization();
        void testDeserializeSpecial();
};

cxxtools::unit::RegisterTest<QueryParamsTest> register_QueryParamsTest;

// helper classes for deserialization

namespace
{
    struct Columns
    {
        std::string name;
        std::string value;
    };

    struct TableQuery
    {
        bool draw;
        std::vector<Columns> columns;
        unsigned start;

        TableQuery() : draw(false), start(0) { }
    };

    void operator>>= (const cxxtools::SerializationInfo& si, Columns& column)
    {
        si.getMember("name") >>= column.name;
        si.getMember("value") >>= column.value;
    }

    void operator>>= (const cxxtools::SerializationInfo& si, TableQuery& query)
    {
        si.getMember("draw") >>= query.draw;
        si.getMember("columns") >>= query.columns;
        si.getMember("start") >>= query.start;
    }

}

void QueryParamsTest::testDeserialization()
{
    cxxtools::QueryParams qp;
    qp.add("draw", "1");
    qp.add("columns[0][name]", "somename");
    qp.add("columns[0][value]", "somevalue");
    qp.add("columns[1][name]", "othername");
    qp.add("columns[1][value]", "othervalue");
    qp.add("start", "42");

    cxxtools::SerializationInfo si;
    si <<= qp;
    CXXTOOLS_UNIT_ASSERT_EQUALS(si.memberCount(), 3);
    CXXTOOLS_UNIT_ASSERT(si.findMember("draw") != 0);
    CXXTOOLS_UNIT_ASSERT(si.findMember("columns") != 0);
    CXXTOOLS_UNIT_ASSERT(si.findMember("start") != 0);

    TableQuery query;
    si >>= query;
    CXXTOOLS_UNIT_ASSERT(query.draw);
    CXXTOOLS_UNIT_ASSERT_EQUALS(query.columns.size(), 2);
    CXXTOOLS_UNIT_ASSERT_EQUALS(query.columns[0].name, "somename");
    CXXTOOLS_UNIT_ASSERT_EQUALS(query.columns[0].value, "somevalue");
    CXXTOOLS_UNIT_ASSERT_EQUALS(query.columns[1].name, "othername");
    CXXTOOLS_UNIT_ASSERT_EQUALS(query.columns[1].value, "othervalue");
    CXXTOOLS_UNIT_ASSERT_EQUALS(query.start, 42);
}

void QueryParamsTest::testDeserializeSpecial()
{
    cxxtools::QueryParams qp;
    qp.add("+Key+[Key%2Bwith space+]", "value");
    qp.add("%2bKey+%20[0]", "value");

    cxxtools::SerializationInfo si;
    si <<= qp;
    CXXTOOLS_UNIT_ASSERT_EQUALS(si.memberCount(), 2);

    log_info(si);

    std::string value;
    CXXTOOLS_UNIT_ASSERT_NOTHROW(si.getMember(" Key ").getMember("Key+with space "));
    CXXTOOLS_UNIT_ASSERT_NOTHROW(si.getMember("+Key  ").getMember("0"));
}
