/*
 * Copyright (C) 2004 Marc Boris Duerner
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
#undef CXXTOOLS_API_EXPORT

#include "cxxtools/unit/testsuite.h"
#include "cxxtools/unit/registertest.h"
#include "cxxtools/string.h"
#include <vector>
#include <sstream>

#if __cplusplus >= 201103L
#include <utility>
#endif


class StringTest : public cxxtools::unit::TestSuite
{
    public:
        StringTest()
        : cxxtools::unit::TestSuite("string")
        {
            cxxtools::unit::TestSuite::registerMethod( "testConstructor", *this, &StringTest::testConstructor );
            cxxtools::unit::TestSuite::registerMethod( "testCompare", *this, &StringTest::testCompare );
            cxxtools::unit::TestSuite::registerMethod( "testCompareShort", *this, &StringTest::testCompareShort );
            cxxtools::unit::TestSuite::registerMethod( "testAssign", *this, &StringTest::testAssign );
            cxxtools::unit::TestSuite::registerMethod( "testAppend", *this, &StringTest::testAppend );
            cxxtools::unit::TestSuite::registerMethod( "testInsert", *this, &StringTest::testInsert );
            cxxtools::unit::TestSuite::registerMethod( "testClear", *this, &StringTest::testClear );
            cxxtools::unit::TestSuite::registerMethod( "testErase", *this, &StringTest::testErase );
            cxxtools::unit::TestSuite::registerMethod( "testReplace", *this, &StringTest::testReplace );
            cxxtools::unit::TestSuite::registerMethod( "testFind", *this, &StringTest::testFind );
            cxxtools::unit::TestSuite::registerMethod( "testRFind", *this, &StringTest::testRFind );
            cxxtools::unit::TestSuite::registerMethod( "testFindFirstOf", *this, &StringTest::testFindFirstOf );
            cxxtools::unit::TestSuite::registerMethod( "testFindLastOf", *this, &StringTest::testFindLastOf );
            cxxtools::unit::TestSuite::registerMethod( "testFindFirstNotOf", *this, &StringTest::testFindFirstNotOf );
            cxxtools::unit::TestSuite::registerMethod( "testFindLastNotOf", *this, &StringTest::testFindLastNotOf );
            cxxtools::unit::TestSuite::registerMethod( "testCStr", *this, &StringTest::testCStr );
            cxxtools::unit::TestSuite::registerMethod( "testSubstr", *this, &StringTest::testSubstr );
            cxxtools::unit::TestSuite::registerMethod( "testSwap", *this, &StringTest::testSwap );
            cxxtools::unit::TestSuite::registerMethod( "testAt", *this, &StringTest::testAt );
            cxxtools::unit::TestSuite::registerMethod( "testPushBack", *this, &StringTest::testPushBack );
            cxxtools::unit::TestSuite::registerMethod( "testCopy", *this, &StringTest::testCopy );
            cxxtools::unit::TestSuite::registerMethod( "testReserve", *this, &StringTest::testReserve );
            cxxtools::unit::TestSuite::registerMethod( "testReserveEmpty", *this, &StringTest::testReserveEmpty );
            cxxtools::unit::TestSuite::registerMethod( "testLengthAndSize", *this, &StringTest::testLengthAndSize );
#if __cplusplus >= 201103L
            cxxtools::unit::TestSuite::registerMethod( "testMove", *this, &StringTest::testMove );
#endif
        }

    protected:
        void testConstructor();
        void testCompare();
        void testCompareShort();
        void testAssign();
        void testAppend();
        void testInsert();
        void testClear();
        void testErase();
        void testReplace();
        void testFind();
        void testRFind();
        void testFindFirstOf();
        void testFindLastOf();
        void testFindFirstNotOf();
        void testFindLastNotOf();
        void testCStr();
        void testSubstr();
        void testSwap();
        void testIndexOperator();
        void testAt();
        void testPushBack();
        void testCopy();
        void testReserve();
        void testReserveEmpty();
        void testLengthAndSize();
#if __cplusplus >= 201103L
        void testMove();
#endif
};

cxxtools::unit::RegisterTest<StringTest> register_StringTest;



void StringTest::testConstructor()
{
    cxxtools::String s1;
    CXXTOOLS_UNIT_ASSERT(s1 == cxxtools::String(L""));

    cxxtools::String s2(L"abcde");
    CXXTOOLS_UNIT_ASSERT(s2 == L"abcde");

    cxxtools::String s3(L"abcde", 3);
    CXXTOOLS_UNIT_ASSERT(s3 == L"abc");

    cxxtools::String s4(3, 'x');
    CXXTOOLS_UNIT_ASSERT(s4 == L"xxx");

    cxxtools::String s5(s2);
    CXXTOOLS_UNIT_ASSERT(s5 == L"abcde");

    cxxtools::String s6(s2, 1);
    CXXTOOLS_UNIT_ASSERT(s6 == L"bcde");

    cxxtools::String s7(s2, 1, 3);
    CXXTOOLS_UNIT_ASSERT(s7 == L"bcd");

    cxxtools::String s10;
    CXXTOOLS_UNIT_ASSERT(s10 == cxxtools::String(L""));

    const cxxtools::Char c11[] = { 'a', 'b', 'c', 'd', 'e', '\0' };
    cxxtools::String s11(c11);
    CXXTOOLS_UNIT_ASSERT(s11 == c11);

    const cxxtools::Char c12[] = { 'a', 'b', 'c', '\0' };
    cxxtools::String s12(L"abcde", 3);
    CXXTOOLS_UNIT_ASSERT(s12 == c12);

    const cxxtools::Char c13[] = { 'x', 'x', 'x', '\0' };
    cxxtools::String s13(3, 'x');
    CXXTOOLS_UNIT_ASSERT(s13 == c13);

    const cxxtools::Char c14[] = { 'a', 'b', 'c', 'd', 'e', '\0' };
    cxxtools::String s14(s11);
    CXXTOOLS_UNIT_ASSERT(s14 == c14);

    const cxxtools::Char c15[] = { 'b', 'c', 'd', 'e', '\0' };
    cxxtools::String s15(s11, 1);
    CXXTOOLS_UNIT_ASSERT(s15 == c15);

    const cxxtools::Char c16[] = { 'b', 'c', 'd', '\0' };
    cxxtools::String s16(s11, 1, 3);
    CXXTOOLS_UNIT_ASSERT(s16 == c16);

    cxxtools::String s20(s2.begin(), s2.end());
    CXXTOOLS_UNIT_ASSERT(s20 == L"abcde");
}

void StringTest::testCompare()
{
    const cxxtools::Char abc[] = { 'a', 'b', 'c', '\0' };

    const wchar_t* z = L"abcxyz";
    cxxtools::String s(L"abcd");
    cxxtools::String t(abc);

    CXXTOOLS_UNIT_ASSERT_EQUALS(s.compare(s)                 , 0);
    CXXTOOLS_UNIT_ASSERT_EQUALS(s.compare(t)                 , 1);
    CXXTOOLS_UNIT_ASSERT_EQUALS(s.compare(z)                 , -1);
    CXXTOOLS_UNIT_ASSERT_EQUALS(s.compare(1, 3, t)           , 1);
    CXXTOOLS_UNIT_ASSERT_EQUALS(s.compare(1, 3, t, 1, 2)     , 1);
    CXXTOOLS_UNIT_ASSERT_EQUALS(s.compare(1, 3, z)           , 1);
    CXXTOOLS_UNIT_ASSERT_EQUALS(s.compare(1, 2, z + 1, 0, 2) , 0);

    cxxtools::String x1(L"abc");
    cxxtools::String x2(abc);
    CXXTOOLS_UNIT_ASSERT(x1 == x2);
    CXXTOOLS_UNIT_ASSERT(x1 == abc);
    CXXTOOLS_UNIT_ASSERT(x2 == abc);

    const cxxtools::Char empty[] = { '\0' };
    cxxtools::String y1(L"");
    cxxtools::String y2(empty);
    CXXTOOLS_UNIT_ASSERT(y1 == y2);
    CXXTOOLS_UNIT_ASSERT(y1 == empty);
    CXXTOOLS_UNIT_ASSERT(y2 == empty);

    CXXTOOLS_UNIT_ASSERT_EQUALS(s.compare(L"abcd"), 0);
    CXXTOOLS_UNIT_ASSERT_EQUALS(s.compare(L"abc"), 1);
    CXXTOOLS_UNIT_ASSERT_EQUALS(s.compare(L"abcde"), -1);

    cxxtools::String zz(L"abcd\0ef", 7);
    CXXTOOLS_UNIT_ASSERT_EQUALS(zz.compare(L"abcd"), 1);
    CXXTOOLS_UNIT_ASSERT_EQUALS(zz.compare(L"abcde"), -1);
    CXXTOOLS_UNIT_ASSERT_EQUALS(zz.compare(L"abcd\0ef", 7), 0);
    CXXTOOLS_UNIT_ASSERT_EQUALS(zz.compare(L"abcd\0ee", 7), 1);
    CXXTOOLS_UNIT_ASSERT_EQUALS(zz.compare(L"abcd\0eg", 7), -1);
    CXXTOOLS_UNIT_ASSERT_EQUALS(zz.compare(L"abcd\0eff", 9), -1);
    CXXTOOLS_UNIT_ASSERT_EQUALS(zz.compare(L"abcd\0eff", 5), 0);
}

void StringTest::testCompareShort()
{
    cxxtools::String s(L"abcd");

    CXXTOOLS_UNIT_ASSERT_EQUALS(s.compare("abcd")            , 0);
    CXXTOOLS_UNIT_ASSERT_EQUALS(s.compare("abc")             , 1);
    CXXTOOLS_UNIT_ASSERT_EQUALS(s.compare("abcxyz")          , -1);
    CXXTOOLS_UNIT_ASSERT_EQUALS(s.compare("abd")             , -1);
    CXXTOOLS_UNIT_ASSERT_EQUALS(s.compare("abb")             , 1);
    CXXTOOLS_UNIT_ASSERT_EQUALS(s.compare("ab")              , 1);

    CXXTOOLS_UNIT_ASSERT_EQUALS(s.compare("abcd", 4)         , 0);
    CXXTOOLS_UNIT_ASSERT_EQUALS(s.compare("abcxyz", 3)       , 1);
    CXXTOOLS_UNIT_ASSERT_EQUALS(s.compare("abcxyz", 4)       , -1);
    CXXTOOLS_UNIT_ASSERT_EQUALS(s.compare("abd", 3)          , -1);
    CXXTOOLS_UNIT_ASSERT_EQUALS(s.compare("abb", 3)          , 1);
    CXXTOOLS_UNIT_ASSERT_EQUALS(s.compare("ab", 2)           , 1);

    cxxtools::String x1(L"abc");
    CXXTOOLS_UNIT_ASSERT(x1 == "abc");

    cxxtools::String y1(L"");
    cxxtools::String y2("");
    CXXTOOLS_UNIT_ASSERT(y1 == y2);
    CXXTOOLS_UNIT_ASSERT(y1 == "");
    CXXTOOLS_UNIT_ASSERT(y2 == "");
}

void StringTest::testAssign()
{

{
    cxxtools::String s1;
    cxxtools::String s2(L"abc");
    cxxtools::String s3 = s1;

    cxxtools::Char& ref = s2[0]; // make it unsharable
    ref = L'x';
    s1 = s2;
}

    const wchar_t* z = L"abcde";
    std::vector<wchar_t> v(z, z + 5);
    cxxtools::String s;
    cxxtools::String t(z);

    s.assign(z);
    CXXTOOLS_UNIT_ASSERT(s == L"abcde");

    s.assign(z + 1, 0, 3);
    CXXTOOLS_UNIT_ASSERT(s == L"bcd");

    s.assign(3, 'x');
    CXXTOOLS_UNIT_ASSERT(s == L"xxx");

    s.assign(t);
    CXXTOOLS_UNIT_ASSERT(s == L"abcde");

    s.assign(t, 1, 3);
    CXXTOOLS_UNIT_ASSERT(s == L"bcd");

    s.assign(v.begin(), v.end());
    CXXTOOLS_UNIT_ASSERT(s == L"abcde");

    s = s;
    CXXTOOLS_UNIT_ASSERT(s == L"abcde");
    s.assign(t);
    s = s.c_str();
    CXXTOOLS_UNIT_ASSERT(s == L"abcde");
}

void StringTest::testAppend()
{
    const wchar_t* z = L"abcde";
    std::vector<wchar_t> v(z, z + 5);
    cxxtools::String s(L"ABC");
    cxxtools::String t(z);

    s.append(z);
    CXXTOOLS_UNIT_ASSERT(s == L"ABCabcde");

    s = L"ABC";
    s.append(z + 1, 0, 3);
    CXXTOOLS_UNIT_ASSERT(s == L"ABCbcd");

    s = L"ABC";
    s.append(3, 'x');
    CXXTOOLS_UNIT_ASSERT(s == L"ABCxxx");

    s = L"ABC";
    s.append(t);
    CXXTOOLS_UNIT_ASSERT(s == L"ABCabcde");

    s = L"ABC";
    s.append(t, 1, 3);
    CXXTOOLS_UNIT_ASSERT(s == L"ABCbcd");

    s = L"ABC";
    s.append(v.begin(), v.end());
    CXXTOOLS_UNIT_ASSERT(s == L"ABCabcde");

    // operator +=
    s = L"ABC";
    s += z;
    CXXTOOLS_UNIT_ASSERT(s == L"ABCabcde");

    s = L"ABC";
    s += 'x';
    CXXTOOLS_UNIT_ASSERT(s == L"ABCx");

    s = L"ABC";
    s += t;
    CXXTOOLS_UNIT_ASSERT(s == L"ABCabcde");

    s = L"ABC";
    cxxtools::String u = s + t;
    CXXTOOLS_UNIT_ASSERT(u == L"ABCabcde");
}

void StringTest::testInsert()
{
    cxxtools::String::iterator i;
    const wchar_t* z = L"abcde";
    std::vector<wchar_t> v(z, z + 5);
    cxxtools::String s(L"ABC");
    cxxtools::String t(z);

    s.insert(2, z);
    CXXTOOLS_UNIT_ASSERT(s == L"ABabcdeC");

    s = L"ABC";
    s.insert(2, z + 1, 0, 3);
    CXXTOOLS_UNIT_ASSERT(s == L"ABbcdC");

    s = L"ABC";
    s.insert(2, 3, 'x');
    CXXTOOLS_UNIT_ASSERT(s == L"ABxxxC");

    s = L"ABC";
    s.insert(2, t);
    CXXTOOLS_UNIT_ASSERT(s == L"ABabcdeC");

    s = L"ABC";
    s.insert(2, t, 1, 3);
    CXXTOOLS_UNIT_ASSERT(s == L"ABbcdC");

    s = L"ABC";
    i = s.begin() + 2;
    s.insert(i, 'x');
    CXXTOOLS_UNIT_ASSERT(s == L"ABxC");

    s = L"ABC";
    i = s.begin() + 2;
    s.insert(i, 3, 'x');
    CXXTOOLS_UNIT_ASSERT(s == L"ABxxxC");

/*  TODO API not implemented yet.
    s = L"ABC";
    s.insert(i, v.begin(), v.end());
    i = s.begin() + 2;
    CXXTOOLS_UNIT_ASSERT(s == L"ABabcdeC");*/
}

