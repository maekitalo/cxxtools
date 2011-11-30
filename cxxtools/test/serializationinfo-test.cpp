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

#include <iostream>
#include "cxxtools/serializationinfo.h"
#include "cxxtools/unit/testsuite.h"
#include "cxxtools/unit/registertest.h"

template <typename T>
T siValue(const cxxtools::SerializationInfo& si)
{
    T value;
    si.getValue(value);
    return value;
}

class SerializationInfoTest : public cxxtools::unit::TestSuite
{

    public:
        SerializationInfoTest()
        : cxxtools::unit::TestSuite("serializationinfo")
        {
            registerMethod("testSiSet", *this, &SerializationInfoTest::testSiSet);
            registerMethod("testSiAssign", *this, &SerializationInfoTest::testSiAssign);
            registerMethod("testSiCopy", *this, &SerializationInfoTest::testSiCopy);
            registerMethod("testSiSwap", *this, &SerializationInfoTest::testSiSwap);
        }

        void testSiSet()
        {
            cxxtools::SerializationInfo si;

            si.setValue(static_cast<unsigned short>(42));
            CXXTOOLS_UNIT_ASSERT(si.isUInt());
            CXXTOOLS_UNIT_ASSERT_EQUALS(siValue<std::string>(si), "42");
            CXXTOOLS_UNIT_ASSERT_EQUALS(siValue<cxxtools::String>(si), cxxtools::String(L"42"));
            CXXTOOLS_UNIT_ASSERT_EQUALS(siValue<unsigned>(si), 42);
            CXXTOOLS_UNIT_ASSERT_EQUALS(siValue<short>(si), 42);

            si.setValue("Hello");
            CXXTOOLS_UNIT_ASSERT(si.isString8());
            CXXTOOLS_UNIT_ASSERT_EQUALS(siValue<std::string>(si), "Hello");
            CXXTOOLS_UNIT_ASSERT_EQUALS(siValue<cxxtools::String>(si), cxxtools::String(L"Hello"));

            si.setValue(cxxtools::String(L"World"));
            CXXTOOLS_UNIT_ASSERT(si.isString());
            CXXTOOLS_UNIT_ASSERT_EQUALS(siValue<std::string>(si), "World");
            CXXTOOLS_UNIT_ASSERT_EQUALS(siValue<cxxtools::String>(si), cxxtools::String(L"World"));

            si.setValue(-17);
            CXXTOOLS_UNIT_ASSERT(si.isInt());
            CXXTOOLS_UNIT_ASSERT_EQUALS(siValue<std::string>(si), "-17");
            CXXTOOLS_UNIT_ASSERT_EQUALS(siValue<cxxtools::String>(si), cxxtools::String(L"-17"));
            CXXTOOLS_UNIT_ASSERT_EQUALS(siValue<unsigned>(si), -17);
            CXXTOOLS_UNIT_ASSERT_EQUALS(siValue<short>(si), -17);

        }

        void testSiAssign()
        {
            cxxtools::SerializationInfo si;
            cxxtools::SerializationInfo si2;

            si.setValue(static_cast<unsigned short>(42));
            si2 = si;
            CXXTOOLS_UNIT_ASSERT(si2.isUInt());
            CXXTOOLS_UNIT_ASSERT_EQUALS(siValue<std::string>(si2), "42");
            CXXTOOLS_UNIT_ASSERT_EQUALS(siValue<cxxtools::String>(si2), cxxtools::String(L"42"));
            CXXTOOLS_UNIT_ASSERT_EQUALS(siValue<unsigned>(si2), 42);
            CXXTOOLS_UNIT_ASSERT_EQUALS(siValue<short>(si2), 42);

            si.setValue("Hello");
            si2 = si;
            CXXTOOLS_UNIT_ASSERT(si2.isString8());
            CXXTOOLS_UNIT_ASSERT_EQUALS(siValue<std::string>(si2), "Hello");
            CXXTOOLS_UNIT_ASSERT_EQUALS(siValue<cxxtools::String>(si2), cxxtools::String(L"Hello"));

            si.setValue(cxxtools::String(L"World"));
            si2 = si;
            CXXTOOLS_UNIT_ASSERT(si2.isString());
            CXXTOOLS_UNIT_ASSERT_EQUALS(siValue<std::string>(si2), "World");
            CXXTOOLS_UNIT_ASSERT_EQUALS(siValue<cxxtools::String>(si2), cxxtools::String(L"World"));

            si.setValue(-17);
            si2 = si;
            CXXTOOLS_UNIT_ASSERT(si2.isInt());
            CXXTOOLS_UNIT_ASSERT_EQUALS(siValue<std::string>(si2), "-17");
            CXXTOOLS_UNIT_ASSERT_EQUALS(siValue<cxxtools::String>(si2), cxxtools::String(L"-17"));
            CXXTOOLS_UNIT_ASSERT_EQUALS(siValue<unsigned>(si2), -17);
            CXXTOOLS_UNIT_ASSERT_EQUALS(siValue<short>(si2), -17);

        }

