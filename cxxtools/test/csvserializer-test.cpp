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
#include "cxxtools/log.h"

//log_define("cxxtools.test.csvserializer")

namespace
{
    struct TestObject
    {
        int intValue;
        std::string stringValue;
        double doubleValue;
    };

    void operator<<= (cxxtools::SerializationInfo& si, const TestObject& obj)
    {
        si.addMember("intValue") <<= obj.intValue;
        si.addMember("stringValue") <<= obj.stringValue;
        si.addMember("doubleValue") <<= obj.doubleValue;
        si.setTypeName("TestObject");
    }

}

class CsvSerializerTest : public cxxtools::unit::TestSuite
{
    public:
        CsvSerializerTest()
            : cxxtools::unit::TestSuite("csvserializer")
        {
            registerMethod("testVectorVector", *this, &CsvSerializerTest::testVectorVector);
            registerMethod("testObjectVector", *this, &CsvSerializerTest::testObjectVector);
            registerMethod("testPartialObject", *this, &CsvSerializerTest::testPartialObject);
            registerMethod("testCustomChars", *this, &CsvSerializerTest::testCustomChars);
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
            data[1].intValue = -2;
            data[1].stringValue = "Foo";
            data[1].doubleValue = -8;

            std::ostringstream out;
            cxxtools::CsvSerializer serializer(out);
            serializer.serialize(data);

            CXXTOOLS_UNIT_ASSERT_EQUALS(out.str(),
                "intValue,stringValue,doubleValue\n"
                "17,Hi,7.5\n"
                "-2,Foo,-8.0\n");
        }

        void testPartialObject()
        {
            std::vector<TestObject> data;

            data.resize(2);
            data[0].intValue = 17;
            data[0].stringValue = "Hi";
            data[0].doubleValue = 7.5;
            data[1].intValue = -2;
            data[1].stringValue = "Foo";
            data[1].doubleValue = -8;

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

};

cxxtools::unit::RegisterTest<CsvSerializerTest> register_CsvSerializerTest;