void StringTest::testClear()
{
    cxxtools::String s(L"abcdefg");

    s.clear();
    CXXTOOLS_UNIT_ASSERT(s == L"");
}

void StringTest::testErase()
{
    cxxtools::String s(L"abcdefg");
    cxxtools::String::iterator p = s.begin() + 2;
    cxxtools::String::iterator q = s.end() - 2;

    s.erase();
    CXXTOOLS_UNIT_ASSERT(s == L"");

    s = L"abcdefg";
    s.erase(2);
    CXXTOOLS_UNIT_ASSERT(s == L"ab");

    s = L"abcdefg";
    s.erase(2, 3);
    CXXTOOLS_UNIT_ASSERT(s == L"abfg");

    s = L"abcdefg";
    p = s.begin() + 2;
    s.erase(p);
    CXXTOOLS_UNIT_ASSERT(s == L"abdefg");

    s = L"abcdefg";
    p = s.begin() + 2;
    q = s.end()   - 2;
    s.erase(p, q);
    CXXTOOLS_UNIT_ASSERT(s == L"abfg");
}

void StringTest::testReplace()
{
    const wchar_t* z = L"vwxyz";
    std::vector<wchar_t> v(z, z + 5);
    cxxtools::String s(L"ABCDEF");
    cxxtools::String t(z);
    cxxtools::String::iterator i1;
    cxxtools::String::iterator i2;

    s.replace(1, 4, z);
    CXXTOOLS_UNIT_ASSERT(s == L"AvwxyzF");

    s = L"ABCDEF";
    s.replace(1, 4, z + 1, 0, 3);
    CXXTOOLS_UNIT_ASSERT(s == L"AwxyF");

    s = L"ABCDEF";
    s.replace(1, 4, 3, 'x');
    CXXTOOLS_UNIT_ASSERT(s == L"AxxxF");

    s = L"ABCDEF";
    s.replace(1, 4, t);
    CXXTOOLS_UNIT_ASSERT(s == L"AvwxyzF");

    s = L"ABCDEF";
    s.replace(1, 4, t, 1, 3);
    CXXTOOLS_UNIT_ASSERT(s == L"AwxyF");

    s = L"ABCDEF";
    i1 = s.begin() + 1;
    i2 = s.end() - 1;
    s.replace(i1, i2, z);
    CXXTOOLS_UNIT_ASSERT(s == L"AvwxyzF");

    cxxtools::Char z2[] = { 'v', 'w', 'x', 'y'  };
    i1 = s.begin() + 1;
    i2 = s.end() - 1;
    s.replace(i1, i2, z2 + 1, 3);
    CXXTOOLS_UNIT_ASSERT(s == L"AwxyF");

    s = L"ABCDEF";
    i1 = s.begin() + 1;
    i2 = s.end() - 1;
    s.replace(i1, i2, 3, 'x');
    CXXTOOLS_UNIT_ASSERT(s == L"AxxxF");

    s = L"ABCDEF";
    i1 = s.begin() + 1;
    i2 = s.end() - 1;
    s.replace(i1, i2, t);
    CXXTOOLS_UNIT_ASSERT(s == L"AvwxyzF");

/*  TODO API not implemented yet.
    s = L"ABCDEF";
    i1 = s.begin() + 1;
    i2 = s.end() - 1;
    s.replace(i1, i2, v.begin(), v.end());
    CXXTOOLS_UNIT_ASSERT(s == L"AvwxyzF");
*/
}

