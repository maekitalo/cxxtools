/*
 * Copyright (C) 2015 Tommi Maekitalo
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
#include "cxxtools/propertiesserializer.h"
#include "cxxtools/properties.h"

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
              boolValue(false)
              { }
    };

    inline void operator<<= (cxxtools::SerializationInfo& si, const TestObject& obj)
    {
        si.addMember("intValue") <<= obj.intValue;
        si.addMember("stringValue") <<= obj.stringValue;
        si.addMember("doubleValue") <<= obj.doubleValue;
        si.addMember("boolValue") <<= obj.boolValue;
        si.addMember("nullValue");
    }

    // test object for plain key test
    struct PlainKeyObject
    {
        int a;
        int b;
        int c;
        int d;
    };

    inline void operator<<= (cxxtools::SerializationInfo& si, const PlainKeyObject& obj)
    {
        si.addMember("a a") <<= obj.a;
        si.addMember("1b") <<= obj.b;
        si.addMember("c-c") <<= obj.c;
        si.addMember("ddd") <<= obj.d;
    }

    struct PropertiesData
    {
        int a;
        std::string propertiesData;
    };

    void operator<<= (cxxtools::SerializationInfo& si, const PropertiesData& j)
    {
      si.addMember("a") <<= j.a;
      cxxtools::SerializationInfo& jsi = si.addMember("propertiesData");
      jsi <<= j.propertiesData;
      jsi.setTypeName("properties");
    }

}

class PropertiesSerializerTest : public cxxtools::unit::TestSuite
{
    public:
        PropertiesSerializerTest()
            : cxxtools::unit::TestSuite("propertiesserializer")
        {
            registerMethod("testInt", *this, &PropertiesSerializerTest::testInt);
            registerMethod("testObject", *this, &PropertiesSerializerTest::testObject);
            registerMethod("testArray", *this, &PropertiesSerializerTest::testArray);
            registerMethod("testEmptyArrays", *this, &PropertiesSerializerTest::testEmptyArrays);
            registerMethod("testString", *this, &PropertiesSerializerTest::testString);
            registerMethod("testPlainInt", *this, &PropertiesSerializerTest::testPlainInt);
            registerMethod("testPlainObject", *this, &PropertiesSerializerTest::testPlainObject);
            registerMethod("testPlainArray", *this, &PropertiesSerializerTest::testPlainArray);
            registerMethod("testPlainString", *this, &PropertiesSerializerTest::testPlainString);
            registerMethod("testMultipleObjects", *this, &PropertiesSerializerTest::testMultipleObjects);
            registerMethod("testPlainEmpty", *this, &PropertiesSerializerTest::testPlainEmpty);
            registerMethod("testEasyProperties", *this, &PropertiesSerializerTest::testEasyProperties);
            registerMethod("testNoSizes", *this, &PropertiesSerializerTest::testNoSizes);
        }

        void testInt()
        {
            std::ostringstream out;
            cxxtools::PropertiesSerializer serializer(out);
            serializer.serialize(-4711, "value").finish();

            CXXTOOLS_UNIT_ASSERT_EQUALS(out.str(), "value = -4711\n");
        }

        void testObject()
        {
            TestObject data;
            data.intValue = 17;
            data.stringValue = "foobar";
            data.doubleValue = 1.5;
            data.boolValue = false;

            std::ostringstream out;
            cxxtools::PropertiesSerializer serializer(out);
            serializer.serialize(data, "testObject").finish();

            CXXTOOLS_UNIT_ASSERT_EQUALS(out.str(),
                "# object testObject.size = 5\n"
                "testObject.intValue = 17\n"
                "testObject.stringValue = foobar\n"
                "testObject.doubleValue = 1.5\n"
                "testObject.boolValue = false\n"
                "testObject.nullValue =\n");
        }

        void testArray()
        {
            std::vector<int> data;
            data.push_back(3);
            data.push_back(4);
            data.push_back(-33);

            std::ostringstream out;
            cxxtools::PropertiesSerializer serializer(out);
            serializer.serialize(data, "array").finish();

            CXXTOOLS_UNIT_ASSERT_EQUALS(out.str(),
                "# array array.size = 3\n"
                "array.0 = 3\n"
                "array.1 = 4\n"
                "array.2 = -33\n");
        }

        void testEmptyArrays()
        {
            std::vector< std::vector<int> > data;
            data.resize(2);

            std::ostringstream out;
            cxxtools::PropertiesSerializer serializer(out);
            serializer.serialize(data).finish();

            CXXTOOLS_UNIT_ASSERT_EQUALS(out.str(),
                "# array size = 2\n"
                "# array 0.size = 0\n"
                "# array 1.size = 0\n");
        }

        void testString()
        {
            cxxtools::String data;
            data = L"hi\xc3a4\xc3b6\xc3bc";

            std::ostringstream out;
            cxxtools::PropertiesSerializer serializer(out);
            serializer.serialize(data, "str").finish();

            CXXTOOLS_UNIT_ASSERT_EQUALS(out.str(), "str = hi\\uc3a4\\uc3b6\\uc3bc\n");

        }

        void testPlainInt()
        {
            std::ostringstream out;
            cxxtools::PropertiesSerializer serializer(out);
            serializer.serialize(-4711).finish();

            CXXTOOLS_UNIT_ASSERT_EQUALS(out.str(), "-4711\n");
        }

        void testPlainObject()
        {
            TestObject data;
            data.intValue = 17;
            data.stringValue = "foobar";
            data.doubleValue = 1.5;
            data.boolValue = false;

            std::ostringstream out;
            cxxtools::PropertiesSerializer serializer(out);
            serializer.serialize(data).finish();

            CXXTOOLS_UNIT_ASSERT_EQUALS(out.str(),
                "# object size = 5\n"
                "intValue = 17\n"
                "stringValue = foobar\n"
                "doubleValue = 1.5\n"
                "boolValue = false\n"
                "nullValue =\n");
        }

        void testPlainArray()
        {
            std::vector<int> data;
            data.push_back(3);
            data.push_back(4);
            data.push_back(-33);

            std::ostringstream out;
            cxxtools::PropertiesSerializer serializer(out);
            serializer.serialize(data).finish();

            CXXTOOLS_UNIT_ASSERT_EQUALS(out.str(),
                "# array size = 3\n"
                "0 = 3\n"
                "1 = 4\n"
                "2 = -33\n");
        }

        void testPlainString()
        {
            cxxtools::String data;
            data = L"hi\xc3a4\xc3b6\xc3bc";

            std::ostringstream out;
            cxxtools::PropertiesSerializer serializer(out);
            serializer.serialize(data).finish();

            CXXTOOLS_UNIT_ASSERT_EQUALS(out.str(), "hi\\uc3a4\\uc3b6\\uc3bc\n");

        }

        void testMultipleObjects()
        {
            std::ostringstream out;
            cxxtools::PropertiesSerializer serializer(out);
            serializer.serialize("Hi", "a")
                      .serialize(42, "b")
                      .finish();

            CXXTOOLS_UNIT_ASSERT_EQUALS(out.str(), "a = Hi\nb = 42\n");
        }

        void testPlainEmpty()
        {
            std::ostringstream out;
            cxxtools::PropertiesSerializer serializer(out);
            serializer.finish();

            CXXTOOLS_UNIT_ASSERT_EQUALS(out.str(), "");
        }

        void testEasyProperties()
        {
            TestObject data;
            data.intValue = 17;
            data.stringValue = "foobar";
            data.doubleValue = 1.5;
            data.boolValue = false;

            std::ostringstream out;
            out << cxxtools::Properties(data);

            CXXTOOLS_UNIT_ASSERT_EQUALS(out.str(),
                "# object size = 5\n"
                "intValue = 17\n"
                "stringValue = foobar\n"
                "doubleValue = 1.5\n"
                "boolValue = false\n"
                "nullValue =\n");
        }

        void testNoSizes()
        {
            TestObject data;
            data.intValue = 17;
            data.stringValue = "foobar";
            data.doubleValue = 1.5;
            data.boolValue = false;

            std::ostringstream out;
            out << cxxtools::Properties(data).outputSize(false);

            CXXTOOLS_UNIT_ASSERT_EQUALS(out.str(),
                "intValue = 17\n"
                "stringValue = foobar\n"
                "doubleValue = 1.5\n"
                "boolValue = false\n"
                "nullValue =\n");
        }

};

cxxtools::unit::RegisterTest<PropertiesSerializerTest> register_PropertiesSerializerTest;
