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
#include "cxxtools/serializationinfo.h"
#include "cxxtools/propertiesdeserializer.h"
#include "cxxtools/propertiesparser.h"
#include "cxxtools/properties.h"

namespace
{
    struct TestObject
    {
        int intValue;
        std::string stringValue;
        double doubleValue;
        bool boolValue;
    };

    void operator>>= (const cxxtools::SerializationInfo& si, TestObject& obj)
    {
        si.getMember("intValue") >>= obj.intValue;
        si.getMember("stringValue") >>= obj.stringValue;
        si.getMember("doubleValue") >>= obj.doubleValue;
        si.getMember("boolValue") >>= obj.boolValue;
    }

    void operator<<= (cxxtools::SerializationInfo& si, const TestObject& obj)
    {
        si.addMember("intValue") <<= obj.intValue;
        si.addMember("stringValue") <<= obj.stringValue;
        si.addMember("doubleValue") <<= obj.doubleValue;
        si.addMember("boolValue") <<= obj.boolValue;
        si.setTypeName("TestObject");
    }

    bool operator== (const TestObject& obj1, const TestObject& obj2)
    {
        return obj1.intValue == obj2.intValue
            && obj1.stringValue == obj2.stringValue
            && obj1.doubleValue == obj2.doubleValue
            && obj1.boolValue == obj2.boolValue;
    }

}

class PropertiesTest : public cxxtools::unit::TestSuite
{
    public:
        PropertiesTest()
            : cxxtools::unit::TestSuite("properties")
        {
            registerMethod("testProperties", *this, &PropertiesTest::testProperties);
            registerMethod("testFailProperties", *this, &PropertiesTest::testFailProperties);
            registerMethod("testScalar", *this, &PropertiesTest::testScalar);
            registerMethod("testStruct", *this, &PropertiesTest::testStruct);
            registerMethod("testVector", *this, &PropertiesTest::testVector);
            registerMethod("testMember", *this, &PropertiesTest::testMember);
            registerMethod("testIStream", *this, &PropertiesTest::testIStream);
            registerMethod("testStrangeKeys", *this, &PropertiesTest::testStrangeKeys);
            registerMethod("testMultilineValues", *this, &PropertiesTest::testMultilineValues);
            registerMethod("testEmptyValue", *this, &PropertiesTest::testEmptyValue);
        }

        void testProperties()
        {
            std::istringstream data(
                "a\\ b = 42\n"
                "b=\\u9\\r\\n\\t\n"
                "\\ufoo =Hi there\n"
                "l:Hi \\\n"
                "there\n"
                "c=\\uabc\\5\\u0000abCD1\\u1234");

            cxxtools::PropertiesDeserializer deserializer(data);

            // the first call to the deserialize method will parse the input
            // and create a SerializationInfo object

            int v = 0;
            CXXTOOLS_UNIT_ASSERT_NOTHROW(deserializer.deserialize(v, "a b"));
            CXXTOOLS_UNIT_ASSERT_EQUALS(v, 42);

            CXXTOOLS_UNIT_ASSERT_THROW(deserializer.deserialize(v, "a b "), cxxtools::SerializationError);

            cxxtools::String b;
            CXXTOOLS_UNIT_ASSERT_NOTHROW(deserializer.deserialize(b, "b"));
            CXXTOOLS_UNIT_ASSERT_EQUALS(b, "\t\r\n\t");

            cxxtools::String c;
            CXXTOOLS_UNIT_ASSERT_NOTHROW(deserializer.deserialize(c, "c"));

            CXXTOOLS_UNIT_ASSERT(c.size() >= 1);
            CXXTOOLS_UNIT_ASSERT_EQUALS(c[0].value(), 0xabc);

            CXXTOOLS_UNIT_ASSERT(c.size() >= 2);
            CXXTOOLS_UNIT_ASSERT_EQUALS(c[1], '5');

            CXXTOOLS_UNIT_ASSERT(c.size() >= 3);
            CXXTOOLS_UNIT_ASSERT_EQUALS(c[2].value(), 0xabcd);

            CXXTOOLS_UNIT_ASSERT(c.size() >= 4);
            CXXTOOLS_UNIT_ASSERT_EQUALS(c[3], '1');

            CXXTOOLS_UNIT_ASSERT(c.size() >= 5);
            CXXTOOLS_UNIT_ASSERT_EQUALS(c[4].value(), 0x1234);

            CXXTOOLS_UNIT_ASSERT_EQUALS(c.size(), 5);

            cxxtools::String l;
            CXXTOOLS_UNIT_ASSERT_NOTHROW(deserializer.deserialize(l, "l"));
            CXXTOOLS_UNIT_ASSERT_EQUALS(l, "Hi there");

            cxxtools::String f;
            CXXTOOLS_UNIT_ASSERT_NOTHROW(deserializer.deserialize(f, "\x0foo"));
            CXXTOOLS_UNIT_ASSERT_EQUALS(f, "Hi there");
        }

