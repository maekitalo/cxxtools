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
#include "cxxtools/bin/bin.h"
#include "cxxtools/log.h"
#include "cxxtools/timespan.h"
#include "cxxtools/hexdump.h"
#include <limits>
#include <stdint.h>
#include <config.h>

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

    template <typename T>
    std::string toBin(const T& t)
    {
        std::ostringstream out;
        out << cxxtools::bin::Bin(t);
        return out.str();
    }

    template <typename T>
    T fromBin(const std::string& bin)
    {
        std::istringstream in(bin);
        T t;
        in >> cxxtools::bin::Bin(t);
        return t;
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
            registerMethod("testChar", *this, &BinSerializerTest::testChar);
            registerMethod("testChar_0", *this, &BinSerializerTest::testChar_0);
            registerMethod("testString8", *this, &BinSerializerTest::testString8);
            registerMethod("testString8_0", *this, &BinSerializerTest::testString8_0);
            registerMethod("testString", *this, &BinSerializerTest::testString);
            registerMethod("testString_0", *this, &BinSerializerTest::testString_0);
            registerMethod("testDouble", *this, &BinSerializerTest::testDouble);
            registerMethod("testArray", *this, &BinSerializerTest::testArray);
            registerMethod("testObject", *this, &BinSerializerTest::testObject);
            registerMethod("testComplexObject", *this, &BinSerializerTest::testComplexObject);
            registerMethod("testObjectVector", *this, &BinSerializerTest::testObjectVector);
            registerMethod("testBinaryData", *this, &BinSerializerTest::testBinaryData);
            registerMethod("testReuse", *this, &BinSerializerTest::testReuse);
            registerMethod("testNamedVector", *this, &BinSerializerTest::testNamedVector);
            registerMethod("testTimespan", *this, &BinSerializerTest::testTimespan);
        }

        void testScalar()
        {
            std::stringstream data;

            int value = 5;
            data << cxxtools::bin::Bin(value);

            int value2 = 0;
            data >> cxxtools::bin::Bin(value2);

            CXXTOOLS_UNIT_ASSERT_EQUALS(value, value2);
        }

        template <typename IntT>
        void testIntValue(IntT value)
        {
            std::stringstream data;

            data << cxxtools::bin::Bin(value);

            IntT result = 0;
            data >> cxxtools::bin::Bin(result);

            CXXTOOLS_UNIT_ASSERT_EQUALS(value, result);
        }

        template <typename T>
        void testStreamableValue(T value, T initvalue)
        {
            std::stringstream data;

            data << cxxtools::bin::Bin(value);

            T result = initvalue;
            data >> cxxtools::bin::Bin(result);

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

            testIntValue(static_cast<int16_t>(std::numeric_limits<int8_t>::max()) + 1);
            testIntValue(static_cast<int32_t>(std::numeric_limits<int16_t>::max()) + 1);
            testIntValue(std::numeric_limits<int32_t>::max());
#ifdef INT64_IS_BASETYPE
            testIntValue(static_cast<int64_t>(std::numeric_limits<int32_t>::max()) + 1);
            testIntValue(std::numeric_limits<int64_t>::max());
#endif

            testIntValue(static_cast<int16_t>(std::numeric_limits<int8_t>::min()) - 1);
            testIntValue(static_cast<int32_t>(std::numeric_limits<int16_t>::min()) - 1);
            testIntValue(std::numeric_limits<int32_t>::min());
#ifdef INT64_IS_BASETYPE
            testIntValue(static_cast<int64_t>(std::numeric_limits<int32_t>::min()) - 1);
            testIntValue(std::numeric_limits<int64_t>::min());
#endif

            testIntValue(static_cast<uint16_t>(std::numeric_limits<uint8_t>::max()) + 1);
            testIntValue(static_cast<uint32_t>(std::numeric_limits<uint16_t>::max()) + 1);
            testIntValue(std::numeric_limits<uint32_t>::max());
#ifdef INT64_IS_BASETYPE
            testIntValue(static_cast<uint64_t>(std::numeric_limits<uint32_t>::max()) + 1);
            testIntValue(std::numeric_limits<uint64_t>::max());
#endif
        }

        void testChar()
        {
            testStreamableValue('a', '\xff');
            testStreamableValue('\1', '\xff');
        }

        void testChar_0()
        {
            testStreamableValue('\0', '\xff');
        }

        void testString8()
        {
            testStreamableValue(std::string(""), std::string("init"));
            testStreamableValue(std::string("Hi there"), std::string("init"));
        }

        void testString8_0()
        {
            testStreamableValue(std::string(1, '\0'), std::string("init"));
        }

        void testString()
        {
            testStreamableValue(cxxtools::String(L""), cxxtools::String(L"init"));
            testStreamableValue(cxxtools::String(L"Hi there"), cxxtools::String(L"init"));
        }

        void testString_0()
        {
            testStreamableValue(cxxtools::String(1, L'\0'), cxxtools::String(L"init"));
        }

        void testDoubleValue(double value)
        {
            std::stringstream data;

            data << cxxtools::bin::Bin(value);

            double result = 0.0;
            data >> cxxtools::bin::Bin(result);

            log_debug("test double value " << value << " =>\n" << cxxtools::hexDump(data.str()) << "\n=> " << result);

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
            testDoubleValue(0.0);
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

            data << cxxtools::bin::Bin(std::numeric_limits<double>::quiet_NaN());

            double result = 0.0;
            data >> cxxtools::bin::Bin(result);

            CXXTOOLS_UNIT_ASSERT(result != result); 

        }

        void testArray()
        {
            std::stringstream data;

            std::vector<int> intvector;
            intvector.push_back(4711);
            intvector.push_back(4712);
            intvector.push_back(-3);
            intvector.push_back(-257);

            data << cxxtools::bin::Bin(intvector);
            log_debug("intvector:\n" << cxxtools::hexDump(data.str()));

            std::vector<int> intvector2;
            data >> cxxtools::bin::Bin(intvector2);

            CXXTOOLS_UNIT_ASSERT_EQUALS(intvector.size(), intvector2.size());
            CXXTOOLS_UNIT_ASSERT_EQUALS(intvector[0], intvector2[0]);
            CXXTOOLS_UNIT_ASSERT_EQUALS(intvector[1], intvector2[1]);
            CXXTOOLS_UNIT_ASSERT_EQUALS(intvector[2], intvector2[2]);
            CXXTOOLS_UNIT_ASSERT_EQUALS(intvector[3], intvector2[3]);
        }

        void testObject()
        {
            std::stringstream data;

            TestObject obj;
            obj.intValue = 17;
            obj.stringValue = "foobar";
            obj.doubleValue = 3.125;
            obj.boolValue = true;
            obj.nullValue = true;

            data << cxxtools::bin::Bin(obj);
            log_debug("bindata testobject:\n" << cxxtools::hexDump(data.str()));

            TestObject obj2;
            data >> cxxtools::bin::Bin(obj2);

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

            data << cxxtools::bin::Bin(v);
            log_debug("bindata complex object:\n" << cxxtools::hexDump(data.str()));

            std::vector<TestObject2> v2;
            data >> cxxtools::bin::Bin(v2);

            CXXTOOLS_UNIT_ASSERT(v == v2);
        }

        void testObjectVector()
        {
            std::stringstream data;

            std::vector<TestObject> obj;
            obj.resize(2);
            obj[0].intValue = 17;
            obj[0].stringValue = "foobar";
            obj[0].doubleValue = 3.125;
            obj[0].boolValue = true;
            obj[0].nullValue = true;
            obj[1].intValue = 18;
            obj[1].stringValue = "hi there";
            obj[1].doubleValue = -17.25;
            obj[1].boolValue = false;
            obj[1].nullValue = true;

            data << cxxtools::bin::Bin(obj);

            std::vector<TestObject> obj2;
            data >> cxxtools::bin::Bin(obj2);

            CXXTOOLS_UNIT_ASSERT_EQUALS(obj2.size(), 2);
            CXXTOOLS_UNIT_ASSERT_EQUALS(obj[0].intValue, obj2[0].intValue);
            CXXTOOLS_UNIT_ASSERT_EQUALS(obj[0].stringValue, obj2[0].stringValue);
            CXXTOOLS_UNIT_ASSERT_EQUALS(obj[0].doubleValue, obj2[0].doubleValue);
            CXXTOOLS_UNIT_ASSERT_EQUALS(obj[0].boolValue, obj2[0].boolValue);
            CXXTOOLS_UNIT_ASSERT_EQUALS(obj[0].nullValue, obj2[0].nullValue);
            CXXTOOLS_UNIT_ASSERT_EQUALS(obj[1].intValue, obj2[1].intValue);
            CXXTOOLS_UNIT_ASSERT_EQUALS(obj[1].stringValue, obj2[1].stringValue);
            CXXTOOLS_UNIT_ASSERT_EQUALS(obj[1].doubleValue, obj2[1].doubleValue);
            CXXTOOLS_UNIT_ASSERT_EQUALS(obj[1].boolValue, obj2[1].boolValue);
            CXXTOOLS_UNIT_ASSERT_EQUALS(obj[1].nullValue, obj2[1].nullValue);
            CXXTOOLS_UNIT_ASSERT(obj == obj2);
        }

        void testBinaryData()
        {
            std::stringstream data;

            std::string v;
            for (unsigned n = 0; n < 1024; ++n)
                v.push_back(static_cast<char>(n));

            data << cxxtools::bin::Bin(v);

            std::string v2;
            data >> cxxtools::bin::Bin(v2);

            CXXTOOLS_UNIT_ASSERT_EQUALS(v2.size(), 1024);
            CXXTOOLS_UNIT_ASSERT(v == v2);

            for (unsigned n = 0; n < 0xffff; ++n)
                v.push_back(static_cast<char>(n));

            data << cxxtools::bin::Bin(v);
            data >> cxxtools::bin::Bin(v2);

            CXXTOOLS_UNIT_ASSERT_EQUALS(v.size(), 0xffff + 1024);
            CXXTOOLS_UNIT_ASSERT_EQUALS(v2.size(), 0xffff + 1024);
            CXXTOOLS_UNIT_ASSERT(v == v2);

        }

        void testReuse();
        void testNamedVector();

        template <typename TS>
        static void testTs(TS t)
        {
            std::stringstream s;
            s << cxxtools::bin::Bin(t);

            TS ts;
            s >> cxxtools::bin::Bin(ts);

            CXXTOOLS_UNIT_ASSERT_EQUALS(t, ts);
            log_info(s.str().size());
        }

        void testTimespan()
        {
            testTs(cxxtools::Microseconds(34));
            testTs(cxxtools::Milliseconds(124));
            testTs(cxxtools::Seconds(cxxtools::Microseconds(34565432)));
            testTs(cxxtools::Seconds(3456));
            testTs(cxxtools::Minutes(18));
            testTs(cxxtools::Hours(67));
        }
};

