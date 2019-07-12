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
#include "cxxtools/iniparser.h"
#include <sstream>

namespace
{
    class E : public cxxtools::IniParser::Event
    {
        cxxtools::String _result;

    public:
        const cxxtools::String& result() const  { return _result; }
        void clear()
        { _result.clear(); }

        virtual bool onSection(const cxxtools::String& section);
        virtual bool onKey(const cxxtools::String& key);
        virtual bool onValue(const cxxtools::String& value);
        virtual bool onComment(const cxxtools::String& comment);
        virtual bool onError();
    };

    bool E::onSection(const cxxtools::String& section)
    {
        _result += L"S(";
        _result += section;
        _result += L')';
        return false;
    }

    bool E::onKey(const cxxtools::String& key)
    {
        _result += L"K(";
        _result += key;
        _result += L')';
        return false;
    }

    bool E::onValue(const cxxtools::String& value)
    {
        _result += L"V(";
        _result += value;
        _result += L')';
        return false;
    }

    bool E::onComment(const cxxtools::String& comment)
    {
        _result += L"C(";
        _result += comment;
        _result += L')';
        return false;
    }

    bool E::onError()
    {
        _result += L"E";
        return false;
    }

}

class IniParserTest : public cxxtools::unit::TestSuite
{

    public:
        IniParserTest()
        : cxxtools::unit::TestSuite("iniparser")
        {
            registerMethod("testIniParser", *this, &IniParserTest::testIniParser);
            registerMethod("testEmptyValue", *this, &IniParserTest::testEmptyValue);
            registerMethod("testSingleQuotedValue", *this, &IniParserTest::testSingleQuotedValue);
            registerMethod("testDoubleQuotedValue", *this, &IniParserTest::testDoubleQuotedValue);
            registerMethod("testEscapedValue", *this, &IniParserTest::testEscapedValue);
        }

        void testIniParser()
        {
            std::istringstream inifile(
                "[s1]\n"
                "k1=v1 \n"
                "k2= v2\n"
                "k3=v3");
            E e;
            cxxtools::IniParser p(e);
            p.parse(inifile);
            CXXTOOLS_UNIT_ASSERT_EQUALS(e.result(), "S(s1)K(k1)V(v1)K(k2)V(v2)K(k3)V(v3)");
        }

        void testEmptyValue()
        {
            std::istringstream inifile(
                "[s1]\n"
                "k1=\n"
                "[s2]\n"
                "k2=\n"
                );
            E e;
            cxxtools::IniParser p(e);
            p.parse(inifile);
            CXXTOOLS_UNIT_ASSERT_EQUALS(e.result(), "S(s1)K(k1)V()S(s2)K(k2)V()");
        }

        void testSingleQuotedValue()
        {
            std::istringstream inifile(
                "[s1]\n"
                "k1= ' v1 ' \n");
            E e;
            cxxtools::IniParser p(e);
            p.parse(inifile);
            CXXTOOLS_UNIT_ASSERT_EQUALS(e.result(), "S(s1)K(k1)V( v1 )");
        }

        void testDoubleQuotedValue()
        {
            std::istringstream inifile(
                "[s1]\n"
                "k1= \" v1 \"");
            E e;
            cxxtools::IniParser p(e);
            p.parse(inifile);
            CXXTOOLS_UNIT_ASSERT_EQUALS(e.result(), "S(s1)K(k1)V( v1 )");
        }

        void testEscapedValue()
        {
            std::istringstream inifile(
                "[s1]\n"
                "k1=\" \\r\\t\\n\\\"\\' \"\n"
                "k2=' \\r\\t\\n\\\"\\' '\n" 
                "k3= v\\t\\\"\\' \n");
            E e;
            cxxtools::IniParser p(e);
            p.parse(inifile);
            CXXTOOLS_UNIT_ASSERT_EQUALS(e.result(), "S(s1)K(k1)V( \r\t\n\"' )K(k2)V( \r\t\n\"' )K(k3)V(v\t\"')");
        }

};

cxxtools::unit::RegisterTest<IniParserTest> register_IniParserTest;
