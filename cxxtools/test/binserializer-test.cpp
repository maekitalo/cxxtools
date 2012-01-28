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
#include "cxxtools/bin/serializer.h"
#include "cxxtools/bin/deserializer.h"
#include "cxxtools/log.h"
#include "cxxtools/hdstream.h"
#include <limits>
#include <stdint.h>

log_define("cxxtools.test.binserializer")

namespace
{
    struct TestObject
    {
        int intValue;
        std::string stringValue;
        double doubleValue;
        bool boolValue;
        bool nullValue;
    };

    void operator>>= (const cxxtools::SerializationInfo& si, TestObject& obj)
    {
        si.getMember("intValue") >>= obj.intValue;
        si.getMember("stringValue") >>= obj.stringValue;
        si.getMember("doubleValue") >>= obj.doubleValue;
        si.getMember("boolValue") >>= obj.boolValue;
        const cxxtools::SerializationInfo* p = si.findMember("nullValue");
        obj.nullValue = p != 0 && p->isNull();
    }

    void operator<<= (cxxtools::SerializationInfo& si, const TestObject& obj)
    {
        si.addMember("intValue") <<= obj.intValue;
        si.addMember("stringValue") <<= obj.stringValue;
        si.addMember("doubleValue") <<= obj.doubleValue;
        si.addMember("boolValue") <<= obj.boolValue;
        si.addMember("nullValue");
        si.setTypeName("TestObject");
    }

    bool operator== (const TestObject& obj1, const TestObject& obj2)
    {
        return obj1.intValue == obj2.intValue
            && obj1.stringValue == obj2.stringValue
            && obj1.doubleValue == obj2.doubleValue
            && obj1.boolValue == obj2.boolValue
            && obj1.nullValue == obj2.nullValue;
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
            registerMethod("testInt", *this, &BinSerializerTest::testInt);
            registerMethod("testDouble", *this, &BinSerializerTest::testDouble);
            registerMethod("testArray", *this, &BinSerializerTest::testArray);
            registerMethod("testObject", *this, &BinSerializerTest::testObject);
            registerMethod("testComplexObject", *this, &BinSerializerTest::testComplexObject);
            registerMethod("testBinaryData", *this, &BinSerializerTest::testBinaryData);
        }

        void testScalar()
        {
            std::stringstream data;
            cxxtools::bin::Serializer serializer(data);
            cxxtools::bin::Deserializer deserializer(data);

            int value = 5;
            serializer.serialize(value);

            int value2 = 0;
            deserializer.deserialize(value2);

            CXXTOOLS_UNIT_ASSERT_EQUALS(value, value2);
        }

        template <typename IntT>
        void testIntValue(IntT value)
        {
            std::stringstream data;
            cxxtools::bin::Serializer serializer(data);
            cxxtools::bin::Deserializer deserializer(data);

            serializer.serialize(value);

            log_debug("int: " << cxxtools::hexDump(data.str()));

            IntT result = 0;
            deserializer.deserialize(result);

            CXXTOOLS_UNIT_ASSERT_EQUALS(value, result);
        }

        void testInt()
        {
            testIntValue(30);
            testIntValue(300);
            testIntValue(100000);

            testIntValue(-30);
            testIntValue(-300);
            testIntValue(-100000);

            testIntValue(static_cast<int64_t>(std::numeric_limits<int8_t>::max()) + 1);
            testIntValue(static_cast<int64_t>(std::numeric_limits<int16_t>::max()) + 1);
            testIntValue(static_cast<int64_t>(std::numeric_limits<int32_t>::max()) + 1);
            testIntValue(std::numeric_limits<int64_t>::max());

            testIntValue(static_cast<int64_t>(std::numeric_limits<int8_t>::min()) - 1);
            testIntValue(static_cast<int64_t>(std::numeric_limits<int16_t>::min()) - 1);
            testIntValue(static_cast<int64_t>(std::numeric_limits<int32_t>::min()) - 1);
            testIntValue(std::numeric_limits<int64_t>::min());

            testIntValue(static_cast<uint64_t>(std::numeric_limits<uint8_t>::max()) + 1);
            testIntValue(static_cast<uint64_t>(std::numeric_limits<uint16_t>::max()) + 1);
            testIntValue(static_cast<uint64_t>(std::numeric_limits<uint32_t>::max()) + 1);
            testIntValue(std::numeric_limits<uint64_t>::max());
        }

        void testDoubleValue(double value)
        {
            std::stringstream data;
            cxxtools::bin::Serializer serializer(data);
            cxxtools::bin::Deserializer deserializer(data);

            serializer.serialize(value);
            double result = 0.0;
            deserializer.deserialize(result);

            log_debug("test double value " << value << " => " << cxxtools::hexDump(data.str()) << " => " << result);

            if (value != value) // check for nan
                CXXTOOLS_UNIT_ASSERT(result != result);
            else if (value == std::numeric_limits<double>::infinity())
                CXXTOOLS_UNIT_ASSERT_EQUALS(result, std::numeric_limits<double>::infinity());
            else if (value == -std::numeric_limits<double>::infinity())
                CXXTOOLS_UNIT_ASSERT_EQUALS(result, -std::numeric_limits<double>::infinity());
            else if (value / result > 1.00001 || value / result < 0.99999)
                CXXTOOLS_UNIT_FAIL("double test failed; value " << value << " got " << result);
        }