        void testSiCopy()
        {
            cxxtools::SerializationInfo si;

            {
                si.setValue(static_cast<unsigned short>(42));
                cxxtools::SerializationInfo si2(si);
                CXXTOOLS_UNIT_ASSERT(si2.isUInt());
                CXXTOOLS_UNIT_ASSERT_EQUALS(siValue<std::string>(si2), "42");
                CXXTOOLS_UNIT_ASSERT_EQUALS(siValue<cxxtools::String>(si2), cxxtools::String(L"42"));
                CXXTOOLS_UNIT_ASSERT_EQUALS(siValue<unsigned>(si2), 42);
                CXXTOOLS_UNIT_ASSERT_EQUALS(siValue<short>(si2), 42);
            }

            {
                si.setValue("Hello");
                cxxtools::SerializationInfo si2(si);
                CXXTOOLS_UNIT_ASSERT(si2.isString8());
                CXXTOOLS_UNIT_ASSERT_EQUALS(siValue<std::string>(si2), "Hello");
                CXXTOOLS_UNIT_ASSERT_EQUALS(siValue<cxxtools::String>(si2), cxxtools::String(L"Hello"));
            }

            {
                si.setValue(cxxtools::String(L"World"));
                cxxtools::SerializationInfo si2(si);
                CXXTOOLS_UNIT_ASSERT(si2.isString());
                CXXTOOLS_UNIT_ASSERT_EQUALS(siValue<std::string>(si2), "World");
                CXXTOOLS_UNIT_ASSERT_EQUALS(siValue<cxxtools::String>(si2), cxxtools::String(L"World"));
            }

            {
                si.setValue(-17);
                cxxtools::SerializationInfo si2(si);
                CXXTOOLS_UNIT_ASSERT(si2.isInt());
                CXXTOOLS_UNIT_ASSERT_EQUALS(siValue<std::string>(si2), "-17");
                CXXTOOLS_UNIT_ASSERT_EQUALS(siValue<cxxtools::String>(si2), cxxtools::String(L"-17"));
                CXXTOOLS_UNIT_ASSERT_EQUALS(siValue<unsigned>(si2), -17);
                CXXTOOLS_UNIT_ASSERT_EQUALS(siValue<short>(si2), -17);
            }

        }

