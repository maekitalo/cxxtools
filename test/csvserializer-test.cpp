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
#include "cxxtools/csvserializer.h"
#include "cxxtools/csv.h"
#include "cxxtools/log.h"

//log_define("cxxtools.test.csvserializer")

namespace
{
    struct TestObject
    {
        int intValue;
        std::string stringValue;
        double doubleValue;
        bool boolValue;
    };

    void operator<<= (cxxtools::SerializationInfo& si, const TestObject& obj)
    {
        si.addMember("intValue") <<= obj.intValue;
        si.addMember("stringValue") <<= obj.stringValue;
        si.addMember("doubleValue") <<= obj.doubleValue;
        si.addMember("boolValue") <<= obj.boolValue;
        si.setTypeName("TestObject");
    }

    struct ComplexObject
    {
        int intValue;
        TestObject object;
    };

    void operator<<= (cxxtools::SerializationInfo& si, const ComplexObject& obj)
    {
        si.addMember("intValue") <<= obj.intValue;
        si.addMember("object") <<= obj.object;
        si.setTypeName("ComplexObject");
    }

#if __cplusplus >= 201703L
    struct TestObjectWithOptionals
    {
        std::optional<int> intValue;
        std::optional<std::string> stringValue;
        std::optional<double> doubleValue;
        std::optional<bool> boolValue;
    };

    void operator<<= (cxxtools::SerializationInfo& si, const TestObjectWithOptionals& obj)
    {
        if (obj.intValue)
            si.addMember("intValue") <<= obj.intValue;
        if (obj.stringValue)
            si.addMember("stringValue") <<= obj.stringValue;
        if (obj.doubleValue)
            si.addMember("doubleValue") <<= obj.doubleValue;
        if (obj.boolValue)
            si.addMember("boolValue") <<= obj.boolValue;
        si.setTypeName("TestObject");
    }
#endif

}

class CsvSerializerTest : public cxxtools::unit::TestSuite
{
    public:
        CsvSerializerTest()
            : cxxtools::unit::TestSuite("csvserializer")
        {
            registerMethod("testVectorVector", *this, &CsvSerializerTest::testVectorVector);
            registerMethod("testObjectVector", *this, &CsvSerializerTest::testObjectVector);
#if __cplusplus >= 201703L
            registerMethod("testObjectVectorWithOptionals", *this, &CsvSerializerTest::testObjectVectorWithOptionals);
#endif
            registerMethod("testComplexObject", *this, &CsvSerializerTest::testComplexObject);
            registerMethod("testPartialObject", *this, &CsvSerializerTest::testPartialObject);
            registerMethod("testPartialComplexObject", *this, &CsvSerializerTest::testPartialComplexObject);
            registerMethod("testCustomTitles", *this, &CsvSerializerTest::testCustomTitles);
            registerMethod("testEmptyCsvWithTitles", *this, &CsvSerializerTest::testEmptyCsvWithTitles);
            registerMethod("testCustomChars", *this, &CsvSerializerTest::testCustomChars);
            registerMethod("testMultichar", *this, &CsvSerializerTest::testMultichar);
            registerMethod("testOStream", *this, &CsvSerializerTest::testOStream);
            registerMethod("testLinefeeddata", *this, &CsvSerializerTest::testLinefeeddata);
            registerMethod("testSingleValue", *this, &CsvSerializerTest::testSingleValue);
            registerMethod("testSingleObject", *this, &CsvSerializerTest::testSingleObject);
            registerMethod("testPartialSingleObject", *this, &CsvSerializerTest::testSingleObject);
            registerMethod("testPartialSingleComplexObject", *this, &CsvSerializerTest::testPartialSingleComplexObject);
        }

        void testVectorVector()
        {
            std::vector<std::vector<std::string> > data;
            data.resize(2);
            data[0].push_back("Hello");
            data[0].push_back("World");
            data[1].push_back("34");
            data[1].push_back("67");

            std::ostringstream out;
            cxxtools::CsvSerializer serializer(out);
            serializer.serialize(data);

            CXXTOOLS_UNIT_ASSERT_EQUALS(out.str(),
                "Hello,World\n"
                "34,67\n");
        }

