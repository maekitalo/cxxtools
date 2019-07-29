/*
 * Copyright (C) 2019 Tommi Maekitalo
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
#include "cxxtools/ini.h"
#include <sstream>

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

    struct TestObjects
    {
        TestObject s1;
        TestObject s2;
    };

    void operator>>= (const cxxtools::SerializationInfo& si, TestObjects& obj)
    {
        si.getMember("s1") >>= obj.s1;
        si.getMember("s2") >>= obj.s2;
    }

    void operator<<= (cxxtools::SerializationInfo& si, const TestObjects& obj)
    {
        si.addMember("s1") <<= obj.s1;
        si.addMember("s2") <<= obj.s2;
    }

    bool operator== (const TestObjects& obj1, const TestObjects& obj2)
    {
        return obj1.s1 == obj2.s1
            && obj1.s2 == obj2.s2;
    }

}

class IniSerializationTest : public cxxtools::unit::TestSuite
{

    public:
        IniSerializationTest()
        : cxxtools::unit::TestSuite("iniserialization")
        {
            registerMethod("testReadVector", *this, &IniSerializationTest::testReadVector);
            registerMethod("testWriteReadObject", *this, &IniSerializationTest::testWriteReadObject);
            registerMethod("testReadValues", *this, &IniSerializationTest::testReadValues);
            registerMethod("testWriteReadValues", *this, &IniSerializationTest::testWriteReadValues);
        }

        void testReadVector()
        {
            std::istringstream in(
                "[s1]\n"
                "intValue=42\n"
                "stringValue=foobar\n"
                "doubleValue=3.125\n"
                "boolValue=true\n"
                "[s2]\n"
                "intValue=17\n"
                "stringValue=baz\n"
                "doubleValue=-1.5\n"
                "boolValue=false\n"
                );

            std::vector<TestObject> v;
            in >> cxxtools::Ini(v);

            CXXTOOLS_UNIT_ASSERT_EQUALS(v.size(), 2);

            CXXTOOLS_UNIT_ASSERT_EQUALS(v[0].intValue, 42);
            CXXTOOLS_UNIT_ASSERT_EQUALS(v[0].stringValue, "foobar");
            CXXTOOLS_UNIT_ASSERT_EQUALS(v[0].doubleValue, 3.125);
            CXXTOOLS_UNIT_ASSERT_EQUALS(v[0].boolValue, true);

            CXXTOOLS_UNIT_ASSERT_EQUALS(v[1].intValue, 17);
            CXXTOOLS_UNIT_ASSERT_EQUALS(v[1].stringValue, "baz");
            CXXTOOLS_UNIT_ASSERT_EQUALS(v[1].doubleValue, -1.5);
            CXXTOOLS_UNIT_ASSERT_EQUALS(v[1].boolValue, false);
        }

        void testWriteReadObject()
        {
            TestObjects to;
            to.s1.intValue = 17;
            to.s1.stringValue = "baz";
            to.s1.doubleValue = -345.5;
            to.s1.boolValue = true;
            to.s2.intValue = 42;
            to.s2.stringValue = "foobar";
            to.s2.doubleValue = 3.125;
            to.s2.boolValue = false;

            std::stringstream inout;
            inout << cxxtools::Ini(to);

            TestObjects to2;
            inout >> cxxtools::Ini(to2);

            CXXTOOLS_UNIT_ASSERT(to == to2);
        }

        void testReadValues()
        {
            std::istringstream in(
                "intValue=42\n"
                "stringValue=foobar\n"
                "doubleValue=3.125\n"
                "boolValue=true\n");

            TestObject obj;
            in >> cxxtools::Ini(obj);

            CXXTOOLS_UNIT_ASSERT_EQUALS(obj.intValue, 42);
            CXXTOOLS_UNIT_ASSERT_EQUALS(obj.stringValue, "foobar");
            CXXTOOLS_UNIT_ASSERT_EQUALS(obj.doubleValue, 3.125);
            CXXTOOLS_UNIT_ASSERT_EQUALS(obj.boolValue, true);
        }

        void testWriteReadValues()
        {
            TestObject obj;
            obj.intValue = 17;
            obj.stringValue = "baz";
            obj.doubleValue = -345.5;
            obj.boolValue = true;

            std::stringstream inout;
            inout << cxxtools::Ini(obj);

            TestObject obj2;
            inout >> cxxtools::Ini(obj2);

            CXXTOOLS_UNIT_ASSERT(obj == obj2);
        }

};

cxxtools::unit::RegisterTest<IniSerializationTest> register_IniSerializationTest;
