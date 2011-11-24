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

#include "cxxtools/unit/testsuite.h"
#include "cxxtools/unit/registertest.h"
#include "cxxtools/jsondeserializer.h"
#include "cxxtools/log.h"

//log_define("cxxtools.test.jsondeserializer")
//
namespace
{
    struct TestObject
    {
        int intValue;
        std::string stringValue;
        double doubleValue;
        bool boolValue;

        TestObject()
            : intValue(0),
              doubleValue(0),
              boolValue(false)
              { }
    };

    inline void operator>>= (const cxxtools::SerializationInfo& si, TestObject& obj)
    {
        si.getMember("intValue") >>= obj.intValue;
        si.getMember("stringValue") >>= obj.stringValue;
        si.getMember("doubleValue") >>= obj.doubleValue;
        si.getMember("boolValue") >>= obj.boolValue;
    }

    struct TestObject2 : public TestObject
    {
        std::set<short> setValue;
        struct
        {
            unsigned n;
            cxxtools::String s;
        } structValue;
    };

    inline void operator>>= (const cxxtools::SerializationInfo& si, TestObject2& obj)
    {
        si >>= static_cast<TestObject&>(obj);
        si.getMember("setValue") >>= obj.setValue;
        const cxxtools::SerializationInfo& ssi = si.getMember("structValue");
        ssi.getMember("n") >>= obj.structValue.n;
        ssi.getMember("s") >>= obj.structValue.s;
    }

}

class JsonDeserializerTest : public cxxtools::unit::TestSuite
{
    public:
        JsonDeserializerTest()
            : cxxtools::unit::TestSuite("jsondeserializer")
        {
            registerMethod("testInt", *this, &JsonDeserializerTest::testInt);
            registerMethod("testObject", *this, &JsonDeserializerTest::testObject);
            registerMethod("testArray", *this, &JsonDeserializerTest::testArray);
            registerMethod("testStrings", *this, &JsonDeserializerTest::testStrings);
            registerMethod("testComplexObject", *this, &JsonDeserializerTest::testComplexObject);
        }

        void testInt()
        {
            int data = 0;
            std::istringstream in("-4711");

            cxxtools::JsonDeserializer deserializer(in);
            deserializer.deserialize(data);

            CXXTOOLS_UNIT_ASSERT_EQUALS(data, -4711);
        }

        void testObject()
        {
            TestObject data;

            std::istringstream in(" {"
                "\"intValue\": 17, "
                "\"stringValue\":  \"foo bar\t\","
                "\"doubleValue\": \"1000\", "
                "\"boolValue\"  :    true"
            "}");

            cxxtools::JsonDeserializer deserializer(in);
            deserializer.deserialize(data);

            CXXTOOLS_UNIT_ASSERT_EQUALS(data.intValue, 17);
            CXXTOOLS_UNIT_ASSERT_EQUALS(data.stringValue, "foo bar\t");
            CXXTOOLS_UNIT_ASSERT_EQUALS(data.doubleValue, 1000.0);
            CXXTOOLS_UNIT_ASSERT_EQUALS(data.boolValue, true);
        }

        void testArray()
        {
            std::vector<int> data;

            std::istringstream in(" [ 3,4, -3]");

            cxxtools::JsonDeserializer deserializer(in);
            deserializer.deserialize(data);

            CXXTOOLS_UNIT_ASSERT_EQUALS(data.size(), 3);
            CXXTOOLS_UNIT_ASSERT_EQUALS(data[0], 3);
            CXXTOOLS_UNIT_ASSERT_EQUALS(data[1], 4);
            CXXTOOLS_UNIT_ASSERT_EQUALS(data[2], -3);
        }

        void testStrings()
        {
            std::vector<cxxtools::String> data;

            std::istringstream in(" [ \"3\", \"\\t\\b\", \"\", \"\\u1e044\"]");

            cxxtools::JsonDeserializer deserializer(in);
            deserializer.deserialize(data);

            CXXTOOLS_UNIT_ASSERT_EQUALS(data.size(), 4);
            CXXTOOLS_UNIT_ASSERT_EQUALS(data[0], L"3");
            CXXTOOLS_UNIT_ASSERT_EQUALS(data[1], L"\t\b");
            CXXTOOLS_UNIT_ASSERT_EQUALS(data[2], L"");
            CXXTOOLS_UNIT_ASSERT_EQUALS(data[3].size(), 2);
            CXXTOOLS_UNIT_ASSERT_EQUALS(data[3][0].value(), 0x1e04);
            CXXTOOLS_UNIT_ASSERT_EQUALS(data[3][1], L'4');
        }

        void testComplexObject()
        {
            TestObject2 data;

            std::istringstream in(" {"
                "\"intValue\": 17, "
                "\"stringValue\":  \"foo bar\t\","
                "\"doubleValue\": \"1000\", "
                "\"boolValue\"  :    true,"
                "\"setValue\":[5,7,8],"
                "\"structValue\" : { \"n\":3,\"s\":\"sss\"}"
            "}");

            cxxtools::JsonDeserializer deserializer(in);
            deserializer.deserialize(data);

            CXXTOOLS_UNIT_ASSERT_EQUALS(data.intValue, 17);
            CXXTOOLS_UNIT_ASSERT_EQUALS(data.stringValue, "foo bar\t");
            CXXTOOLS_UNIT_ASSERT_EQUALS(data.doubleValue, 1000.0);
            CXXTOOLS_UNIT_ASSERT_EQUALS(data.boolValue, true);
            CXXTOOLS_UNIT_ASSERT_EQUALS(data.setValue.size(), 3);
            CXXTOOLS_UNIT_ASSERT(data.setValue.find(5) != data.setValue.end());
            CXXTOOLS_UNIT_ASSERT(data.setValue.find(7) != data.setValue.end());
            CXXTOOLS_UNIT_ASSERT(data.setValue.find(8) != data.setValue.end());
            CXXTOOLS_UNIT_ASSERT_EQUALS(data.structValue.n, 3);
            CXXTOOLS_UNIT_ASSERT_EQUALS(data.structValue.s, L"sss");
        }
};

cxxtools::unit::RegisterTest<JsonDeserializerTest> register_JsonDeserializerTest;
