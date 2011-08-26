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
#include "cxxtools/serializationinfo.h"
#include "cxxtools/binserializer.h"
#include "cxxtools/bindeserializer.h"

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

    void operator<<= (cxxtools::SerializationInfo& si, const TestObject& obj)
    {
        si.addMember("intValue") <<= obj.intValue;
        si.addMember("stringValue") <<= obj.stringValue;
        si.addMember("doubleValue") <<= obj.doubleValue;
        si.setTypeName("TestObject");
    }

    bool operator== (const TestObject& obj1, const TestObject& obj2)
    {
        return obj1.intValue == obj2.intValue
            && obj1.stringValue == obj2.stringValue
            && obj1.doubleValue == obj2.doubleValue;
    }

    struct TestObject2 : public TestObject
    {
        typedef std::set<unsigned> SetType;
        typedef std::map<unsigned, std::string> MapType;
        SetType setValue;
        MapType mapValue;
    };

    void operator>>= (const cxxtools::SerializationInfo& si, TestObject2& obj)
    {
        si >>= static_cast<TestObject&>(obj);
        si.getMember("setValue") >>= obj.setValue;
        si.getMember("mapValue") >>= obj.mapValue;
    }

    void operator<<= (cxxtools::SerializationInfo& si, const TestObject2& obj)
    {
        si <<= static_cast<const TestObject&>(obj);
        si.addMember("setValue") <<= obj.setValue;
        si.addMember("mapValue") <<= obj.mapValue;
        si.setTypeName("TestObject2");
    }

    bool operator== (const TestObject2& obj1, const TestObject2& obj2)
    {
        return static_cast<const TestObject&>(obj1) == static_cast<const TestObject&>(obj2)
            && obj1.setValue == obj2.setValue
            && obj1.mapValue == obj2.mapValue;
    }
}

class BinSerializerTest : public cxxtools::unit::TestSuite
{
    public:
        BinSerializerTest()
            : cxxtools::unit::TestSuite("binserializer")
        {
            registerMethod("testScalar", *this, &BinSerializerTest::testScalar);
            registerMethod("testArray", *this, &BinSerializerTest::testArray);
            registerMethod("testObject", *this, &BinSerializerTest::testObject);
            registerMethod("testComplexObject", *this, &BinSerializerTest::testComplexObject);
        }

        void testScalar()
        {
            std::stringstream data;
            cxxtools::BinSerializer serializer(data);
            cxxtools::BinDeserializer deserializer(data);

            int value = 5;
            serializer.serialize(value, "value");

            int value2 = 0;
            deserializer.deserialize(value2);

            CXXTOOLS_UNIT_ASSERT_EQUALS(value, value2);
        }

        void testArray()
        {
            std::stringstream data;
            cxxtools::BinSerializer serializer(data);
            cxxtools::BinDeserializer deserializer(data);

            std::vector<int> intvector;
            intvector.push_back(4711);
            intvector.push_back(4712);
            intvector.push_back(-3);

            serializer.serialize(intvector, "intvector");

            std::vector<int> intvector2;
            deserializer.deserialize(intvector2);

            CXXTOOLS_UNIT_ASSERT_EQUALS(intvector.size(), intvector2.size());
            CXXTOOLS_UNIT_ASSERT_EQUALS(intvector[0], intvector2[0]);
            CXXTOOLS_UNIT_ASSERT_EQUALS(intvector[1], intvector2[1]);
            CXXTOOLS_UNIT_ASSERT_EQUALS(intvector[2], intvector2[2]);
        }

        void testObject()
        {
            std::stringstream data;
            cxxtools::BinSerializer serializer(data);
            cxxtools::BinDeserializer deserializer(data);

            TestObject obj;
            obj.intValue = 17;
            obj.stringValue = "foobar";
            obj.doubleValue = 3.125;
            serializer.serialize(obj, "obj");

            TestObject obj2;
            deserializer.deserialize(obj2);

            CXXTOOLS_UNIT_ASSERT_EQUALS(obj.intValue, obj2.intValue);
            CXXTOOLS_UNIT_ASSERT_EQUALS(obj.stringValue, obj2.stringValue);
            CXXTOOLS_UNIT_ASSERT_EQUALS(obj.doubleValue, obj2.doubleValue);
            CXXTOOLS_UNIT_ASSERT(obj == obj2);
        }

        void testComplexObject()
        {
            std::stringstream data;
            cxxtools::BinSerializer serializer(data);
            cxxtools::BinDeserializer deserializer(data);

            std::vector<TestObject2> v;
            TestObject2 obj;
            obj.intValue = 17;
            obj.stringValue = "foobar";
            obj.doubleValue = 3.125;
            obj.setValue.insert(17);
            obj.setValue.insert(23);
            obj.mapValue[45] = "fourtyfive";
            obj.mapValue[88] = "eightyeight";
            obj.mapValue[100] = "onehundred";
            v.push_back(obj);

            obj.setValue.insert(88);
            v.push_back(obj);

            serializer.serialize(v, "v");

            std::vector<TestObject2> v2;
            deserializer.deserialize(v2);

            CXXTOOLS_UNIT_ASSERT(v == v2);
        }

};

cxxtools::unit::RegisterTest<BinSerializerTest> register_BinSerializerTest;
