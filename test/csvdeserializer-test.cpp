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
#include "cxxtools/csvdeserializer.h"
#include "cxxtools/csv.h"
#include "cxxtools/log.h"

//log_define("cxxtools.test.csvdeserializer")

namespace
{
    struct TestObject
    {
        int intValue;
        std::string stringValue;
        double doubleValue;
    };

    void operator>>= (const cxxtools::SerializationInfo& si, TestObject& obj)
    {
        si.getMember("intValue") >>= obj.intValue;
        si.getMember("stringValue") >>= obj.stringValue;
        si.getMember("doubleValue") >>= obj.doubleValue;
    }

}

class CsvDeserializerTest : public cxxtools::unit::TestSuite
{
    public:
        CsvDeserializerTest()
            : cxxtools::unit::TestSuite("csvdeserializer")
        {
            registerMethod("testVectorVector", *this, &CsvDeserializerTest::testVectorVector);
            registerMethod("testVectorVectorNoTitle", *this, &CsvDeserializerTest::testVectorVectorNoTitle);
            registerMethod("testIntVector", *this, &CsvDeserializerTest::testIntVector);
            registerMethod("testObjectVector", *this, &CsvDeserializerTest::testObjectVector);
            registerMethod("testMissigColumn", *this, &CsvDeserializerTest::testMissingColumn);
            registerMethod("testTooManyColumns", *this, &CsvDeserializerTest::testTooManyColumns);
            registerMethod("testCr", *this, &CsvDeserializerTest::testCr);
            registerMethod("testEmptyLines", *this, &CsvDeserializerTest::testEmptyLines);
            registerMethod("testSingleColumn", *this, &CsvDeserializerTest::testSingleColumn);
            registerMethod("testSetDelimiter", *this, &CsvDeserializerTest::testSetDelimiter);
            registerMethod("testQuotedTitle", *this, &CsvDeserializerTest::testQuotedTitle);
            registerMethod("testFailDecoding", *this, &CsvDeserializerTest::testFailDecoding);
            registerMethod("testIStream", *this, &CsvDeserializerTest::testIStream);
        }

        void testVectorVector()
        {
            std::vector<std::vector<std::string> > data;
            std::istringstream in(
                "A|B|C\n"
                "Hello|World|\n"
                "34|67|\"23\"\n"
                "col1|'col2'|col3\n");

            cxxtools::CsvDeserializer deserializer(in);
            deserializer.deserialize(data);

            CXXTOOLS_UNIT_ASSERT_EQUALS(data.size(), 3);
            CXXTOOLS_UNIT_ASSERT_EQUALS(data[0].size(), 3);
            CXXTOOLS_UNIT_ASSERT_EQUALS(data[1].size(), 3);
            CXXTOOLS_UNIT_ASSERT_EQUALS(data[2].size(), 3);
            CXXTOOLS_UNIT_ASSERT_EQUALS(data[0][0], "Hello");
            CXXTOOLS_UNIT_ASSERT_EQUALS(data[0][1], "World");
            CXXTOOLS_UNIT_ASSERT_EQUALS(data[1][0], "34");
            CXXTOOLS_UNIT_ASSERT_EQUALS(data[1][1], "67");
            CXXTOOLS_UNIT_ASSERT_EQUALS(data[1][2], "23");
            CXXTOOLS_UNIT_ASSERT_EQUALS(data[2][0], "col1");
            CXXTOOLS_UNIT_ASSERT_EQUALS(data[2][1], "col2");
            CXXTOOLS_UNIT_ASSERT_EQUALS(data[2][2], "col3");
        }

        void testVectorVectorNoTitle()
        {
            std::vector<std::vector<std::string> > data;
            std::istringstream in(
                "Hello|World|\n"
                "34|67|\"23\"\n"
                "col1|'col2'|col3\n");

            cxxtools::CsvDeserializer deserializer(in);
            deserializer.readTitle(false);
            deserializer.delimiter('|');
            deserializer.deserialize(data);

            CXXTOOLS_UNIT_ASSERT_EQUALS(data.size(), 3);
            CXXTOOLS_UNIT_ASSERT_EQUALS(data[0].size(), 3);
            CXXTOOLS_UNIT_ASSERT_EQUALS(data[1].size(), 3);
            CXXTOOLS_UNIT_ASSERT_EQUALS(data[2].size(), 3);
            CXXTOOLS_UNIT_ASSERT_EQUALS(data[0][0], "Hello");
            CXXTOOLS_UNIT_ASSERT_EQUALS(data[0][1], "World");
            CXXTOOLS_UNIT_ASSERT_EQUALS(data[1][0], "34");
            CXXTOOLS_UNIT_ASSERT_EQUALS(data[1][1], "67");
            CXXTOOLS_UNIT_ASSERT_EQUALS(data[1][2], "23");
            CXXTOOLS_UNIT_ASSERT_EQUALS(data[2][0], "col1");
            CXXTOOLS_UNIT_ASSERT_EQUALS(data[2][1], "col2");
            CXXTOOLS_UNIT_ASSERT_EQUALS(data[2][2], "col3");
        }

