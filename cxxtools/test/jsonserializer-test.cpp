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
#include "cxxtools/jsonserializer.h"
#include "cxxtools/log.h"

log_define("cxxtools.test.jsonserializer")

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

    inline void operator<<= (cxxtools::SerializationInfo& si, const TestObject& obj)
    {
        si.addMember("intValue") <<= obj.intValue;
        si.addMember("stringValue") <<= obj.stringValue;
        si.addMember("doubleValue") <<= obj.doubleValue;
        si.addMember("boolValue") <<= obj.boolValue;
    }

}

class JsonSerializerTest : public cxxtools::unit::TestSuite
{
    public:
        JsonSerializerTest()
            : cxxtools::unit::TestSuite("jsonserializer")
        {
            registerMethod("testInt", *this, &JsonSerializerTest::testInt);
            registerMethod("testObject", *this, &JsonSerializerTest::testObject);
            registerMethod("testArray", *this, &JsonSerializerTest::testArray);
            registerMethod("testString", *this, &JsonSerializerTest::testString);
            registerMethod("testPlainInt", *this, &JsonSerializerTest::testPlainInt);
            registerMethod("testPlainObject", *this, &JsonSerializerTest::testPlainObject);
            registerMethod("testPlainArray", *this, &JsonSerializerTest::testPlainArray);
            registerMethod("testPlainString", *this, &JsonSerializerTest::testPlainString);
        }

        void testInt()
        {
            std::ostringstream out;
            cxxtools::JsonSerializer serializer(out);
            serializer.serialize(-4711, "value").finish();

            CXXTOOLS_UNIT_ASSERT_EQUALS(out.str(), "{\"value\":-4711}");
        }

        void testObject()
        {
            TestObject data;
            data.intValue = 17;
            data.stringValue = "foobar";
            data.doubleValue = 1.5;
            data.boolValue = false;

            std::ostringstream out;
            cxxtools::JsonSerializer serializer(out);
            serializer.serialize(data, "testObject").finish();

            CXXTOOLS_UNIT_ASSERT_EQUALS(out.str(), "{\"testObject\":{"
                "\"intValue\":17,"
                "\"stringValue\":\"foobar\","
                "\"doubleValue\":1.5,"
                "\"boolValue\":0"
                "}}");
        }

        void testArray()
        {
            std::vector<int> data;
            data.push_back(3);
            data.push_back(4);
            data.push_back(-33);

            std::ostringstream out;
            cxxtools::JsonSerializer serializer(out);
            serializer.serialize(data, "array").finish();

            CXXTOOLS_UNIT_ASSERT_EQUALS(out.str(), "{\"array\":[3,4,-33]}");
        }

        void testString()
        {
            cxxtools::String data;
            data = L"hi\xc3a4\xc3b6\xc3bc";

            std::ostringstream out;
            cxxtools::JsonSerializer serializer(out);
            serializer.serialize(data, "str").finish();

            CXXTOOLS_UNIT_ASSERT_EQUALS(out.str(), "{\"str\":\"hi\\uc3a4\\uc3b6\\uc3bc\"}");

        }

        void testPlainInt()
        {
            std::ostringstream out;
            cxxtools::JsonSerializer serializer(out);
            serializer.serialize(-4711).finish();

            CXXTOOLS_UNIT_ASSERT_EQUALS(out.str(), "-4711");
        }

        void testPlainObject()
        {
            TestObject data;
            data.intValue = 17;
            data.stringValue = "foobar";
            data.doubleValue = 1.5;
            data.boolValue = false;

            std::ostringstream out;
            cxxtools::JsonSerializer serializer(out);
            serializer.serialize(data).finish();

            CXXTOOLS_UNIT_ASSERT_EQUALS(out.str(), "{"
                "\"intValue\":17,"
                "\"stringValue\":\"foobar\","
                "\"doubleValue\":1.5,"
                "\"boolValue\":0"
                "}");
        }

        void testPlainArray()
        {
            std::vector<int> data;
            data.push_back(3);
            data.push_back(4);
            data.push_back(-33);

            std::ostringstream out;
            cxxtools::JsonSerializer serializer(out);
            serializer.serialize(data).finish();

            CXXTOOLS_UNIT_ASSERT_EQUALS(out.str(), "[3,4,-33]");
        }

        void testPlainString()
        {
            cxxtools::String data;
            data = L"hi\xc3a4\xc3b6\xc3bc";

            std::ostringstream out;
            cxxtools::JsonSerializer serializer(out);
            serializer.serialize(data).finish();

            CXXTOOLS_UNIT_ASSERT_EQUALS(out.str(), "\"hi\\uc3a4\\uc3b6\\uc3bc\"");

        }

};

cxxtools::unit::RegisterTest<JsonSerializerTest> register_JsonSerializerTest;
