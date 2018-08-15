/*
 * Copyright (C) 2015 Tommi Maekitalo
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
#include "cxxtools/unit/testsuite.h"
#include "cxxtools/unit/registertest.h"

class SerializationTest : public cxxtools::unit::TestSuite
{

    public:
        SerializationTest()
        : cxxtools::unit::TestSuite("serialization")
        {
            registerMethod("testBool", *this, &SerializationTest::testBool);
            registerMethod("testSignedChar", *this, &SerializationTest::testSignedChar);
            registerMethod("testUnsignedChar", *this, &SerializationTest::testUnsignedChar);
            registerMethod("testChar", *this, &SerializationTest::testChar);
            registerMethod("testShort", *this, &SerializationTest::testShort);
            registerMethod("testUnsignedShort", *this, &SerializationTest::testUnsignedShort);
            registerMethod("testInt", *this, &SerializationTest::testInt);
            registerMethod("testUnsignedInt", *this, &SerializationTest::testUnsignedInt);
            registerMethod("testLong", *this, &SerializationTest::testLong);
            registerMethod("testUnsingedLong", *this, &SerializationTest::testUnsingedLong);
            registerMethod("testDouble", *this, &SerializationTest::testDouble);
            registerMethod("testStdString", *this, &SerializationTest::testStdString);
            registerMethod("testUnicodeString", *this, &SerializationTest::testUnicodeString);
            registerMethod("testStringToInt", *this, &SerializationTest::testStringToInt);
            registerMethod("testIntToString", *this, &SerializationTest::testIntToString);
            registerMethod("testVector", *this, &SerializationTest::testVector);
            registerMethod("testBoolVector", *this, &SerializationTest::testBoolVector);
            registerMethod("testList", *this, &SerializationTest::testList);
            registerMethod("testDeque", *this, &SerializationTest::testDeque);
            registerMethod("testSet", *this, &SerializationTest::testSet);
            registerMethod("testMultiset", *this, &SerializationTest::testMultiset);
            registerMethod("testMap", *this, &SerializationTest::testMap);
            registerMethod("testMultimap", *this, &SerializationTest::testMultimap);
            registerMethod("testCArray", *this, &SerializationTest::testCArray);
#if __cplusplus >= 201103L
            registerMethod("testForwardList", *this, &SerializationTest::testForwardList);
            registerMethod("testUnorderedSet", *this, &SerializationTest::testUnorderedSet);
            registerMethod("testUnorderedMultiset", *this, &SerializationTest::testUnorderedMultiset);
            registerMethod("testUnorderedMap", *this, &SerializationTest::testUnorderedMap);
            registerMethod("testUnorderedMultimap", *this, &SerializationTest::testUnorderedMultimap);
            registerMethod("testTuple", *this, &SerializationTest::testTuple);
            registerMethod("testArray", *this, &SerializationTest::testArray);
            registerMethod("testEnum", *this, &SerializationTest::testEnum);
#endif
        }

        void testBool()
        {
            cxxtools::SerializationInfo si;

            bool b = true;
            bool bb = false;

            si <<= b;
            si >>= bb;
            CXXTOOLS_UNIT_ASSERT_EQUALS(b, true);
            CXXTOOLS_UNIT_ASSERT_EQUALS(b, bb);

            b = false;
            si <<= b;
            si >>= bb;
            CXXTOOLS_UNIT_ASSERT_EQUALS(b, false);
            CXXTOOLS_UNIT_ASSERT_EQUALS(b, bb);
        }

        void testSignedChar()
        {
            cxxtools::SerializationInfo si;

            const signed char c = 'h';
            signed char cc = '\0';

            si <<= c;
            si >>= cc;
            CXXTOOLS_UNIT_ASSERT_EQUALS(c, cc);
        }

        void testUnsignedChar()
        {
            cxxtools::SerializationInfo si;

            const unsigned char c = 'U';
            unsigned char cc = '\0';

            si <<= c;
            si >>= cc;
            CXXTOOLS_UNIT_ASSERT_EQUALS(c, cc);
        }

        void testChar()
        {
            cxxtools::SerializationInfo si;

            const char c = 'e';
            char cc = '\0';

            si <<= c;
            si >>= cc;
            CXXTOOLS_UNIT_ASSERT_EQUALS(c, cc);
        }

        void testShort()
        {
            cxxtools::SerializationInfo si;

            const short v = -42;
            short vv = 0;

            si <<= v;
            si >>= vv;
            CXXTOOLS_UNIT_ASSERT_EQUALS(vv, v);
        }

        void testUnsignedShort()
        {
            cxxtools::SerializationInfo si;

            const unsigned short v = 42;
            unsigned short vv = 0;

            si <<= v;
            si >>= vv;
            CXXTOOLS_UNIT_ASSERT_EQUALS(vv, v);
        }

        void testInt()
        {
            cxxtools::SerializationInfo si;

            const int v = 442;
            int vv = 0;

            si <<= v;
            si >>= vv;
            CXXTOOLS_UNIT_ASSERT_EQUALS(vv, v);
        }

        void testUnsignedInt()
        {
            cxxtools::SerializationInfo si;

            const unsigned int v = 42;
            unsigned int vv = 0;

            si <<= v;
            si >>= vv;
            CXXTOOLS_UNIT_ASSERT_EQUALS(vv, v);
        }

        void testLong()
        {
            cxxtools::SerializationInfo si;

            const long v = -1234567890l;
            long vv = 0;

            si <<= v;
            si >>= vv;
            CXXTOOLS_UNIT_ASSERT_EQUALS(vv, v);
        }

        void testUnsingedLong()
        {
            cxxtools::SerializationInfo si;

            const unsigned long v = 1234567890l;
            unsigned long vv = 0;

            si <<= v;
            si >>= vv;
            CXXTOOLS_UNIT_ASSERT_EQUALS(vv, v);
        }

        void testDouble()
        {
            cxxtools::SerializationInfo si;

            const double v = 1.252;
            double vv = 0;

            si <<= v;
            si >>= vv;

            // Equality between doubles is no problem here since doubles are
            // kept as doubles inside SerializationInfo
            CXXTOOLS_UNIT_ASSERT_EQUALS(vv, v);
        }

        void testStdString()
        {
            cxxtools::SerializationInfo si;

            const char v[] = "Hi there";
            std::string vv;

            si <<= v;
            si >>= vv;
            CXXTOOLS_UNIT_ASSERT_EQUALS(vv, v);
        }

        void testUnicodeString()
        {
            cxxtools::SerializationInfo si;

            const cxxtools::String v = L"Hi there";
            cxxtools::String vv;

            si <<= v;
            si >>= vv;
            CXXTOOLS_UNIT_ASSERT_EQUALS(vv, v);
        }

        void testStringToInt()
        {
            cxxtools::SerializationInfo si;

            const char v[] = " -42 ";
            int vv = 0;

            si <<= v;
            si >>= vv;
            CXXTOOLS_UNIT_ASSERT_EQUALS(vv, -42);
        }

        void testIntToString()
        {
            cxxtools::SerializationInfo si;

            const int v = -42;
            std::string vv;

            si <<= v;
            si >>= vv;
            CXXTOOLS_UNIT_ASSERT_EQUALS(vv, "-42");
        }

        void testVector()
        {
            cxxtools::SerializationInfo si;

            std::vector<int> v;
            v.push_back(42);
            v.push_back(23);
            v.push_back(-754);
            std::vector<int> vv;

            si <<= v;
            si >>= vv;
            CXXTOOLS_UNIT_ASSERT(v == vv);
        }

        void testBoolVector()
        {
            cxxtools::SerializationInfo si;

            std::vector<bool> v;
            v.push_back(true);
            v.push_back(false);
            v.push_back(true);
            std::vector<bool> vv;

            si <<= v;
            si >>= vv;
            CXXTOOLS_UNIT_ASSERT(v == vv);
        }

        void testList()
        {
            cxxtools::SerializationInfo si;

            std::list<int> v;
            v.push_back(42);
            v.push_back(23);
            v.push_back(-754);
            std::list<int> vv;

            si <<= v;
            si >>= vv;
            CXXTOOLS_UNIT_ASSERT(vv == v);
        }

        void testDeque()
        {
            cxxtools::SerializationInfo si;

            std::deque<int> v;
            v.push_back(42);
            v.push_back(23);
            v.push_back(-754);

            std::deque<int> vv;

            si <<= v;
            si >>= vv;
            CXXTOOLS_UNIT_ASSERT(vv == v);
        }

        void testSet()
        {
            cxxtools::SerializationInfo si;

            std::set<std::string> v;
            v.insert("Hello");
            v.insert("World");

            std::set<std::string> vv;

            si <<= v;
            si >>= vv;
            CXXTOOLS_UNIT_ASSERT(vv == v);
        }

        void testMultiset()
        {
            cxxtools::SerializationInfo si;

            std::multiset<std::string> v;
            v.insert("Hello");
            v.insert("Hello");
            v.insert("World");

            std::multiset<std::string> vv;

            si <<= v;
            si >>= vv;
            CXXTOOLS_UNIT_ASSERT_EQUALS(vv.size(), 3);
            CXXTOOLS_UNIT_ASSERT(vv == v);
        }

        void testMap()
        {
            cxxtools::SerializationInfo si;

            std::map<int, std::string> v;
            v[17] = "Hello";
            v[56] = "World";

            std::map<int, std::string> vv;

            si <<= v;
            si >>= vv;
            CXXTOOLS_UNIT_ASSERT(vv == v);
        }

        void testMultimap()
        {
            cxxtools::SerializationInfo si;

            typedef std::multimap<int, std::string> M;
            M v;
            v.insert(M::value_type(17, "Hello"));
            v.insert(M::value_type(56, "World"));
            v.insert(M::value_type(17, "blah"));

            std::multimap<int, std::string> vv;

            si <<= v;
            si >>= vv;
            CXXTOOLS_UNIT_ASSERT_EQUALS(vv.size(), 3);
            CXXTOOLS_UNIT_ASSERT(vv == v);
        }

        void testCArray()
        {
            cxxtools::SerializationInfo si;
            int v[3], vv[3];

            v[0] = 19;
            v[1] = -4;
            v[2] = 42;

            si <<= v;
            si >>= vv;

            CXXTOOLS_UNIT_ASSERT_EQUALS(vv[0], 19);
            CXXTOOLS_UNIT_ASSERT_EQUALS(vv[1], -4);
            CXXTOOLS_UNIT_ASSERT_EQUALS(vv[2], 42);
        }

#if __cplusplus >= 201103L

        void testForwardList()
        {
            cxxtools::SerializationInfo si;

            std::forward_list<std::string> v;
            v.push_front("Hello");
            v.push_front("World");

            std::forward_list<std::string> vv;

            si <<= v;
            si >>= vv;
            CXXTOOLS_UNIT_ASSERT(vv == v);
        }

        void testUnorderedSet()
        {
            cxxtools::SerializationInfo si;

            std::unordered_set<std::string> v;
            v.insert("Hello");
            v.insert("World");

            std::unordered_set<std::string> vv;

            si <<= v;
            si >>= vv;
            CXXTOOLS_UNIT_ASSERT(vv == v);
        }

        void testUnorderedMultiset()
        {
            cxxtools::SerializationInfo si;

            std::unordered_multiset<std::string> v;
            v.insert("Hello");
            v.insert("Hello");
            v.insert("World");

            std::unordered_multiset<std::string> vv;

            si <<= v;
            si >>= vv;
            CXXTOOLS_UNIT_ASSERT_EQUALS(vv.size(), 3);
            CXXTOOLS_UNIT_ASSERT(vv == v);
        }

        void testUnorderedMap()
        {
            cxxtools::SerializationInfo si;

            std::unordered_map<int, std::string> v;
            v[17] = "Hello";
            v[56] = "World";

            std::unordered_map<int, std::string> vv;

            si <<= v;
            si >>= vv;
            CXXTOOLS_UNIT_ASSERT(vv == v);
        }

        void testUnorderedMultimap()
        {
            cxxtools::SerializationInfo si;

            typedef std::unordered_multimap<int, std::string> M;
            M v;
            v.insert(M::value_type(17, "Hello"));
            v.insert(M::value_type(56, "World"));
            v.insert(M::value_type(17, "blah"));

            std::unordered_multimap<int, std::string> vv;

            si <<= v;
            si >>= vv;
            CXXTOOLS_UNIT_ASSERT_EQUALS(vv.size(), 3);
            CXXTOOLS_UNIT_ASSERT(vv == v);
        }

        void testTuple()
        {
            cxxtools::SerializationInfo si;
            std::tuple<int, std::string, double> v;

            si <<= std::make_tuple(42, "Hello", 3.5);
            si >>= v;

            CXXTOOLS_UNIT_ASSERT_EQUALS(std::get<0>(v), 42);
            CXXTOOLS_UNIT_ASSERT_EQUALS(std::get<1>(v), "Hello");
            CXXTOOLS_UNIT_ASSERT_EQUALS(std::get<2>(v), 3.5);
        }

        void testArray()
        {
            cxxtools::SerializationInfo si;
            std::array<int, 3> v, vv;

            v[0] = 19;
            v[1] = -4;
            v[2] = 42;

            si <<= v;
            si >>= vv;

            CXXTOOLS_UNIT_ASSERT_EQUALS(vv[0], 19);
            CXXTOOLS_UNIT_ASSERT_EQUALS(vv[1], -4);
            CXXTOOLS_UNIT_ASSERT_EQUALS(vv[2], 42);
        }

        void testEnum()
        {
            enum class Foo : char
            {
                FOO, BAR, BAZ
            };

            {
                Foo foo(Foo::FOO);
                Foo foo2(Foo::BAR);

                cxxtools::SerializationInfo si;
                si <<= cxxtools::EnumClass(foo);
                si >>= cxxtools::EnumClass(foo2);

                CXXTOOLS_UNIT_ASSERT(foo == foo2);
            }
            
            {
                const Foo foo(Foo::BAZ);
                Foo foo2(Foo::BAR);

                cxxtools::SerializationInfo si;
                si <<= cxxtools::EnumClass(foo);
                si >>= cxxtools::EnumClass(foo2);

                CXXTOOLS_UNIT_ASSERT(foo == foo2);
            }

        }
#endif

};

cxxtools::unit::RegisterTest<SerializationTest> register_SerializationTest;
