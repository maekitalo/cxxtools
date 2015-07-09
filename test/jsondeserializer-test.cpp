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
#include "cxxtools/json.h"
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
        bool nullValue;

        TestObject()
            : intValue(0),
              doubleValue(0),
              boolValue(false),
              nullValue(false)
              { }
    };

    inline void operator>>= (const cxxtools::SerializationInfo& si, TestObject& obj)
    {
        si.getMember("intValue") >>= obj.intValue;
        si.getMember("stringValue") >>= obj.stringValue;
        si.getMember("doubleValue") >>= obj.doubleValue;
        si.getMember("boolValue") >>= obj.boolValue;
        const cxxtools::SerializationInfo* p = si.findMember("nullValue");
        obj.nullValue = p != 0 && p->isNull();
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

    struct EmptyObject
    {
    };

    inline void operator>>= (const cxxtools::SerializationInfo& si, EmptyObject& obj)
    {
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
            registerMethod("testObjectPlainKeys", *this, &JsonDeserializerTest::testObjectPlainKeys);
            registerMethod("testEmptyObject", *this, &JsonDeserializerTest::testEmptyObject);
            registerMethod("testArray", *this, &JsonDeserializerTest::testArray);
            registerMethod("testEmptyArrays", *this, &JsonDeserializerTest::testEmptyArrays);
            registerMethod("testStrings", *this, &JsonDeserializerTest::testStrings);
            registerMethod("testUnicodeString", *this, &JsonDeserializerTest::testUnicodeString);
            registerMethod("testComplexObject", *this, &JsonDeserializerTest::testComplexObject);
            registerMethod("testCommentLine", *this, &JsonDeserializerTest::testCommentLine);
            registerMethod("testCommentMultiline", *this, &JsonDeserializerTest::testCommentMultiline);
            registerMethod("testMultipleObjectsT", *this, &JsonDeserializerTest::testMultipleObjectsT);
            registerMethod("testMultipleObjectsI", *this, &JsonDeserializerTest::testMultipleObjectsI);
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
                "\"boolValue\"  :    true,"
                "\"nullValue\"  :  null"
            "}");

            cxxtools::JsonDeserializer deserializer(in);
            deserializer.deserialize(data);

            CXXTOOLS_UNIT_ASSERT_EQUALS(data.intValue, 17);
            CXXTOOLS_UNIT_ASSERT_EQUALS(data.stringValue, "foo bar\t");
            CXXTOOLS_UNIT_ASSERT_EQUALS(data.doubleValue, 1000.0);
            CXXTOOLS_UNIT_ASSERT_EQUALS(data.boolValue, true);
            CXXTOOLS_UNIT_ASSERT_EQUALS(data.nullValue, true);
        }

        void testObjectPlainKeys()
        {
            TestObject data;

            std::istringstream in(" {"
                "intValue: 17, "
                "stringValue:  \"foo bar\t\","
                "doubleValue: \"1000\", "
                "boolValue  :    true,"
                "nullValue  :  null"
            "}");

            cxxtools::JsonDeserializer deserializer(in);
            deserializer.deserialize(data);

            CXXTOOLS_UNIT_ASSERT_EQUALS(data.intValue, 17);
            CXXTOOLS_UNIT_ASSERT_EQUALS(data.stringValue, "foo bar\t");
            CXXTOOLS_UNIT_ASSERT_EQUALS(data.doubleValue, 1000.0);
            CXXTOOLS_UNIT_ASSERT_EQUALS(data.boolValue, true);
            CXXTOOLS_UNIT_ASSERT_EQUALS(data.nullValue, true);
        }

        void testEmptyObject()
        {
            EmptyObject data;
            std::istringstream in("{}");

            cxxtools::JsonDeserializer deserializer(in);
            deserializer.deserialize(data);
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

        void testEmptyArrays()
        {
            std::vector<int> data;

            std::istringstream in("[[],[]]");

            cxxtools::JsonDeserializer deserializer(in);
            deserializer.deserialize(data);

            CXXTOOLS_UNIT_ASSERT_EQUALS(data.size(), 2);
            CXXTOOLS_UNIT_ASSERT_EQUALS(data[0], 0);
            CXXTOOLS_UNIT_ASSERT_EQUALS(data[1], 0);
        }

        void testStrings()
        {
            std::vector<cxxtools::String> data;

            std::istringstream in(" [ \"3\", \"\\t\\b\", \"\", \"\\u1e044\"]");

            cxxtools::JsonDeserializer deserializer(in);
            deserializer.deserialize(data);

            CXXTOOLS_UNIT_ASSERT_EQUALS(data.size(), 4);
            CXXTOOLS_UNIT_ASSERT_EQUALS(data[0], "3");
            CXXTOOLS_UNIT_ASSERT_EQUALS(data[1], "\t\b");
            CXXTOOLS_UNIT_ASSERT_EQUALS(data[2], "");
            CXXTOOLS_UNIT_ASSERT_EQUALS(data[3].size(), 2);
            CXXTOOLS_UNIT_ASSERT_EQUALS(data[3][0].value(), 0x1e04);
            CXXTOOLS_UNIT_ASSERT_EQUALS(data[3][1], '4');
        }

        void testUnicodeString()
        {
            cxxtools::String data;

            std::istringstream in("\"M\xc3\xa4kitalo\xf0\x90\xa4\x80\"");

            cxxtools::JsonDeserializer deserializer(in);
            deserializer.deserialize(data);

            CXXTOOLS_UNIT_ASSERT_EQUALS(data.size(), 9);
            CXXTOOLS_UNIT_ASSERT_EQUALS(data[1].value(), 0xe4);
            CXXTOOLS_UNIT_ASSERT_EQUALS(data[8].value(), 0x10900);
        }

        void testComplexObject()
        {
            TestObject2 data;

            std::istringstream in(" {"
                "\"intValue\": 17, "
                "\"stringValue\":  \"foo bar\t\","
                "\"doubleValue\": \"1000\", "
                "\"boolValue\"  :    true,"
                "\"nullValue\"  :  null,"
                "\"setValue\":[5,7,8],"
                "\"structValue\" : { \"n\":3,\"s\":\"sss\"}"
            "}");

            cxxtools::JsonDeserializer deserializer(in);
            deserializer.deserialize(data);

            CXXTOOLS_UNIT_ASSERT_EQUALS(data.intValue, 17);
            CXXTOOLS_UNIT_ASSERT_EQUALS(data.stringValue, "foo bar\t");
            CXXTOOLS_UNIT_ASSERT_EQUALS(data.doubleValue, 1000.0);
            CXXTOOLS_UNIT_ASSERT_EQUALS(data.boolValue, true);
            CXXTOOLS_UNIT_ASSERT_EQUALS(data.nullValue, true);
            CXXTOOLS_UNIT_ASSERT_EQUALS(data.setValue.size(), 3);
            CXXTOOLS_UNIT_ASSERT(data.setValue.find(5) != data.setValue.end());
            CXXTOOLS_UNIT_ASSERT(data.setValue.find(7) != data.setValue.end());
            CXXTOOLS_UNIT_ASSERT(data.setValue.find(8) != data.setValue.end());
            CXXTOOLS_UNIT_ASSERT_EQUALS(data.structValue.n, 3);
            CXXTOOLS_UNIT_ASSERT_EQUALS(data.structValue.s, "sss");
        }

        void testCommentLine()
        {
            TestObject data;

            std::istringstream in("// \n { //\n"
                "\"intValue\"//\n: //\n 17 //\n, "
                "\"stringValue\": //\n\"foo bar\t\"//\n,"
                "\"doubleValue\": \"1000\", "
                "\"boolValue\"  : //\n   true,"
                "\"nullValue\"  :  null"
            "//\n}");

            cxxtools::JsonDeserializer deserializer(in);
            deserializer.deserialize(data);

            CXXTOOLS_UNIT_ASSERT_EQUALS(data.intValue, 17);
            CXXTOOLS_UNIT_ASSERT_EQUALS(data.stringValue, "foo bar\t");
            CXXTOOLS_UNIT_ASSERT_EQUALS(data.doubleValue, 1000.0);
            CXXTOOLS_UNIT_ASSERT_EQUALS(data.boolValue, true);
            CXXTOOLS_UNIT_ASSERT_EQUALS(data.nullValue, true);

            {
                int data = 0;
                std::istringstream in("//\n-4711");

                cxxtools::JsonDeserializer deserializer(in);
                deserializer.deserialize(data);

                CXXTOOLS_UNIT_ASSERT_EQUALS(data, -4711);
            }
        }

        void testCommentMultiline()
        {
            TestObject data;

            std::istringstream in("/* */ { /* */"
                "\"intValue\"/* */: /* */ 17 /* */, "
                "\"stringValue\": /* */\"foo bar\t\"/* */,"
                "\"doubleValue\": \"1000\", "
                "\"boolValue\"  : /* \n */   true,"
                "\"nullValue\"  :  null"
            "/* */}");

            cxxtools::JsonDeserializer deserializer(in);
            deserializer.deserialize(data);

            CXXTOOLS_UNIT_ASSERT_EQUALS(data.intValue, 17);
            CXXTOOLS_UNIT_ASSERT_EQUALS(data.stringValue, "foo bar\t");
            CXXTOOLS_UNIT_ASSERT_EQUALS(data.doubleValue, 1000.0);
            CXXTOOLS_UNIT_ASSERT_EQUALS(data.boolValue, true);
            CXXTOOLS_UNIT_ASSERT_EQUALS(data.nullValue, true);
        }

        void testMultipleObjectsT()
        {
            std::vector<int> data;
            std::istringstream in("[3][4] [5]");
            cxxtools::TextIStream tin(in, new cxxtools::Utf8Codec());

            tin >> cxxtools::Json(data);
            CXXTOOLS_UNIT_ASSERT_EQUALS(data.size(), 1);
            CXXTOOLS_UNIT_ASSERT_EQUALS(data[0], 3);

            tin >> cxxtools::Json(data);
            CXXTOOLS_UNIT_ASSERT_EQUALS(data.size(), 1);
            CXXTOOLS_UNIT_ASSERT_EQUALS(data[0], 4);

            tin >> cxxtools::Json(data);
            CXXTOOLS_UNIT_ASSERT_EQUALS(data.size(), 1);
            CXXTOOLS_UNIT_ASSERT_EQUALS(data[0], 5);

        }

        void testMultipleObjectsI()
        {
            std::vector<int> data;
            std::istringstream in("[3][4] [5]");

            in >> cxxtools::Json(data);
            CXXTOOLS_UNIT_ASSERT_EQUALS(data.size(), 1);
            CXXTOOLS_UNIT_ASSERT_EQUALS(data[0], 3);

            in >> cxxtools::Json(data);
            CXXTOOLS_UNIT_ASSERT_EQUALS(data.size(), 1);
            CXXTOOLS_UNIT_ASSERT_EQUALS(data[0], 4);

            in >> cxxtools::Json(data);
            CXXTOOLS_UNIT_ASSERT_EQUALS(data.size(), 1);
            CXXTOOLS_UNIT_ASSERT_EQUALS(data[0], 5);

        }

};

cxxtools::unit::RegisterTest<JsonDeserializerTest> register_JsonDeserializerTest;