void BinSerializerTest::testReuse()
{
    cxxtools::bin::Deserializer d;

    {
        std::stringstream data;
        cxxtools::SerializationInfo si;
        si.addMember("foo") <<= "bar";
        data << cxxtools::bin::Bin(si);

        d.read(data);

        cxxtools::SerializationInfo si2;
        d.deserialize(si2);

        std::string bar;
        CXXTOOLS_UNIT_ASSERT_EQUALS(si2.memberCount(), 1);
        CXXTOOLS_UNIT_ASSERT_NOTHROW(si2.getMember("foo") >>= bar);
        CXXTOOLS_UNIT_ASSERT_EQUALS(bar, "bar");
    }

    {
        std::stringstream data;
        cxxtools::SerializationInfo si;
        si.addMember("bar").addMember("bar") <<= 42;
        data << cxxtools::bin::Bin(si);

        d.read(data);

        cxxtools::SerializationInfo si2;
        d.deserialize(si2);

        int bar;
        CXXTOOLS_UNIT_ASSERT_EQUALS(si2.memberCount(), 1);
        CXXTOOLS_UNIT_ASSERT_NOTHROW(si2.getMember("bar").getMember("bar") >>= bar);
        CXXTOOLS_UNIT_ASSERT_EQUALS(bar, 42);
    }

}