        void testIntVector()
        {
            std::vector<std::vector<int> > data;
            std::istringstream in(
                "A|B|C\n"
                "12|'23'|0\n"
                "34|67|\"23\"");

            cxxtools::CsvDeserializer deserializer(in);
            deserializer.deserialize(data);

            CXXTOOLS_UNIT_ASSERT_EQUALS(data.size(), 2);
            CXXTOOLS_UNIT_ASSERT_EQUALS(data[0].size(), 3);
            CXXTOOLS_UNIT_ASSERT_EQUALS(data[1].size(), 3);
            CXXTOOLS_UNIT_ASSERT_EQUALS(data[0][0], 12);
            CXXTOOLS_UNIT_ASSERT_EQUALS(data[0][1], 23);
            CXXTOOLS_UNIT_ASSERT_EQUALS(data[0][2], 0);
            CXXTOOLS_UNIT_ASSERT_EQUALS(data[1][0], 34);
            CXXTOOLS_UNIT_ASSERT_EQUALS(data[1][1], 67);
            CXXTOOLS_UNIT_ASSERT_EQUALS(data[1][2], 23);
        }

        void testObjectVector()
        {
            std::vector<TestObject> data;
            std::istringstream in(
                "intValue\tstringValue\tdoubleValue\n"
                "17\t'Hi'\t2.5\n"
                "-6\tFoo\t-1000");

            cxxtools::CsvDeserializer deserializer(in);
            deserializer.deserialize(data);

            CXXTOOLS_UNIT_ASSERT_EQUALS(data.size(), 2);
            CXXTOOLS_UNIT_ASSERT_EQUALS(data[0].intValue, 17);
            CXXTOOLS_UNIT_ASSERT_EQUALS(data[0].stringValue, "Hi");
            CXXTOOLS_UNIT_ASSERT_EQUALS(data[0].doubleValue, 2.5);
            CXXTOOLS_UNIT_ASSERT_EQUALS(data[1].intValue, -6);
            CXXTOOLS_UNIT_ASSERT_EQUALS(data[1].stringValue, "Foo");
            CXXTOOLS_UNIT_ASSERT_EQUALS(data[1].doubleValue, -1000);
        }

        void testMissingColumn()
        {
            std::vector<std::vector<std::string> > data;
            std::istringstream in(
                "A|B|C\n"
                "Hello|World\n"
                "34|67|\"23\"|someValue\n"
                "col1|'col2'|col3\n");

            cxxtools::CsvDeserializer deserializer(in);
            CXXTOOLS_UNIT_ASSERT_THROW(deserializer.deserialize(data), std::exception);
        }

        void testTooManyColumns()
        {
            std::vector<std::vector<std::string> > data;
            std::istringstream in(
                "A|B|C\n"
                "Hello|World|blah\n"
                "34|67|\"23\"|someValue\n"
                "col1|'col2'|col3|col4");

            cxxtools::CsvDeserializer deserializer(in);
            CXXTOOLS_UNIT_ASSERT_THROW(deserializer.deserialize(data), std::exception);
        }

        void testCr()
        {
            std::vector<std::vector<std::string> > data;
            std::istringstream in(
                "A|B|C\r\n"
                "Hello|World|\r"
                "34|67|\"23\"\n"
                "col1|'col2'|col3\r\n");

            cxxtools::CsvDeserializer deserializer(in);
            deserializer.deserialize(data);

            CXXTOOLS_UNIT_ASSERT_EQUALS(data.size(), 3);
            CXXTOOLS_UNIT_ASSERT_EQUALS(data[0].size(), 3);
            CXXTOOLS_UNIT_ASSERT_EQUALS(data[1].size(), 3);
            CXXTOOLS_UNIT_ASSERT_EQUALS(data[2].size(), 3);
            CXXTOOLS_UNIT_ASSERT_EQUALS(data[0][0], "Hello");
            CXXTOOLS_UNIT_ASSERT_EQUALS(data[0][1], "World");
            CXXTOOLS_UNIT_ASSERT_EQUALS(data[1][0], "34");
            CXXTOOLS_UNIT_ASSERT_EQUALS(data[1][1], "67");
            CXXTOOLS_UNIT_ASSERT_EQUALS(data[1][2], "23");
            CXXTOOLS_UNIT_ASSERT_EQUALS(data[2][0], "col1");
            CXXTOOLS_UNIT_ASSERT_EQUALS(data[2][1], "col2");
            CXXTOOLS_UNIT_ASSERT_EQUALS(data[2][2], "col3");
        }

