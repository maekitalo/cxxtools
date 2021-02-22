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

#include "cxxtools/serializationinfo.h"
#include "cxxtools/serializationerror.h"
#include "cxxtools/unit/testsuite.h"
#include "cxxtools/unit/registertest.h"
#include "cxxtools/convert.h"
#include "cxxtools/log.h"

log_define("cxxtools.unit.serializationinfo")

#if __cplusplus >= 201103L
#include <utility>
#endif

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
            registerMethod("testString", *this, &SerializationInfoTest::testString);
            registerMethod("testString8", *this, &SerializationInfoTest::testString8);
            registerMethod("testChar", *this, &SerializationInfoTest::testChar);
            registerMethod("testInt", *this, &SerializationInfoTest::testInt);
            registerMethod("testUInt", *this, &SerializationInfoTest::testUInt);
            registerMethod("testFloat", *this, &SerializationInfoTest::testFloat);
            registerMethod("testDouble", *this, &SerializationInfoTest::testDouble);
            registerMethod("testLongDouble", *this, &SerializationInfoTest::testLongDouble);
            registerMethod("testSiAssign", *this, &SerializationInfoTest::testSiAssign);
            registerMethod("testSiCopy", *this, &SerializationInfoTest::testSiCopy);
            registerMethod("testSiSwap", *this, &SerializationInfoTest::testSiSwap);
#if __cplusplus >= 201103L
            registerMethod("testMove", *this, &SerializationInfoTest::testMove);
