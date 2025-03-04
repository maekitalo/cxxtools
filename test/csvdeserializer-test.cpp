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

#if __cplusplus >= 201703L
#include <optional>
#endif

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

#if __cplusplus >= 201703L

    struct TestObjectOptional
    {
        std::optional<int> intValue;
        std::optional<double> doubleValue;
        std::optional<char> charValue;
    };

    void operator>>= (const cxxtools::SerializationInfo& si, TestObjectOptional& obj)
    {
        si.getMember("intValue") >>= obj.intValue;
        si.getMember("doubleValue") >>= obj.doubleValue;
        si.getMember("charValue") >>= obj.charValue;
    }

#endif

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
            registerMethod("testDoubleQuoteData", *this, &CsvDeserializerTest::testDoubleQuoteData);
            registerMethod("testFailDecoding", *this, &CsvDeserializerTest::testFailDecoding);
            registerMethod("testLinefeed", *this, &CsvDeserializerTest::testLinefeed);
            registerMethod("testUnicode", *this, &CsvDeserializerTest::testUnicode);
#if __cplusplus >= 201703L
            registerMethod("testOptional", *this, &CsvDeserializerTest::testOptional);
#endif
            registerMethod("testSkipEmptyLines", *this, &CsvDeserializerTest::testSkipEmptyLines);
            registerMethod("testNoSkipEmptyLines", *this, &CsvDeserializerTest::testNoSkipEmptyLines);
        }

        void testVectorVector()
        {
            std::vector<std::vector<std::string> > data;
            std::istringstream in(
                "A|B|C\n"
                "Hello|World|\n"
                "34|67|\"23\"\n"
                "col1|'col2'|col3\n");

            in >> cxxtools::Csv(data);

            CXXTOOLS_UNIT_ASSERT_EQUALS(data.size(), 3u);
            CXXTOOLS_UNIT_ASSERT_EQUALS(data[0].size(), 3u);
            CXXTOOLS_UNIT_ASSERT_EQUALS(data[1].size(), 3u);
            CXXTOOLS_UNIT_ASSERT_EQUALS(data[2].size(), 3u);
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

            in >> cxxtools::Csv(data).readTitle(false).delimiter('|');

            CXXTOOLS_UNIT_ASSERT_EQUALS(data.size(), 3u);
            CXXTOOLS_UNIT_ASSERT_EQUALS(data[0].size(), 3u);
            CXXTOOLS_UNIT_ASSERT_EQUALS(data[1].size(), 3u);
            CXXTOOLS_UNIT_ASSERT_EQUALS(data[2].size(), 3u);
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

            cxxtools::CsvDeserializer deserializer;
            deserializer.read(in);
            deserializer.deserialize(data);

            CXXTOOLS_UNIT_ASSERT_EQUALS(data.size(), 2u);
            CXXTOOLS_UNIT_ASSERT_EQUALS(data[0].size(), 3u);
            CXXTOOLS_UNIT_ASSERT_EQUALS(data[1].size(), 3u);
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

            cxxtools::CsvDeserializer deserializer;
            deserializer.read(in);
            deserializer.deserialize(data);

            CXXTOOLS_UNIT_ASSERT_EQUALS(data.size(), 2u);
            CXXTOOLS_UNIT_ASSERT_EQUALS(data[0].intValue, 17);
            CXXTOOLS_UNIT_ASSERT_EQUALS(data[0].stringValue, "Hi");
            CXXTOOLS_UNIT_ASSERT_EQUALS(data[0].doubleValue, 2.5);
            CXXTOOLS_UNIT_ASSERT_EQUALS(data[1].intValue, -6);
            CXXTOOLS_UNIT_ASSERT_EQUALS(data[1].stringValue, "Foo");
            CXXTOOLS_UNIT_ASSERT_EQUALS(data[1].doubleValue, -1000);
        }

        void testMissingColumn()
        {
            std::istringstream in(
                "A|B|C\n"
                "Hello|World\n"
                "34|67|\"23\"|someValue\n"
                "col1|'col2'|col3\n");

            cxxtools::CsvDeserializer deserializer;
            CXXTOOLS_UNIT_ASSERT_THROW(deserializer.read(in), std::exception);
        }

        void testTooManyColumns()
        {
            std::istringstream in(
                "A|B|C\n"
                "Hello|World|blah\n"
                "34|67|\"23\"|someValue\n"
                "col1|'col2'|col3|col4");

            cxxtools::CsvDeserializer deserializer;
            CXXTOOLS_UNIT_ASSERT_THROW(deserializer.read(in), std::exception);
        }

        void testCr()
        {
            std::vector<std::vector<std::string> > data;
            std::istringstream in(
                "A|B|C\r\n"
                "Hello|World|\r"
                "34|67|\"23\"\n"
                "col1|'col2'|col3\r\n");

            cxxtools::CsvDeserializer deserializer;
            deserializer.read(in);
            deserializer.deserialize(data);

            CXXTOOLS_UNIT_ASSERT_EQUALS(data.size(), 3u);
            CXXTOOLS_UNIT_ASSERT_EQUALS(data[0].size(), 3u);
            CXXTOOLS_UNIT_ASSERT_EQUALS(data[1].size(), 3u);
            CXXTOOLS_UNIT_ASSERT_EQUALS(data[2].size(), 3u);
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

            cxxtools::CsvDeserializer deserializer;
            deserializer.read(in);
            deserializer.deserialize(data);

            CXXTOOLS_UNIT_ASSERT_EQUALS(data.size(), 2u);
            CXXTOOLS_UNIT_ASSERT_EQUALS(data[0].size(), 3u);
            CXXTOOLS_UNIT_ASSERT_EQUALS(data[1].size(), 3u);
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

            cxxtools::CsvDeserializer deserializer;
            deserializer.read(in);
            deserializer.deserialize(data);

            CXXTOOLS_UNIT_ASSERT_EQUALS(data.size(), 3u);
            CXXTOOLS_UNIT_ASSERT_EQUALS(data[0].size(), 1u);
            CXXTOOLS_UNIT_ASSERT_EQUALS(data[1].size(), 1u);
            CXXTOOLS_UNIT_ASSERT_EQUALS(data[2].size(), 1u);
        }

        void testSetDelimiter()
        {
            std::vector<std::vector<int> > data;
            std::istringstream in(
                "A;|B|C\n"
                "12|'23'|0\n"
                "34|67|\"23\"");

            cxxtools::CsvDeserializer deserializer;
            deserializer.delimiter('|');
            deserializer.read(in);
            deserializer.deserialize(data);

            CXXTOOLS_UNIT_ASSERT_EQUALS(data.size(), 2u);
            CXXTOOLS_UNIT_ASSERT_EQUALS(data[0].size(), 3u);
            CXXTOOLS_UNIT_ASSERT_EQUALS(data[1].size(), 3u);
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

            cxxtools::CsvDeserializer deserializer;
            deserializer.read(in);
            deserializer.deserialize(data);

            CXXTOOLS_UNIT_ASSERT_EQUALS(data.size(), 2u);
            CXXTOOLS_UNIT_ASSERT_EQUALS(data[0].intValue, 17);
            CXXTOOLS_UNIT_ASSERT_EQUALS(data[0].stringValue, "Hi");
            CXXTOOLS_UNIT_ASSERT_EQUALS(data[0].doubleValue, 2.5);
            CXXTOOLS_UNIT_ASSERT_EQUALS(data[1].intValue, -6);
            CXXTOOLS_UNIT_ASSERT_EQUALS(data[1].stringValue, "Foo");
            CXXTOOLS_UNIT_ASSERT_EQUALS(data[1].doubleValue, -1000);
        }

        void testQuoteData()
        {
            std::vector<std::vector<std::string> > data;
            std::istringstream in(
                "A|B\n"
                "He'llo|Wor'ld'\n"
                "He\"llo|Wor\"'ld\n");

            in >> cxxtools::Csv(data);

            CXXTOOLS_UNIT_ASSERT_EQUALS(data.size(), 2u);
            CXXTOOLS_UNIT_ASSERT_EQUALS(data[0].size(), 2u);
            CXXTOOLS_UNIT_ASSERT_EQUALS(data[1].size(), 2u);
            CXXTOOLS_UNIT_ASSERT_EQUALS(data[0][0], "He'llo");
            CXXTOOLS_UNIT_ASSERT_EQUALS(data[0][1], "Wor'ld'");
            CXXTOOLS_UNIT_ASSERT_EQUALS(data[1][0], "He\"llo");
            CXXTOOLS_UNIT_ASSERT_EQUALS(data[1][1], "Wor\"'ld");
        }

        void testDoubleQuoteData()
        {
            std::vector<std::vector<std::string> > data;
            std::istringstream in(
                "A|B\n"
                "\"Hello\"|\"\"\"World\"\"\"\n"
                "\"He\"\"llo\"|'''World'");

            in >> cxxtools::Csv(data);

            CXXTOOLS_UNIT_ASSERT_EQUALS(data.size(), 2u);
            CXXTOOLS_UNIT_ASSERT_EQUALS(data[0].size(), 2u);
            CXXTOOLS_UNIT_ASSERT_EQUALS(data[1].size(), 2u);
            CXXTOOLS_UNIT_ASSERT_EQUALS(data[0][0], "Hello");
            CXXTOOLS_UNIT_ASSERT_EQUALS(data[0][1], "\"World\"");
            CXXTOOLS_UNIT_ASSERT_EQUALS(data[1][0], "He\"llo");
            CXXTOOLS_UNIT_ASSERT_EQUALS(data[1][1], "'World");
        }

        void testFailDecoding()
        {
            std::istringstream in(
                "A\xff|B|C\n");

            cxxtools::CsvDeserializer deserializer;
            CXXTOOLS_UNIT_ASSERT_THROW(deserializer.read(in), std::exception);
        }

        void testLinefeed()
        {
            std::vector<std::vector<std::string> > data;
            std::istringstream in(
                "Hello,World\n"
                "\"foo\nbar\",blub\n");

            cxxtools::CsvDeserializer deserializer;
            deserializer.readTitle(false);
            deserializer.delimiter(',');
            deserializer.read(in);
            deserializer.deserialize(data);

            CXXTOOLS_UNIT_ASSERT_EQUALS(data.size(), 2u);
            CXXTOOLS_UNIT_ASSERT_EQUALS(data[0].size(), 2u);
            CXXTOOLS_UNIT_ASSERT_EQUALS(data[1].size(), 2u);
            CXXTOOLS_UNIT_ASSERT_EQUALS(data[0][0], "Hello");
            CXXTOOLS_UNIT_ASSERT_EQUALS(data[0][1], "World");
            CXXTOOLS_UNIT_ASSERT_EQUALS(data[1][0], "foo\nbar");
            CXXTOOLS_UNIT_ASSERT_EQUALS(data[1][1], "blub");
        }

        void testUnicode()
        {
            std::vector<std::vector<cxxtools::String> > data;

            std::istringstream in(
                "a;b\n"
                "M\xc3\xa4kitalo;42\n");

            cxxtools::CsvDeserializer deserializer;
            deserializer.read(in);
            deserializer.deserialize(data);

            CXXTOOLS_UNIT_ASSERT_EQUALS(data.size(), 1u);
            CXXTOOLS_UNIT_ASSERT_EQUALS(data[0].size(), 2u);
            CXXTOOLS_UNIT_ASSERT_EQUALS(data[0][0], cxxtools::String(L"M\xe4kitalo"));
            CXXTOOLS_UNIT_ASSERT_EQUALS(data[0][1], cxxtools::String(L"42"));
        }