        void testFailProperties()
        {
            std::istringstream data(
                "ab = \\uz42\n");
            CXXTOOLS_UNIT_ASSERT_THROW(cxxtools::PropertiesDeserializer deserializer(data), cxxtools::PropertiesParserError);
        }

        void testScalar()
        {
            std::istringstream data(
                "rootlogger=I\n"
                "value.a=17\n"
                "a.b.c=3.25"
            );

            cxxtools::PropertiesDeserializer deserializer(data);

            int value = 5;
            deserializer.deserialize(value, "value.a");
            CXXTOOLS_UNIT_ASSERT_EQUALS(value, 17);

            double d = 0;
            deserializer.deserialize(d, "a.b.c");
            CXXTOOLS_UNIT_ASSERT_EQUALS(d, 3.25);
        }

        void testStruct()
        {
            std::istringstream data(
                "s.foo.intValue=5\n"
                "s.foo.stringValue=Hi there\n"
                "s.foo.doubleValue=45.5\n"
                "s.foo.boolValue=true\n"
            );

            cxxtools::PropertiesDeserializer deserializer(data);

            TestObject obj;
            deserializer.deserialize(obj, "s.foo");

            CXXTOOLS_UNIT_ASSERT_EQUALS(obj.intValue, 5);
            CXXTOOLS_UNIT_ASSERT_EQUALS(obj.stringValue, "Hi there");
            CXXTOOLS_UNIT_ASSERT_EQUALS(obj.doubleValue, 45.5);
            CXXTOOLS_UNIT_ASSERT(obj.boolValue);
        }

        void testVector()
        {
            std::istringstream data(
                "myvector.0=4\n"
                "myvector.1=Hi\n"
                "myvector.2=foo\n"
            );

            cxxtools::PropertiesDeserializer deserializer(data);

            std::vector<std::string> v;

            deserializer.deserialize(v, "myvector");

            CXXTOOLS_UNIT_ASSERT_EQUALS(v.size(), 3);
            CXXTOOLS_UNIT_ASSERT_EQUALS(v[0], "4");
            CXXTOOLS_UNIT_ASSERT_EQUALS(v[1], "Hi");
            CXXTOOLS_UNIT_ASSERT_EQUALS(v[2], "foo");
        }

        void testMember()
        {
            std::istringstream data(
                "a.b.c.d=5\n"
                "a.e.f.g=7\n");

            cxxtools::PropertiesDeserializer deserializer(data);
            const cxxtools::SerializationInfo& si = deserializer.si();

            int v = 0;
            CXXTOOLS_UNIT_ASSERT_NOTHROW(si.getMember("a").getMember("b.c.d") >>= v);
            CXXTOOLS_UNIT_ASSERT_EQUALS(v, 5);

            v = 0;
            CXXTOOLS_UNIT_ASSERT_NOTHROW(si.getMember("a.b").getMember("c.d") >>= v);
            CXXTOOLS_UNIT_ASSERT_EQUALS(v, 5);

            v = 0;
            CXXTOOLS_UNIT_ASSERT_NOTHROW(si.getMember("a.b.c").getMember("d") >>= v);
            CXXTOOLS_UNIT_ASSERT_EQUALS(v, 5);

            v = 0;
            CXXTOOLS_UNIT_ASSERT_NOTHROW(si.getMember("a.b.c.d") >>= v);
            CXXTOOLS_UNIT_ASSERT_EQUALS(v, 5);

            v = 0;
            CXXTOOLS_UNIT_ASSERT_NOTHROW(
                si.getMember("a").getMember("b").getMember("c").getMember("d") >>= v);
            CXXTOOLS_UNIT_ASSERT_EQUALS(v, 5);

            v = 0;
            CXXTOOLS_UNIT_ASSERT_NOTHROW(
                si.getMember("a.b").getMember("c").getMember("d") >>= v);
            CXXTOOLS_UNIT_ASSERT_EQUALS(v, 5);

            v = 0;
            CXXTOOLS_UNIT_ASSERT_NOTHROW(
                si.getMember("a").getMember("b.c").getMember("d") >>= v);
            CXXTOOLS_UNIT_ASSERT_EQUALS(v, 5);

            v = 0;
            CXXTOOLS_UNIT_ASSERT_NOTHROW(
                si.getMember("a").getMember("b.c.d") >>= v);
            CXXTOOLS_UNIT_ASSERT_EQUALS(v, 5);

            v = 0;
            CXXTOOLS_UNIT_ASSERT_NOTHROW(
                si.getMember("a.b").getMember("c.d") >>= v);
            CXXTOOLS_UNIT_ASSERT_EQUALS(v, 5);

            v = 0;
            CXXTOOLS_UNIT_ASSERT_NOTHROW(
                si.getMember("a.e.f.g") >>= v);
            CXXTOOLS_UNIT_ASSERT_EQUALS(v, 7);

        }