void StringTest::testFind()
{
    cxxtools::String s(L"abc-abc");
    cxxtools::String t(L"bc");
    cxxtools::Char   abcd[] = { 'a', 'b', 'c', 'd', '\0' };

    CXXTOOLS_UNIT_ASSERT_EQUALS(s.find(t)          , 1);
    CXXTOOLS_UNIT_ASSERT_EQUALS(s.find(t, 2)       , 5);
    CXXTOOLS_UNIT_ASSERT_EQUALS(s.find(L"bc")      , 1);
    CXXTOOLS_UNIT_ASSERT_EQUALS(s.find(L"bc", 2)   , 5);
    CXXTOOLS_UNIT_ASSERT_EQUALS(s.find(abcd, 2, 3) , 4);
    CXXTOOLS_UNIT_ASSERT_EQUALS(s.find('b')        , 1);
    CXXTOOLS_UNIT_ASSERT_EQUALS(s.find('b', 2)     , 5);
}

void StringTest::testRFind()
{
    cxxtools::String s(L"abc-abc");
    cxxtools::String t(L"bc");
    cxxtools::Char   abcd[] = { 'a', 'b', 'c', 'd', '\0' };

    CXXTOOLS_UNIT_ASSERT_EQUALS(s.rfind(t)          , 5);
    CXXTOOLS_UNIT_ASSERT_EQUALS(s.rfind(t, 2)       , 1);
    CXXTOOLS_UNIT_ASSERT_EQUALS(s.rfind(L"bc")      , 5);
    CXXTOOLS_UNIT_ASSERT_EQUALS(s.rfind(L"bc", 2)   , 1);
    CXXTOOLS_UNIT_ASSERT_EQUALS(s.rfind(abcd, 2, 3) , 0);
    CXXTOOLS_UNIT_ASSERT_EQUALS(s.rfind('b')        , 5);
    CXXTOOLS_UNIT_ASSERT_EQUALS(s.rfind('b', 2)     , 1);
}