        void testObjectVector()
        {
            std::vector<TestObject> data;

            data.resize(2);
            data[0].intValue = 17;
            data[0].stringValue = "Hi";
            data[0].doubleValue = 7.5;
            data[0].boolValue = true;
            data[1].intValue = -2;
            data[1].stringValue = "Foo";
            data[1].doubleValue = -8;
            data[1].boolValue = false;

            std::ostringstream out;
            cxxtools::CsvSerializer serializer(out);
            serializer.serialize(data);

            CXXTOOLS_UNIT_ASSERT_EQUALS(out.str(),
                "intValue,stringValue,doubleValue,boolValue\n"
                "17,Hi,7.5,true\n"
                "-2,Foo,-8,false\n");
        }

#if __cplusplus >= 201703L
        void testObjectVectorWithOptionals()
        {
            std::vector<TestObjectWithOptionals> data;

            data.resize(2);
            data[0].intValue = 17;
            data[0].stringValue = "Hi";
            data[0].doubleValue = 7.5;
            data[0].boolValue = true;
            data[1].intValue = -2;
            data[1].stringValue = "Foo";

            std::ostringstream out;
            cxxtools::CsvSerializer serializer(out);
            serializer.serialize(data);

            CXXTOOLS_UNIT_ASSERT_EQUALS(out.str(),
                "intValue,stringValue,doubleValue,boolValue\n"
                "17,Hi,7.5,true\n"
                "-2,Foo,,\n");
        }
#endif

        void testComplexObject()
        {
            std::vector<ComplexObject> data(2);
            data[0].intValue = 17;
            data[0].object.intValue = 42;
            data[0].object.doubleValue = 5;
            data[0].object.boolValue = true;
            data[1].intValue = 18;
            data[1].object.intValue = 55;
            data[1].object.doubleValue = -1;
            data[1].object.boolValue = false;

            std::ostringstream out;
            cxxtools::CsvSerializer serializer(out);
            serializer.serialize(data);

            CXXTOOLS_UNIT_ASSERT_EQUALS(out.str(),
                "intValue,object.intValue,object.stringValue,object.doubleValue,object.boolValue\n"
                "17,42,,5,true\n"
                "18,55,,-1,false\n");
        }

        void testPartialObject()
        {
            std::vector<TestObject> data;

            data.resize(2);
            data[0].intValue = 17;
            data[0].stringValue = "Hi";
            data[0].doubleValue = 7.5;
            data[0].boolValue = true;
            data[1].intValue = -2;
            data[1].stringValue = "Foo";
            data[1].doubleValue = -8;
            data[1].boolValue = false;

            std::ostringstream out;
            cxxtools::CsvSerializer serializer(out);
            serializer.selectColumn("stringValue");
            serializer.selectColumn("intValue");
            serializer.serialize(data);

            CXXTOOLS_UNIT_ASSERT_EQUALS(out.str(),
                "stringValue,intValue\n"
                "Hi,17\n"
                "Foo,-2\n");
        }

        void testPartialComplexObject()
        {
            std::vector<ComplexObject> data(2);
            data[0].intValue = 17;
            data[0].object.intValue = 42;
            data[0].object.doubleValue = 5;
            data[0].object.boolValue = true;
            data[1].intValue = 18;
            data[1].object.intValue = 55;
            data[1].object.doubleValue = -1;
            data[1].object.boolValue = false;

            std::ostringstream out;
            cxxtools::CsvSerializer serializer(out);
            serializer.selectColumn("object.doubleValue");
            serializer.selectColumn("intValue");
            serializer.serialize(data);

            CXXTOOLS_UNIT_ASSERT_EQUALS(out.str(),
                "object.doubleValue,intValue\n"
                "5,17\n"
                "-1,18\n");
        }

        void testCustomTitles()
        {
            std::vector<TestObject> data;

            data.resize(2);
            data[0].intValue = 17;
            data[0].stringValue = "Hi";
            data[0].doubleValue = 7.5;
            data[0].boolValue = true;
            data[1].intValue = -2;
            data[1].stringValue = "Foo";
            data[1].doubleValue = -8;
            data[1].boolValue = false;

            std::ostringstream out;
            cxxtools::CsvSerializer serializer(out);
            serializer.selectColumn("stringValue", L"col1");
            serializer.selectColumn("intValue", L"col2");
            serializer.selectColumn("doubleValue", L"col3");
            serializer.selectColumn("boolValue", L"col4");
            serializer.serialize(data);

            CXXTOOLS_UNIT_ASSERT_EQUALS(out.str(),
                "col1,col2,col3,col4\n"
                "Hi,17,7.5,true\n"
                "Foo,-2,-8,false\n");
        }