        void testDouble()
        {
            testDoubleValue(1234.0);   // short float
            testDoubleValue(-1234.0);   // short float
            testDoubleValue(12345678.0);   // medium float
            testDoubleValue(-12345678.0);   // medium float
            testDoubleValue(123456789123456789.0);  // long float
            testDoubleValue(-123456789123456789.0);  // long float
            testDoubleValue(-3.877e-123);
            testDoubleValue(std::numeric_limits<double>::max());
            //testDoubleValue(std::numeric_limits<double>::min());
            testDoubleValue(std::numeric_limits<double>::infinity());
            testDoubleValue(-std::numeric_limits<double>::infinity());
            testDoubleValue(std::numeric_limits<double>::quiet_NaN());

            std::stringstream data;
            cxxtools::bin::Serializer serializer(data);
            cxxtools::bin::Deserializer deserializer(data);

            serializer.serialize(std::numeric_limits<double>::quiet_NaN());
            double result = 0.0;
            deserializer.deserialize(result);

            CXXTOOLS_UNIT_ASSERT(result != result); 

        }

        void testArray()
        {
            std::stringstream data;
            cxxtools::bin::Serializer serializer(data);
            cxxtools::bin::Deserializer deserializer(data);

            std::vector<int> intvector;
            intvector.push_back(4711);
            intvector.push_back(4712);
            intvector.push_back(-3);
            intvector.push_back(-257);

            serializer.serialize(intvector);

            log_debug("intvector: " << cxxtools::hexDump(data.str()));

            std::vector<int> intvector2;
            deserializer.deserialize(intvector2);

            CXXTOOLS_UNIT_ASSERT_EQUALS(intvector.size(), intvector2.size());
            CXXTOOLS_UNIT_ASSERT_EQUALS(intvector[0], intvector2[0]);
            CXXTOOLS_UNIT_ASSERT_EQUALS(intvector[1], intvector2[1]);
            CXXTOOLS_UNIT_ASSERT_EQUALS(intvector[2], intvector2[2]);
            CXXTOOLS_UNIT_ASSERT_EQUALS(intvector[3], intvector2[3]);
        }

        void testObject()
        {
            std::stringstream data;
            cxxtools::bin::Serializer serializer(data);
            cxxtools::bin::Deserializer deserializer(data);

            TestObject obj;
            obj.intValue = 17;
            obj.stringValue = "foobar";
            obj.doubleValue = 3.125;
            obj.boolValue = true;
            obj.nullValue = true;
            serializer.serialize(obj);

            log_debug("bindata testobject: " << cxxtools::hexDump(data.str()));

            TestObject obj2;
            deserializer.deserialize(obj2);

            CXXTOOLS_UNIT_ASSERT_EQUALS(obj.intValue, obj2.intValue);
            CXXTOOLS_UNIT_ASSERT_EQUALS(obj.stringValue, obj2.stringValue);
            CXXTOOLS_UNIT_ASSERT_EQUALS(obj.doubleValue, obj2.doubleValue);
            CXXTOOLS_UNIT_ASSERT_EQUALS(obj.boolValue, obj2.boolValue);
            CXXTOOLS_UNIT_ASSERT_EQUALS(obj.nullValue, obj2.nullValue);
            CXXTOOLS_UNIT_ASSERT(obj == obj2);
        }

        void testComplexObject()
        {
            std::stringstream data;
            cxxtools::bin::Serializer serializer(data);
            cxxtools::bin::Deserializer deserializer(data);

            std::vector<TestObject2> v;
            TestObject2 obj;
            obj.intValue = 17;
            obj.stringValue = "foobar";
            obj.doubleValue = 3.125;
            obj.boolValue = false;
            obj.nullValue = true;
            obj.setValue.insert(17);
            obj.setValue.insert(23);
            obj.mapValue[45] = "fourtyfive";
            obj.mapValue[88] = "eightyeight";
            obj.mapValue[100] = "onehundred";
            v.push_back(obj);

            obj.setValue.insert(88);
            v.push_back(obj);

            serializer.serialize(v);

            log_debug("bindata complex object: " << cxxtools::hexDump(data.str()));

            std::vector<TestObject2> v2;
            deserializer.deserialize(v2);

            CXXTOOLS_UNIT_ASSERT(v == v2);
        }

        void testBinaryData()
        {
            std::stringstream data;
            cxxtools::bin::Serializer serializer(data);
            cxxtools::bin::Deserializer deserializer(data);

            std::string v;
            for (unsigned n = 0; n < 1024; ++n)
                v.push_back(static_cast<char>(n));

            serializer.serialize(v);
            log_debug("v.data=" << cxxtools::hexDump(data.str()));

            std::string v2;
            deserializer.deserialize(v2);

            CXXTOOLS_UNIT_ASSERT_EQUALS(v2.size(), 1024);
            CXXTOOLS_UNIT_ASSERT(v == v2);

            data.str(std::string());

            for (unsigned n = 0; n < 0xffff; ++n)
                v.push_back(static_cast<char>(n));

            serializer.serialize(v);
            deserializer.deserialize(v2);

            CXXTOOLS_UNIT_ASSERT_EQUALS(v2.size(), 0xffff + 1024);
            CXXTOOLS_UNIT_ASSERT(v == v2);

        }
};

cxxtools::unit::RegisterTest<BinSerializerTest> register_BinSerializerTest;