namespace foo
{
    typedef std::vector<int> FooVector;

    void operator<<= (cxxtools::SerializationInfo& si, const FooVector& iv)
    {
        cxxtools::operator<<= (si, iv);
        si.setTypeName("FooVector");
    }

    struct Foo
    {
        FooVector data;
    };

    void operator<<= (cxxtools::SerializationInfo& si, const Foo& foo)
    {
        si.addMember("data") <<= foo.data;
        si.setTypeName("Foo");
    }

    void operator>>= (const cxxtools::SerializationInfo& si, Foo& foo)
    {
        si.getMember("data") >>= foo.data;
    }
}

void BinSerializerTest::testNamedVector()
{
    foo::Foo f;
    f.data.push_back(42);
    f.data.push_back(12);

    std::stringstream data;
    data << cxxtools::bin::Bin(f);

    log_debug("named foovector:\n" << cxxtools::hexDump(data.str()));

    foo::Foo f2;
    cxxtools::bin::Deserializer d(data);
    d.deserialize(f2);

    CXXTOOLS_UNIT_ASSERT_EQUALS(f2.data.size(), 2);
    CXXTOOLS_UNIT_ASSERT_EQUALS(f2.data[0], 42);
    CXXTOOLS_UNIT_ASSERT_EQUALS(f2.data[1], 12);
}

cxxtools::unit::RegisterTest<BinSerializerTest> register_BinSerializerTest;