void StringTest::testFindFirstOf()
{
    cxxtools::String s(L"abc-abc");
    cxxtools::String t(L"a-x");
    cxxtools::Char   abcd[] = { 'a', 'b', 'c', 'd', '\0' };

    CXXTOOLS_UNIT_ASSERT_EQUALS(s.find_first_of(t)          , 0);
    CXXTOOLS_UNIT_ASSERT_EQUALS(s.find_first_of(t, 2)       , 3);
    CXXTOOLS_UNIT_ASSERT_EQUALS(s.find_first_of(L"bc")      , 1);
    CXXTOOLS_UNIT_ASSERT_EQUALS(s.find_first_of(L"bc", 2)   , 2);
    CXXTOOLS_UNIT_ASSERT_EQUALS(s.find_first_of(abcd, 2, 3) , 2);
    CXXTOOLS_UNIT_ASSERT_EQUALS(s.find_first_of('b')        , 1);
    CXXTOOLS_UNIT_ASSERT_EQUALS(s.find_first_of('b', 2)     , 5);
}

void StringTest::testFindLastOf()
{
    cxxtools::String s(L"abc-abc");
    cxxtools::String t(L"a-x");
    cxxtools::Char   abcd[] = { 'a', 'b', 'c', 'd', '\0' };

    CXXTOOLS_UNIT_ASSERT_EQUALS(s.find_last_of(t)          , 4);
    CXXTOOLS_UNIT_ASSERT_EQUALS(s.find_last_of(t, 2)       , 0);
    CXXTOOLS_UNIT_ASSERT_EQUALS(s.find_last_of(L"bc")      , 6);
    CXXTOOLS_UNIT_ASSERT_EQUALS(s.find_last_of(L"bc", 2)   , 2);
    CXXTOOLS_UNIT_ASSERT_EQUALS(s.find_last_of(abcd, 2, 3) , 2);
    CXXTOOLS_UNIT_ASSERT_EQUALS(s.find_last_of('b')        , 5);
    CXXTOOLS_UNIT_ASSERT_EQUALS(s.find_last_of('b', 2)     , 1);
}