        void testSiSwap()
        {
            cxxtools::SerializationInfo si;
            cxxtools::SerializationInfo si1;
            cxxtools::SerializationInfo si2;

            // simple type <-> simple type
            si1.setValue(17);
            si2.setValue(1.5);
            si1.swap(si2);
            CXXTOOLS_UNIT_ASSERT(si1.isFloat());
            CXXTOOLS_UNIT_ASSERT_EQUALS(siValue<double>(si1), 1.5);
            CXXTOOLS_UNIT_ASSERT(si2.isInt());
            CXXTOOLS_UNIT_ASSERT_EQUALS(siValue<int>(si2), 17);

            // simple type <-> std::string
            si1.setValue(18);
            si2.setValue("Hello");
            si1.swap(si2);
            CXXTOOLS_UNIT_ASSERT(si1.isString8());
            CXXTOOLS_UNIT_ASSERT_EQUALS(siValue<std::string>(si1), "Hello");
            CXXTOOLS_UNIT_ASSERT(si2.isInt());
            CXXTOOLS_UNIT_ASSERT_EQUALS(siValue<int>(si2), 18);

            // simple type <-> String
            si1.setValue(19);
            si2.setValue(cxxtools::String(L"World"));
            si1.swap(si2);
            CXXTOOLS_UNIT_ASSERT(si1.isString());
            CXXTOOLS_UNIT_ASSERT_EQUALS(siValue<cxxtools::String>(si1), L"World");
            CXXTOOLS_UNIT_ASSERT(si2.isInt());
            CXXTOOLS_UNIT_ASSERT_EQUALS(siValue<int>(si2), 19);

            // std::string <-> simple type
            si1.setValue("Hi");
            si2.setValue(1.5);
            si1.swap(si2);
            CXXTOOLS_UNIT_ASSERT(si1.isFloat());
            CXXTOOLS_UNIT_ASSERT_EQUALS(siValue<double>(si1), 1.5);
            CXXTOOLS_UNIT_ASSERT(si2.isString8());
            CXXTOOLS_UNIT_ASSERT_EQUALS(siValue<std::string>(si2), "Hi");

            // std::string <-> std::string
            si1.setValue("Hello");
            si2.setValue("World");
            si1.swap(si2);
            CXXTOOLS_UNIT_ASSERT(si1.isString8());
            CXXTOOLS_UNIT_ASSERT_EQUALS(siValue<std::string>(si1), "World");
            CXXTOOLS_UNIT_ASSERT(si2.isString8());
            CXXTOOLS_UNIT_ASSERT_EQUALS(siValue<std::string>(si2), "Hello");

            // std::string <-> String
            si1.setValue("Foo");
            si2.setValue(cxxtools::String("Bar"));
            si1.swap(si2);
            CXXTOOLS_UNIT_ASSERT(si1.isString());
            CXXTOOLS_UNIT_ASSERT_EQUALS(siValue<cxxtools::String>(si1), L"Bar");
            CXXTOOLS_UNIT_ASSERT(si2.isString8());
            CXXTOOLS_UNIT_ASSERT_EQUALS(siValue<std::string>(si2), "Foo");

            // String <-> simple type
            si1.setValue(cxxtools::String("Hi"));
            si2.setValue(1.5);
            si1.swap(si2);
            CXXTOOLS_UNIT_ASSERT(si1.isFloat());
            CXXTOOLS_UNIT_ASSERT_EQUALS(siValue<double>(si1), 1.5);
            CXXTOOLS_UNIT_ASSERT(si2.isString());
            CXXTOOLS_UNIT_ASSERT_EQUALS(siValue<std::string>(si2), "Hi");

            // String <-> std::string
            si1.setValue(cxxtools::String(L"Hello"));
            si2.setValue("World");
            si1.swap(si2);
            CXXTOOLS_UNIT_ASSERT(si1.isString8());
            CXXTOOLS_UNIT_ASSERT_EQUALS(siValue<std::string>(si1), "World");
            CXXTOOLS_UNIT_ASSERT(si2.isString());
            CXXTOOLS_UNIT_ASSERT_EQUALS(siValue<std::string>(si2), "Hello");

            // String <-> String
            si1.setValue(cxxtools::String("Foo"));
            si2.setValue(cxxtools::String("Bar"));
            si1.swap(si2);
            CXXTOOLS_UNIT_ASSERT(si1.isString());
            CXXTOOLS_UNIT_ASSERT_EQUALS(siValue<cxxtools::String>(si1), L"Bar");
            CXXTOOLS_UNIT_ASSERT(si2.isString());
            CXXTOOLS_UNIT_ASSERT_EQUALS(siValue<std::string>(si2), "Foo");


            // other random tests

            si.setValue(static_cast<unsigned short>(42));
            si.swap(si2);
            CXXTOOLS_UNIT_ASSERT(si2.isUInt());
            CXXTOOLS_UNIT_ASSERT_EQUALS(siValue<std::string>(si2), "42");
            CXXTOOLS_UNIT_ASSERT_EQUALS(siValue<cxxtools::String>(si2), cxxtools::String(L"42"));
            CXXTOOLS_UNIT_ASSERT_EQUALS(siValue<unsigned>(si2), 42);
            CXXTOOLS_UNIT_ASSERT_EQUALS(siValue<short>(si2), 42);

            si.setValue("Hello");
            si.swap(si2);
            CXXTOOLS_UNIT_ASSERT(si.isUInt());
            CXXTOOLS_UNIT_ASSERT_EQUALS(siValue<short>(si), 42);
            CXXTOOLS_UNIT_ASSERT(si2.isString8());
            CXXTOOLS_UNIT_ASSERT_EQUALS(siValue<std::string>(si2), "Hello");
            CXXTOOLS_UNIT_ASSERT_EQUALS(siValue<cxxtools::String>(si2), cxxtools::String(L"Hello"));

            si.setValue(cxxtools::String(L"World"));
            si.swap(si2);
            CXXTOOLS_UNIT_ASSERT(si.isString8());
            CXXTOOLS_UNIT_ASSERT_EQUALS(siValue<std::string>(si), "Hello");
            CXXTOOLS_UNIT_ASSERT(si2.isString());
            CXXTOOLS_UNIT_ASSERT_EQUALS(siValue<std::string>(si2), "World");
            CXXTOOLS_UNIT_ASSERT_EQUALS(siValue<cxxtools::String>(si2), cxxtools::String(L"World"));

            si.setValue(-17);
            si.swap(si2);
            CXXTOOLS_UNIT_ASSERT(si.isString());
            CXXTOOLS_UNIT_ASSERT(si2.isInt());
            CXXTOOLS_UNIT_ASSERT_EQUALS(siValue<std::string>(si2), "-17");
            CXXTOOLS_UNIT_ASSERT_EQUALS(siValue<cxxtools::String>(si2), cxxtools::String(L"-17"));
            CXXTOOLS_UNIT_ASSERT_EQUALS(siValue<unsigned>(si2), -17);
            CXXTOOLS_UNIT_ASSERT_EQUALS(siValue<short>(si2), -17);
        }

};

cxxtools::unit::RegisterTest<SerializationInfoTest> register_SerializationInfoTest;