        void testEmptyLines()
        {
            std::vector<std::vector<std::string> > data;
            std::istringstream in(
                "A;B;C\n"
                ";;\n"
                ";;\n");

            cxxtools::CsvDeserializer deserializer(in);
            deserializer.deserialize(data);

            CXXTOOLS_UNIT_ASSERT_EQUALS(data.size(), 2);
            CXXTOOLS_UNIT_ASSERT_EQUALS(data[0].size(), 3);
            CXXTOOLS_UNIT_ASSERT_EQUALS(data[1].size(), 3);
            CXXTOOLS_UNIT_ASSERT_EQUALS(data[0][0], "");
            CXXTOOLS_UNIT_ASSERT_EQUALS(data[0][1], "");
            CXXTOOLS_UNIT_ASSERT_EQUALS(data[0][2], "");
            CXXTOOLS_UNIT_ASSERT_EQUALS(data[1][0], "");
            CXXTOOLS_UNIT_ASSERT_EQUALS(data[1][1], "");
            CXXTOOLS_UNIT_ASSERT_EQUALS(data[1][2], "");
        }

        void testSingleColumn()
        {
            std::vector<std::vector<std::string> > data;
            std::istringstream in(
                "A\n"
                "1\n"
                "2\n"
                "\n");

            cxxtools::CsvDeserializer deserializer(in);
            deserializer.deserialize(data);

            CXXTOOLS_UNIT_ASSERT_EQUALS(data.size(), 3);
            CXXTOOLS_UNIT_ASSERT_EQUALS(data[0].size(), 1);
            CXXTOOLS_UNIT_ASSERT_EQUALS(data[1].size(), 1);
            CXXTOOLS_UNIT_ASSERT_EQUALS(data[2].size(), 1);
        }

        void testSetDelimiter()
        {
            std::vector<std::vector<int> > data;
            std::istringstream in(
                "A;|B|C\n"
                "12|'23'|0\n"
                "34|67|\"23\"");

            cxxtools::CsvDeserializer deserializer(in);
            deserializer.delimiter('|');
            deserializer.deserialize(data);

            CXXTOOLS_UNIT_ASSERT_EQUALS(data.size(), 2);
            CXXTOOLS_UNIT_ASSERT_EQUALS(data[0].size(), 3);
            CXXTOOLS_UNIT_ASSERT_EQUALS(data[1].size(), 3);
            CXXTOOLS_UNIT_ASSERT_EQUALS(data[0][0], 12);
            CXXTOOLS_UNIT_ASSERT_EQUALS(data[0][1], 23);
            CXXTOOLS_UNIT_ASSERT_EQUALS(data[0][2], 0);
            CXXTOOLS_UNIT_ASSERT_EQUALS(data[1][0], 34);
            CXXTOOLS_UNIT_ASSERT_EQUALS(data[1][1], 67);
            CXXTOOLS_UNIT_ASSERT_EQUALS(data[1][2], 23);
        }

        void testQuotedTitle()
        {
            std::vector<TestObject> data;
            std::istringstream in(
                "\"intValue\",'stringValue',\"doubleValue\"\n"
                "17,'Hi',2.5\n"
                "-6,Foo,-1000");

            cxxtools::CsvDeserializer deserializer(in);
            deserializer.deserialize(data);

            CXXTOOLS_UNIT_ASSERT_EQUALS(data.size(), 2);
            CXXTOOLS_UNIT_ASSERT_EQUALS(data[0].intValue, 17);
            CXXTOOLS_UNIT_ASSERT_EQUALS(data[0].stringValue, "Hi");
            CXXTOOLS_UNIT_ASSERT_EQUALS(data[0].doubleValue, 2.5);
            CXXTOOLS_UNIT_ASSERT_EQUALS(data[1].intValue, -6);
            CXXTOOLS_UNIT_ASSERT_EQUALS(data[1].stringValue, "Foo");
            CXXTOOLS_UNIT_ASSERT_EQUALS(data[1].doubleValue, -1000);
        }

        void testFailDecoding()
        {
            std::vector<std::vector<std::string> > data;
            std::istringstream in(
                "A\xff|B|C\n");

            cxxtools::CsvDeserializer deserializer(in);
            CXXTOOLS_UNIT_ASSERT_THROW(deserializer.deserialize(data), std::exception);
        }

        void testIStream()
        {
            std::istringstream in(
                "A|B\n"
                "Hello|World\n");

            std::vector<std::vector<std::string> > data;
            in >> cxxtools::Csv(data);

            CXXTOOLS_UNIT_ASSERT_EQUALS(data.size(), 1);
            CXXTOOLS_UNIT_ASSERT_EQUALS(data[0].size(), 2);
            CXXTOOLS_UNIT_ASSERT_EQUALS(data[0][0], "Hello");
            CXXTOOLS_UNIT_ASSERT_EQUALS(data[0][1], "World");
        }

};

cxxtools::unit::RegisterTest<CsvDeserializerTest> register_CsvDeserializerTest;
