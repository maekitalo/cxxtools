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
#include "cxxtools/xml/xmlserializer.h"
#include "cxxtools/propertiesdeserializer.h"

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

class PropertiesDeserializerTest : public cxxtools::unit::TestSuite
{
    public:
        PropertiesDeserializerTest()
            : cxxtools::unit::TestSuite("propertiesdeserializer")
        {
            registerMethod("testScalar", *this, &PropertiesDeserializerTest::testScalar);
            registerMethod("testStruct", *this, &PropertiesDeserializerTest::testStruct);
            registerMethod("testVector", *this, &PropertiesDeserializerTest::testVector);
            registerMethod("testMember", *this, &PropertiesDeserializerTest::testMember);
            registerMethod("testProperties", *this, &PropertiesDeserializerTest::testProperties);
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

            data.clear();
            data.seekg(0);
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
                "myvector=4\n"
                "myvector=Hi\n"
                "myvector=foo\n"
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
            deserializer.deserialize();
            const cxxtools::SerializationInfo& si = *deserializer.si();

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
            CXXTOOLS_UNIT_ASSERT_NOTHROW(si.getMember("a").getMember(0) >>= v);
            CXXTOOLS_UNIT_ASSERT_EQUALS(v, 5);

            v = 0;
            CXXTOOLS_UNIT_ASSERT_NOTHROW(si.getMember("a").getMember(1) >>= v);
            CXXTOOLS_UNIT_ASSERT_EQUALS(v, 7);

        }

        void testProperties()
        {
            std::istringstream data(
                "a\\ b = 42\n"
                "b=\\u9\\r\\n\\t\n"
                "\\ufoo =Hi there\n"
                "l=Hi\\\n"
                "there\n"
                "c=\\uabCD1\\u1234");

            cxxtools::PropertiesDeserializer deserializer(data);
            deserializer.deserialize();
            const cxxtools::SerializationInfo& si = *deserializer.si();

            int v = 0;
            CXXTOOLS_UNIT_ASSERT_NOTHROW(si.getMember("a b") >>= v);
            CXXTOOLS_UNIT_ASSERT_EQUALS(v, 42);

            cxxtools::String b;
            CXXTOOLS_UNIT_ASSERT_NOTHROW(si.getMember("b") >>= b);
            CXXTOOLS_UNIT_ASSERT_EQUALS(b, L"\t\r\n\t");

            cxxtools::String c;
            CXXTOOLS_UNIT_ASSERT_NOTHROW(si.getMember("c") >>= c);
            CXXTOOLS_UNIT_ASSERT_EQUALS(c.size(), 3);
            CXXTOOLS_UNIT_ASSERT_EQUALS(c[0].value(), 0xabcd);
            CXXTOOLS_UNIT_ASSERT_EQUALS(c[1], '1');
            CXXTOOLS_UNIT_ASSERT_EQUALS(c[2].value(), 0x1234);

            cxxtools::String l;
            CXXTOOLS_UNIT_ASSERT_NOTHROW(si.getMember("l") >>= l);
            CXXTOOLS_UNIT_ASSERT_EQUALS(l, L"Hi\nthere");

            cxxtools::String f;
            CXXTOOLS_UNIT_ASSERT_NOTHROW(si.getMember("\x0foo") >>= f);
            CXXTOOLS_UNIT_ASSERT_EQUALS(f, L"Hi there");
        }

};

cxxtools::unit::RegisterTest<PropertiesDeserializerTest> register_PropertiesDeserializerTest;