        void testEmptyCsvWithTitles()
        {
            std::vector<TestObject> data;

            std::ostringstream out;
            cxxtools::CsvSerializer serializer(out);
            serializer.selectColumn("stringValue", L"col1");
            serializer.selectColumn("intValue", L"col2");
            serializer.selectColumn("doubleValue", L"col3");
            serializer.selectColumn("boolValue", L"col4");
            serializer.serialize(data);

            CXXTOOLS_UNIT_ASSERT_EQUALS(out.str(),
                "col1,col2,col3,col4\n");
        }

        void testCustomChars()
        {
            std::vector<std::vector<std::string> > data;
            data.resize(2);
            data[0].push_back("Hello");
            data[0].push_back("fWorld");
            data[1].push_back("34");
            data[1].push_back("67");

            std::ostringstream out;
            cxxtools::CsvSerializer serializer(out);
            serializer.delimiter('l');
            serializer.quote('f');
            serializer.lineEnding(L"Tab");
            serializer.serialize(data);

            CXXTOOLS_UNIT_ASSERT_EQUALS(out.str(),
                "fHelloflfffWorldfTab"
                "34l67Tab");
        }

        void testMultichar()
        {
            std::vector<std::vector<std::string> > data;
            data.resize(2);
            data[0].push_back("Hello");
            data[0].push_back("World");
            data[1].push_back("34");
            data[1].push_back("67");

            std::ostringstream out;
            cxxtools::CsvSerializer serializer(out);
            serializer.delimiter(L"foobar");
            serializer.lineEnding(L"\r\n");

            serializer.serialize(data);

            CXXTOOLS_UNIT_ASSERT_EQUALS(out.str(),
                "HellofoobarWorld\r\n"
                "34foobar67\r\n");
        }

        void testOStream()
        {
            std::vector<std::vector<std::string> > data(2);
            data[0].push_back("Hello");
            data[0].push_back("World");
            data[1].push_back("34");
            data[1].push_back("67");

            std::ostringstream out;
            out << cxxtools::Csv(data);

            CXXTOOLS_UNIT_ASSERT_EQUALS(out.str(),
                "Hello,World\n"
                "34,67\n");
        }

        void testLinefeeddata()
        {
            std::vector<std::vector<std::string> > data(2);
            data[0].push_back("Hello");
            data[0].push_back("World");
            data[1].push_back("foo\nbar");
            data[1].push_back("blub");

            std::ostringstream out;
            cxxtools::CsvSerializer serializer(out);
            serializer.serialize(data);

            CXXTOOLS_UNIT_ASSERT_EQUALS(out.str(),
                "Hello,World\n"
                "\"foo\nbar\",blub\n");
        }

        void testSingleValue()
        {
            std::ostringstream out;
            cxxtools::CsvSerializer serializer(out);
            serializer.serialize("Hello,World");

            CXXTOOLS_UNIT_ASSERT_EQUALS(out.str(),
                "\"Hello,World\"");
        }

        void testSingleObject()
        {
            TestObject data;

            data.intValue = 17;
            data.stringValue = "Hi";
            data.doubleValue = 7.5;
            data.boolValue = true;

            std::ostringstream out;
            cxxtools::CsvSerializer serializer(out);
            serializer.serialize(data);

            CXXTOOLS_UNIT_ASSERT_EQUALS(out.str(),
                "intValue,stringValue,doubleValue,boolValue\n"
                "17,Hi,7.5,true\n");
        }

        void testPartialSingleObject()
        {
            TestObject data;

            data.intValue = 17;
            data.stringValue = "Hi";
            data.doubleValue = 7.5;
            data.boolValue = true;

            std::ostringstream out;
            cxxtools::CsvSerializer serializer(out);
            serializer.selectColumn("stringValue", L"foo");
            serializer.serialize(data);

            CXXTOOLS_UNIT_ASSERT_EQUALS(out.str(),
                "foo\n"
                "Hi\n");
        }

        void testPartialSingleComplexObject()
        {
            ComplexObject data;
            data.intValue = 17;
            data.object.intValue = 42;
            data.object.doubleValue = 5;
            data.object.boolValue = true;

            std::ostringstream out;
            cxxtools::CsvSerializer serializer(out);
            serializer.selectColumn("object.doubleValue", L"foo");
            serializer.serialize(data);

            CXXTOOLS_UNIT_ASSERT_EQUALS(out.str(),
                "foo\n"
                "5\n");
        }
};

cxxtools::unit::RegisterTest<CsvSerializerTest> register_CsvSerializerTest;
