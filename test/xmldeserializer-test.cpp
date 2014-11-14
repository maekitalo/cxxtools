/*
 * Copyright (C) 2014 Tommi Maekitalo
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
#include "cxxtools/xml/xmldeserializer.h"
#include "cxxtools/log.h"
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

}

class XmlDeserializerTest : public cxxtools::unit::TestSuite
{
    public:
        XmlDeserializerTest()
            : cxxtools::unit::TestSuite("xmldeserializer")
        {
            registerMethod("testObjectWithAttributes", *this, &XmlDeserializerTest::testObjectWithAttributes);
            registerMethod("testManyObjectsWithAttributes", *this, &XmlDeserializerTest::testManyObjectsWithAttributes);
        }

        void testObjectWithAttributes()
        {
            std::istringstream data(
                "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
                //"<object intValue=\"42\" stringValue=\"Hi\" doubleValue=\"2.25\" boolValue=\"true\"><foo></foo></object>");
                "<object intValue=\"42\" stringValue=\"Hi\" doubleValue=\"2.25\" boolValue=\"true\"/>");

            cxxtools::xml::XmlDeserializer d(data, true);

            TestObject t;
            d.deserialize(t);

            CXXTOOLS_UNIT_ASSERT_EQUALS(t.intValue, 42);
            CXXTOOLS_UNIT_ASSERT_EQUALS(t.stringValue, "Hi");
            CXXTOOLS_UNIT_ASSERT_EQUALS(t.doubleValue, 2.25);
            CXXTOOLS_UNIT_ASSERT_EQUALS(t.boolValue, true);
        }

        void testManyObjectsWithAttributes()
        {
            std::istringstream data(
                "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
                "<root>\n"
                "<object intValue=\"42\" stringValue=\"Hi\" doubleValue=\"2.25\" boolValue=\"true\"/>\n"
                "<object intValue=\"43\" stringValue=\"There\" doubleValue=\"17\" boolValue=\"false\"/>\n"
                "</root>");

            cxxtools::xml::XmlDeserializer d(data, true);

            std::vector<TestObject> t;
            d.deserialize(t);

            CXXTOOLS_UNIT_ASSERT_EQUALS(t.size(), 2);
            CXXTOOLS_UNIT_ASSERT_EQUALS(t[0].intValue, 42);
            CXXTOOLS_UNIT_ASSERT_EQUALS(t[0].stringValue, "Hi");
            CXXTOOLS_UNIT_ASSERT_EQUALS(t[0].doubleValue, 2.25);
            CXXTOOLS_UNIT_ASSERT_EQUALS(t[0].boolValue, true);
            CXXTOOLS_UNIT_ASSERT_EQUALS(t[1].intValue, 43);
            CXXTOOLS_UNIT_ASSERT_EQUALS(t[1].stringValue, "There");
            CXXTOOLS_UNIT_ASSERT_EQUALS(t[1].doubleValue, 17);
            CXXTOOLS_UNIT_ASSERT_EQUALS(t[1].boolValue, false);
        }

};

cxxtools::unit::RegisterTest<XmlDeserializerTest> register_XmlDeserializerTest;