void StringTest::testFindFirstNotOf()
{
    cxxtools::String s(L"abc-abc");
    cxxtools::String t(L"a-x");
    cxxtools::Char   abcd[] = { 'a', 'b', 'c', 'd', '\0' };

    CXXTOOLS_UNIT_ASSERT_EQUALS(s.find_first_not_of(t)          , 1);
    CXXTOOLS_UNIT_ASSERT_EQUALS(s.find_first_not_of(t, 2)       , 2);
    CXXTOOLS_UNIT_ASSERT_EQUALS(s.find_first_not_of(L"bc")      , 0);
    CXXTOOLS_UNIT_ASSERT_EQUALS(s.find_first_not_of(L"bc", 2)   , 3);
    CXXTOOLS_UNIT_ASSERT_EQUALS(s.find_first_not_of(abcd, 2, 3) , 3);
    CXXTOOLS_UNIT_ASSERT_EQUALS(s.find_first_not_of('b')        , 0);
    CXXTOOLS_UNIT_ASSERT_EQUALS(s.find_first_not_of('b', 2)     , 2);
}

void StringTest::testFindLastNotOf()
{
    cxxtools::String s(L"abc-abc");
    cxxtools::String t(L"a-x");
    cxxtools::Char   abcd[] = { 'a', 'b', 'c', 'd', '\0' };

    CXXTOOLS_UNIT_ASSERT_EQUALS(s.find_last_not_of(t)          , 6);
    CXXTOOLS_UNIT_ASSERT_EQUALS(s.find_last_not_of(t, 2)       , 2);
    CXXTOOLS_UNIT_ASSERT_EQUALS(s.find_last_not_of(L"bc")      , 4);
    CXXTOOLS_UNIT_ASSERT_EQUALS(s.find_last_not_of(L"bc", 2)   , 0);
    CXXTOOLS_UNIT_ASSERT_EQUALS(s.find_last_not_of(abcd, 2, 3) , cxxtools::String::npos);
    CXXTOOLS_UNIT_ASSERT_EQUALS(s.find_last_not_of('b')        , 6);
    CXXTOOLS_UNIT_ASSERT_EQUALS(s.find_last_not_of('b', 2)     , 2);
}