        void testIStream()
        {
            std::istringstream data(
                "s.foo.intValue=5\n"
                "s.foo.stringValue=Hi there\n"
                "s.foo.doubleValue=45.5\n"
                "s.foo.boolValue=true\n"
            );

            TestObject obj;
            data >> cxxtools::Properties(obj, "s.foo");

            CXXTOOLS_UNIT_ASSERT_EQUALS(obj.intValue, 5);
            CXXTOOLS_UNIT_ASSERT_EQUALS(obj.stringValue, "Hi there");
            CXXTOOLS_UNIT_ASSERT_EQUALS(obj.doubleValue, 45.5);
            CXXTOOLS_UNIT_ASSERT(obj.boolValue);
        }

        void testStrangeKeys()
        {
            std::istringstream data(
                "a-b*+\\==5\n"
                "\\:\\::7\n");

            cxxtools::PropertiesDeserializer deserializer(data);
            const cxxtools::SerializationInfo& si = deserializer.si();

            int v = 0;
            CXXTOOLS_UNIT_ASSERT_NOTHROW(si.getMember("a-b*+=") >>= v);
            CXXTOOLS_UNIT_ASSERT_EQUALS(v, 5);

            CXXTOOLS_UNIT_ASSERT_NOTHROW(si.getMember("::") >>= v);
            CXXTOOLS_UNIT_ASSERT_EQUALS(v, 7);

        }

        void testMultilineValues()
        {
            std::istringstream data(
                "fruits                           apple, banana, pear, \\\n"
                "                  cantaloupe, watermelon, \\\n"
                "                  kiwi, mango");

            cxxtools::PropertiesDeserializer deserializer(data);
            const cxxtools::SerializationInfo& si = deserializer.si();

            std::string v;
            CXXTOOLS_UNIT_ASSERT_NOTHROW(si.getMember("fruits") >>= v);
            CXXTOOLS_UNIT_ASSERT_EQUALS(v, "apple, banana, pear, cantaloupe, watermelon, kiwi, mango");
        }

        void testEmptyValue()
        {
            std::istringstream data("apple\n"
                "banana \n"
                "pear : \n"
                "cantaloupe");

            cxxtools::PropertiesDeserializer deserializer(data);
            const cxxtools::SerializationInfo& si = deserializer.si();

            std::string v;
            CXXTOOLS_UNIT_ASSERT_NOTHROW(si.getMember("apple") >>= v);
            CXXTOOLS_UNIT_ASSERT_EQUALS(v, "");

            CXXTOOLS_UNIT_ASSERT_NOTHROW(si.getMember("banana") >>= v);
            CXXTOOLS_UNIT_ASSERT_EQUALS(v, "");

            CXXTOOLS_UNIT_ASSERT_NOTHROW(si.getMember("pear") >>= v);
            CXXTOOLS_UNIT_ASSERT_EQUALS(v, "");

            CXXTOOLS_UNIT_ASSERT_NOTHROW(si.getMember("cantaloupe") >>= v);
            CXXTOOLS_UNIT_ASSERT_EQUALS(v, "");

        }

};

cxxtools::unit::RegisterTest<PropertiesTest> register_PropertiesTest;