#if __cplusplus >= 201703L
        void testOptional()
        {
            std::vector<TestObjectOptional> data;

            std::istringstream in(
                "intValue,doubleValue,charValue\n"
                "45,,u\n"
                ",5.5,\n");

            cxxtools::CsvDeserializer deserializer;
            deserializer.read(in);
            deserializer.deserialize(data);

            CXXTOOLS_UNIT_ASSERT_EQUALS(data.size(), 2u);
            CXXTOOLS_UNIT_ASSERT(data[0].intValue);
            CXXTOOLS_UNIT_ASSERT_EQUALS(*data[0].intValue, 45);
            CXXTOOLS_UNIT_ASSERT(!data[0].doubleValue);
            CXXTOOLS_UNIT_ASSERT(data[0].charValue);
            CXXTOOLS_UNIT_ASSERT_EQUALS(*data[0].charValue, 'u');

            CXXTOOLS_UNIT_ASSERT(!data[1].intValue);
            CXXTOOLS_UNIT_ASSERT(data[1].doubleValue);
            CXXTOOLS_UNIT_ASSERT_EQUALS(*data[1].doubleValue, 5.5);
            CXXTOOLS_UNIT_ASSERT(!data[1].charValue);
        }
#endif

        void testSkipEmptyLines()
        {
            std::vector<std::vector<std::string> > data;
            std::istringstream in(
                "A|B|C\n"
                "\n"
                "Hello|World|\n"
                "\n"
                "34|67|\"23\"\n"
                "col1|'col2'|col3\n"
                "\n");

            in >> cxxtools::Csv(data).skipEmptyLines(true);
            CXXTOOLS_UNIT_ASSERT_EQUALS(data.size(), 3u);
        }

        void testNoSkipEmptyLines()
        {
            std::vector<std::vector<std::string> > data;
            std::istringstream in(
                "A|B|C\n"
                "\n"
                "Hello|World|\n"
                "\n"
                "34|67|\"23\"\n"
                "col1|'col2'|col3\n"
                "\n");

            cxxtools::CsvDeserializer deserializer;
            deserializer.skipEmptyLines(false);
            CXXTOOLS_UNIT_ASSERT_THROW(deserializer.read(in), std::exception);
        }
};

cxxtools::unit::RegisterTest<CsvDeserializerTest> register_CsvDeserializerTest;