void StringTest::testCStr()
{
    cxxtools::String s1(L"abc");
    CXXTOOLS_UNIT_ASSERT(s1.c_str()[0] == 'a' && s1.c_str()[1] == 'b' && s1.c_str()[2] == 'c' && s1.c_str()[3] == '\0');

    cxxtools::String s2;
    CXXTOOLS_UNIT_ASSERT(s2.c_str()[0] == '\0');

    cxxtools::Char abc[] = { 'a', 'b', 'c', '\0' };
    cxxtools::String s3(abc);
    CXXTOOLS_UNIT_ASSERT(s3.c_str()[0] == 'a' && s3.c_str()[1] == 'b' && s3.c_str()[2] == 'c' && s3.c_str()[3] == '\0');

    cxxtools::Char zero[] = { '\0' };
    cxxtools::String s4(zero);
    CXXTOOLS_UNIT_ASSERT(s4.c_str()[0] == '\0');
}

void StringTest::testSubstr()
{
    cxxtools::String s(L"abcdefg");

    CXXTOOLS_UNIT_ASSERT(s.substr()     == L"abcdefg");
    CXXTOOLS_UNIT_ASSERT(s.substr(2)    == L"cdefg");
    CXXTOOLS_UNIT_ASSERT(s.substr(2, 3) == L"cde");
}