#endif
            registerMethod("testStringToBool", *this, &SerializationInfoTest::testStringToBool);
            registerMethod("testMember", *this, &SerializationInfoTest::testMember);
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
            CXXTOOLS_UNIT_ASSERT_EQUALS(siValue<short>(si), -17);

            si.setValue(0.0);
            CXXTOOLS_UNIT_ASSERT(si.isDouble());
            CXXTOOLS_UNIT_ASSERT_EQUALS(cxxtools::convert<double>(siValue<std::string>(si)), 0);
            CXXTOOLS_UNIT_ASSERT_EQUALS(cxxtools::convert<double>(siValue<cxxtools::String>(si)), 0);
            CXXTOOLS_UNIT_ASSERT_EQUALS(siValue<unsigned>(si), 0);
            CXXTOOLS_UNIT_ASSERT_EQUALS(siValue<short>(si), 0);
            CXXTOOLS_UNIT_ASSERT_EQUALS(siValue<double>(si), 0);
        }

        void testString()
        {
            cxxtools::SerializationInfo si;
            si.setValue(cxxtools::String(L"42"));

            CXXTOOLS_UNIT_ASSERT(si.isString());
            CXXTOOLS_UNIT_ASSERT(!si.isString8());
            CXXTOOLS_UNIT_ASSERT(!si.isChar());
            CXXTOOLS_UNIT_ASSERT(!si.isInt());
            CXXTOOLS_UNIT_ASSERT(!si.isUInt());
            CXXTOOLS_UNIT_ASSERT(!si.isFloat());
            CXXTOOLS_UNIT_ASSERT(!si.isDouble());
            CXXTOOLS_UNIT_ASSERT(!si.isLongDouble());

            CXXTOOLS_UNIT_ASSERT_EQUALS(siValue<cxxtools::String>(si), cxxtools::String(L"42"));
            CXXTOOLS_UNIT_ASSERT_EQUALS(siValue<std::string>(si), "42");
            CXXTOOLS_UNIT_ASSERT_EQUALS(siValue<char>(si), '4');
            CXXTOOLS_UNIT_ASSERT_EQUALS(siValue<unsigned char>(si), static_cast<unsigned char>(42));
            CXXTOOLS_UNIT_ASSERT_EQUALS(siValue<bool>(si), true);
            CXXTOOLS_UNIT_ASSERT_EQUALS(siValue<int>(si), 42);
            CXXTOOLS_UNIT_ASSERT_EQUALS(siValue<double>(si), 42.0);
        }

        void testString8()
        {
            cxxtools::SerializationInfo si;
            si.setValue(std::string("42"));

            CXXTOOLS_UNIT_ASSERT(!si.isString());
            CXXTOOLS_UNIT_ASSERT(si.isString8());
            CXXTOOLS_UNIT_ASSERT(!si.isChar());
            CXXTOOLS_UNIT_ASSERT(!si.isInt());
            CXXTOOLS_UNIT_ASSERT(!si.isUInt());
            CXXTOOLS_UNIT_ASSERT(!si.isFloat());
            CXXTOOLS_UNIT_ASSERT(!si.isDouble());
            CXXTOOLS_UNIT_ASSERT(!si.isLongDouble());

            CXXTOOLS_UNIT_ASSERT_EQUALS(siValue<cxxtools::String>(si), cxxtools::String(L"42"));
            CXXTOOLS_UNIT_ASSERT_EQUALS(siValue<std::string>(si), "42");
            CXXTOOLS_UNIT_ASSERT_EQUALS(siValue<char>(si), '4');
            CXXTOOLS_UNIT_ASSERT_EQUALS(siValue<unsigned char>(si), static_cast<unsigned char>(42));
            CXXTOOLS_UNIT_ASSERT_EQUALS(siValue<bool>(si), true);
            CXXTOOLS_UNIT_ASSERT_EQUALS(siValue<int>(si), 42);
            CXXTOOLS_UNIT_ASSERT_EQUALS(siValue<double>(si), 42.0);
        }

        void testChar()
        {
            cxxtools::SerializationInfo si;
            si.setValue('7');

            CXXTOOLS_UNIT_ASSERT(!si.isString());
            CXXTOOLS_UNIT_ASSERT(!si.isString8());
            CXXTOOLS_UNIT_ASSERT(si.isChar());
            CXXTOOLS_UNIT_ASSERT(!si.isInt());
            CXXTOOLS_UNIT_ASSERT(!si.isUInt());
            CXXTOOLS_UNIT_ASSERT(!si.isFloat());
            CXXTOOLS_UNIT_ASSERT(!si.isDouble());
            CXXTOOLS_UNIT_ASSERT(!si.isLongDouble());

            CXXTOOLS_UNIT_ASSERT_EQUALS(siValue<cxxtools::String>(si), cxxtools::String(L"7"));
            CXXTOOLS_UNIT_ASSERT_EQUALS(siValue<std::string>(si), "7");
            CXXTOOLS_UNIT_ASSERT_EQUALS(siValue<char>(si), '7');
            CXXTOOLS_UNIT_ASSERT_EQUALS(siValue<unsigned char>(si), static_cast<unsigned char>(7));
            CXXTOOLS_UNIT_ASSERT_EQUALS(siValue<bool>(si), true);
            CXXTOOLS_UNIT_ASSERT_EQUALS(siValue<int>(si), 7);
            CXXTOOLS_UNIT_ASSERT_EQUALS(siValue<double>(si), 7.0);
        }

        void testInt()
        {
            cxxtools::SerializationInfo si;
            si.setValue(42);

            CXXTOOLS_UNIT_ASSERT(!si.isString());
            CXXTOOLS_UNIT_ASSERT(!si.isString8());
            CXXTOOLS_UNIT_ASSERT(!si.isChar());
            CXXTOOLS_UNIT_ASSERT(si.isInt());
            CXXTOOLS_UNIT_ASSERT(!si.isUInt());
            CXXTOOLS_UNIT_ASSERT(!si.isFloat());
            CXXTOOLS_UNIT_ASSERT(!si.isDouble());
            CXXTOOLS_UNIT_ASSERT(!si.isLongDouble());

            CXXTOOLS_UNIT_ASSERT_EQUALS(siValue<cxxtools::String>(si), cxxtools::String(L"42"));
            CXXTOOLS_UNIT_ASSERT_EQUALS(siValue<std::string>(si), "42");
            CXXTOOLS_UNIT_ASSERT_EQUALS(siValue<char>(si), 42);
            CXXTOOLS_UNIT_ASSERT_EQUALS(siValue<unsigned char>(si), static_cast<unsigned char>(42));
            CXXTOOLS_UNIT_ASSERT_EQUALS(siValue<bool>(si), true);
            CXXTOOLS_UNIT_ASSERT_EQUALS(siValue<int>(si), 42);
            CXXTOOLS_UNIT_ASSERT_EQUALS(siValue<double>(si), 42.0);
        }

        void testUInt()
        {
            cxxtools::SerializationInfo si;
            si.setValue(static_cast<unsigned int>(42));

            CXXTOOLS_UNIT_ASSERT(!si.isString());
            CXXTOOLS_UNIT_ASSERT(!si.isString8());
            CXXTOOLS_UNIT_ASSERT(!si.isChar());
            CXXTOOLS_UNIT_ASSERT(!si.isInt());
            CXXTOOLS_UNIT_ASSERT(si.isUInt());
            CXXTOOLS_UNIT_ASSERT(!si.isFloat());
            CXXTOOLS_UNIT_ASSERT(!si.isDouble());
            CXXTOOLS_UNIT_ASSERT(!si.isLongDouble());

            CXXTOOLS_UNIT_ASSERT_EQUALS(siValue<cxxtools::String>(si), cxxtools::String(L"42"));
            CXXTOOLS_UNIT_ASSERT_EQUALS(siValue<std::string>(si), "42");
            CXXTOOLS_UNIT_ASSERT_EQUALS(siValue<char>(si), 42);
            CXXTOOLS_UNIT_ASSERT_EQUALS(siValue<unsigned char>(si), static_cast<unsigned char>(42));
            CXXTOOLS_UNIT_ASSERT_EQUALS(siValue<bool>(si), true);
            CXXTOOLS_UNIT_ASSERT_EQUALS(siValue<int>(si), 42);
            CXXTOOLS_UNIT_ASSERT_EQUALS(siValue<double>(si), 42.0);
        }

        void testFloat()
        {
            cxxtools::SerializationInfo si;
            si.setValue(static_cast<float>(48.0));

            CXXTOOLS_UNIT_ASSERT(!si.isString());
            CXXTOOLS_UNIT_ASSERT(!si.isString8());
            CXXTOOLS_UNIT_ASSERT(!si.isChar());
            CXXTOOLS_UNIT_ASSERT(!si.isInt());
            CXXTOOLS_UNIT_ASSERT(!si.isUInt());
            CXXTOOLS_UNIT_ASSERT(si.isFloat());
            CXXTOOLS_UNIT_ASSERT(!si.isDouble());
            CXXTOOLS_UNIT_ASSERT(!si.isLongDouble());

            CXXTOOLS_UNIT_ASSERT_EQUALS(cxxtools::convert<float>(siValue<std::string>(si)), 48);
            CXXTOOLS_UNIT_ASSERT_EQUALS(cxxtools::convert<float>(siValue<cxxtools::String>(si)), 48);
            CXXTOOLS_UNIT_ASSERT_EQUALS(siValue<char>(si), 48);
            CXXTOOLS_UNIT_ASSERT_EQUALS(siValue<unsigned char>(si), static_cast<unsigned char>(48));
            CXXTOOLS_UNIT_ASSERT_EQUALS(siValue<bool>(si), true);
            CXXTOOLS_UNIT_ASSERT_EQUALS(siValue<int>(si), 48);
            CXXTOOLS_UNIT_ASSERT_EQUALS(siValue<float>(si), 48.0);
        }

        void testDouble()
        {
            cxxtools::SerializationInfo si;
            si.setValue(48.0);

            CXXTOOLS_UNIT_ASSERT(!si.isString());
            CXXTOOLS_UNIT_ASSERT(!si.isString8());
            CXXTOOLS_UNIT_ASSERT(!si.isChar());
            CXXTOOLS_UNIT_ASSERT(!si.isInt());
            CXXTOOLS_UNIT_ASSERT(!si.isUInt());
            CXXTOOLS_UNIT_ASSERT(!si.isFloat());
            CXXTOOLS_UNIT_ASSERT(si.isDouble());
            CXXTOOLS_UNIT_ASSERT(!si.isLongDouble());

            CXXTOOLS_UNIT_ASSERT_EQUALS(cxxtools::convert<double>(siValue<std::string>(si)), 48);
            CXXTOOLS_UNIT_ASSERT_EQUALS(cxxtools::convert<double>(siValue<cxxtools::String>(si)), 48);
            CXXTOOLS_UNIT_ASSERT_EQUALS(siValue<char>(si), 48);
            CXXTOOLS_UNIT_ASSERT_EQUALS(siValue<unsigned char>(si), static_cast<unsigned char>(48));
            CXXTOOLS_UNIT_ASSERT_EQUALS(siValue<bool>(si), true);
            CXXTOOLS_UNIT_ASSERT_EQUALS(siValue<int>(si), 48);
            CXXTOOLS_UNIT_ASSERT_EQUALS(siValue<double>(si), 48.0);
        }

        void testLongDouble()
        {
            cxxtools::SerializationInfo si;
            si.setValue(48.0l);

            CXXTOOLS_UNIT_ASSERT(!si.isString());
            CXXTOOLS_UNIT_ASSERT(!si.isString8());
            CXXTOOLS_UNIT_ASSERT(!si.isChar());
            CXXTOOLS_UNIT_ASSERT(!si.isInt());
            CXXTOOLS_UNIT_ASSERT(!si.isUInt());
            CXXTOOLS_UNIT_ASSERT(!si.isFloat());
            CXXTOOLS_UNIT_ASSERT(!si.isDouble());
            CXXTOOLS_UNIT_ASSERT(si.isLongDouble());

            CXXTOOLS_UNIT_ASSERT_EQUALS(cxxtools::convert<long double>(siValue<std::string>(si)), 48);
            CXXTOOLS_UNIT_ASSERT_EQUALS(cxxtools::convert<long double>(siValue<cxxtools::String>(si)), 48);
            CXXTOOLS_UNIT_ASSERT_EQUALS(siValue<char>(si), 48);
            CXXTOOLS_UNIT_ASSERT_EQUALS(siValue<unsigned char>(si), static_cast<unsigned char>(48));
            CXXTOOLS_UNIT_ASSERT_EQUALS(siValue<bool>(si), true);
            CXXTOOLS_UNIT_ASSERT_EQUALS(siValue<int>(si), 48);
            CXXTOOLS_UNIT_ASSERT_EQUALS(siValue<long double>(si), 48.0);
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
                CXXTOOLS_UNIT_ASSERT_EQUALS(siValue<short>(si2), -17);
            }

        }