void StringTest::testSwap()
{
    cxxtools::String s1(L"abc");
    cxxtools::String s2(L"xyz");

    CXXTOOLS_UNIT_ASSERT(s1 == L"abc");
    CXXTOOLS_UNIT_ASSERT(s2 == L"xyz");

    s1.swap(s2);

    CXXTOOLS_UNIT_ASSERT(s1 == L"xyz");
    CXXTOOLS_UNIT_ASSERT(s2 == L"abc");

    s2.swap(s1);

    CXXTOOLS_UNIT_ASSERT(s1 == L"abc");
    CXXTOOLS_UNIT_ASSERT(s2 == L"xyz");
}

void StringTest::testIndexOperator()
{
    cxxtools::String s(L"abcdef");

    CXXTOOLS_UNIT_ASSERT(s[0] == 'a');
    CXXTOOLS_UNIT_ASSERT(s[5] == 'f');
    CXXTOOLS_UNIT_ASSERT(s[6] == '\0');

/*    bool exceptionOccured = false;
    try {
        s[10];
    } catch (const cxxtools::Exception& e) {
        exceptionOccured = true;
    }*/
}

void StringTest::testAt()
{
    cxxtools::String s(L"abcdef");

    CXXTOOLS_UNIT_ASSERT(s.at(0) == 'a');
    CXXTOOLS_UNIT_ASSERT(s.at(5) == 'f');
}

void StringTest::testPushBack()
{
    cxxtools::String s(L"abc");

    s.push_back('d');
    CXXTOOLS_UNIT_ASSERT(s == L"abcd");
}