#if __cplusplus >= 201103L
        void testMove()
        {
            {
                cxxtools::SerializationInfo si;
                si.setValue(static_cast<unsigned short>(42));
                si.addMember("foo");
                si.addMember("bar");

                cxxtools::SerializationInfo si2(std::move(si));

                CXXTOOLS_UNIT_ASSERT(si2.isUInt());
                CXXTOOLS_UNIT_ASSERT_EQUALS(siValue<unsigned>(si2), 42);
                CXXTOOLS_UNIT_ASSERT_EQUALS(si.memberCount(), 0);
                CXXTOOLS_UNIT_ASSERT_EQUALS(si2.memberCount(), 2);
            }

            {
                cxxtools::SerializationInfo si;
                const char str[] = "this is a string with aribtrary content";
                si.setValue(str);

                cxxtools::SerializationInfo si2;
                si2.setValue(L"string to be replaced");
                si2 = std::move(si);
                CXXTOOLS_UNIT_ASSERT_EQUALS(siValue<std::string>(si2), str);
                CXXTOOLS_UNIT_ASSERT(siValue<std::string>(si) != str);
            }

            {
                cxxtools::SerializationInfo si;
                const char str[] = "this is a string with aribtrary content";
                si.setValue(str);

                cxxtools::SerializationInfo si2(std::move(si));
                CXXTOOLS_UNIT_ASSERT_EQUALS(siValue<std::string>(si2), str);
                CXXTOOLS_UNIT_ASSERT(siValue<std::string>(si) != str);
            }

        }

#endif

        void testSiSwap()
        {
            cxxtools::SerializationInfo si;
            cxxtools::SerializationInfo si1;
            cxxtools::SerializationInfo si2;

            // simple type <-> simple type
            si1.setValue(17);
            si2.setValue(1.5);
            si1.swap(si2);
            CXXTOOLS_UNIT_ASSERT(si1.isDouble());
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
            CXXTOOLS_UNIT_ASSERT_EQUALS(siValue<cxxtools::String>(si1), cxxtools::String(L"World"));
            CXXTOOLS_UNIT_ASSERT(si2.isInt());
            CXXTOOLS_UNIT_ASSERT_EQUALS(siValue<int>(si2), 19);

            // std::string <-> simple type
            si1.setValue("Hi");
            si2.setValue(1.5);
            si1.swap(si2);
            CXXTOOLS_UNIT_ASSERT(si1.isDouble());
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
            CXXTOOLS_UNIT_ASSERT_EQUALS(siValue<cxxtools::String>(si1), cxxtools::String(L"Bar"));
            CXXTOOLS_UNIT_ASSERT(si2.isString8());
            CXXTOOLS_UNIT_ASSERT_EQUALS(siValue<std::string>(si2), "Foo");

            // String <-> simple type
            si1.setValue(cxxtools::String("Hi"));
            si2.setValue(1.5);
            si1.swap(si2);
            CXXTOOLS_UNIT_ASSERT(si1.isDouble());
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
            CXXTOOLS_UNIT_ASSERT_EQUALS(siValue<cxxtools::String>(si1), cxxtools::String(L"Bar"));
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
            CXXTOOLS_UNIT_ASSERT_EQUALS(siValue<short>(si2), -17);
        }

        void testStringToBool()
        {
            cxxtools::SerializationInfo si;

            si.setValue("78");
            CXXTOOLS_UNIT_ASSERT(siValue<bool>(si));
        }

        void testMember()
        {
            cxxtools::SerializationInfo si;
            si.addMember("foo");
            si.addMember("bar");
            si.addMember("baz");
            si.addMember("foo");

            CXXTOOLS_UNIT_ASSERT_EQUALS(si.memberCount(), 4);
            CXXTOOLS_UNIT_ASSERT_NOTHROW(si.getMember("foo"));
            CXXTOOLS_UNIT_ASSERT_THROW(si.getMember("fooo"), cxxtools::SerializationError);
            CXXTOOLS_UNIT_ASSERT_EQUALS(si.getMember(0).name(), "foo");
            CXXTOOLS_UNIT_ASSERT_EQUALS(si.getMember(1).name(), "bar");
            CXXTOOLS_UNIT_ASSERT_EQUALS(si.getMember(2).name(), "baz");
            CXXTOOLS_UNIT_ASSERT_EQUALS(si.getMember(3).name(), "foo");
        }
};

cxxtools::unit::RegisterTest<SerializationInfoTest> register_SerializationInfoTest;