void StringTest::testCopy()
{
    cxxtools::Char t1[3];
    cxxtools::String s(L"abcd");

    s.copy(t1, 2);
    t1[2] = '\0';

    const cxxtools::Char c1[] = { 'a', 'b', '\0' };
    CXXTOOLS_UNIT_ASSERT(std::char_traits<cxxtools::Char>::compare(t1, c1, 3) == 0);


    cxxtools::Char t2[5];
    s.copy(t2, 4);
    t2[4] = '\0';

    const cxxtools::Char c2[] = { 'a', 'b', 'c', 'd', '\0' };
    CXXTOOLS_UNIT_ASSERT_EQUALS(std::char_traits<cxxtools::Char>::compare(t2, c2, 5) , 0);


    cxxtools::Char t3[3];
    s.copy(t3, 2, 2);
    t3[2] = '\0';

    const cxxtools::Char c3[] = { 'c', 'd', '\0' };
    CXXTOOLS_UNIT_ASSERT_EQUALS(std::char_traits<cxxtools::Char>::compare(t3, c3, 3) , 0);
}


void StringTest::testReserve()
{
    const cxxtools::Char c1[] = { 'a', 'b', 'c', 'd', '\0' };
    cxxtools::String s(L"abcd");
    cxxtools::String s2 = s;
    s2.reserve(10);

    CXXTOOLS_UNIT_ASSERT( s2.capacity() >= 10 );
    CXXTOOLS_UNIT_ASSERT( s2.size() == 4 );
    CXXTOOLS_UNIT_ASSERT_EQUALS( std::char_traits<cxxtools::Char>::compare(s2.c_str(), c1, 4) , 0 );

    CXXTOOLS_UNIT_ASSERT( s.capacity() >= 4 );
    CXXTOOLS_UNIT_ASSERT_EQUALS( s.size(), 4 );
    CXXTOOLS_UNIT_ASSERT_EQUALS( std::char_traits<cxxtools::Char>::compare(s.c_str(), c1, 4) , 0 );
}


void StringTest::testReserveEmpty()
{
    cxxtools::String s;
    s.reserve(0);
    CXXTOOLS_UNIT_ASSERT_EQUALS( s.size() , 0 );
}


void StringTest::testLengthAndSize()
{
    cxxtools::String s1;
    CXXTOOLS_UNIT_ASSERT_EQUALS(s1.length() , 0);
    CXXTOOLS_UNIT_ASSERT_EQUALS(s1.size()   , 0);

    cxxtools::String s2(L"ab");
    CXXTOOLS_UNIT_ASSERT_EQUALS(s2.length() , 2);
    CXXTOOLS_UNIT_ASSERT_EQUALS(s2.size()   , 2);

    s2 += L"cd";
    CXXTOOLS_UNIT_ASSERT_EQUALS(s2.length() , 4);
    CXXTOOLS_UNIT_ASSERT_EQUALS(s2.size()   , 4);


    cxxtools::Char ab[] = { 'a', 'b', '\0' };
    cxxtools::String s3(ab);
    CXXTOOLS_UNIT_ASSERT_EQUALS(s3.length() , 2);
    CXXTOOLS_UNIT_ASSERT_EQUALS(s3.size()   , 2);

    cxxtools::Char cd[] = { 'c', 'd', '\0' };
    s3 += cd;
    CXXTOOLS_UNIT_ASSERT_EQUALS(s3.length() , 4);
    CXXTOOLS_UNIT_ASSERT_EQUALS(s3.size()   , 4);
}

#if __cplusplus >= 201103L
void StringTest::testMove()
{
    {
        // test move constructor
        wchar_t str[] = L"hi";
        cxxtools::String s1(str);
        cxxtools::String s2(std::move(s1));
        CXXTOOLS_UNIT_ASSERT(s2 == str);
        CXXTOOLS_UNIT_ASSERT(s1 == str);  // this is true since s1 is a short string and moving is just copying the data without affecting the source
    }

    {
        // test move assignment
        wchar_t str[] = L"This is a quite long string constant, which do not fit into a short string";

        cxxtools::String s1(str);
        CXXTOOLS_UNIT_ASSERT(s1 == str);

        cxxtools::String s2;
        s2 = std::move(s1);
        CXXTOOLS_UNIT_ASSERT(s2 == str);
        CXXTOOLS_UNIT_ASSERT(s1 != str);  // this is true since s1 is a long string and moving is moving the data from s1 to s2
    }
}

#endif
