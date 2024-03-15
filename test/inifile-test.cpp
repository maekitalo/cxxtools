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
#include "cxxtools/inifile.h"
#include <sstream>

class IniFileTest : public cxxtools::unit::TestSuite
{

    public:
        IniFileTest()
        : cxxtools::unit::TestSuite("inifile")
        {
            registerMethod("testReadIniFile", *this, &IniFileTest::testReadIniFile);
            registerMethod("testWriteIniFile", *this, &IniFileTest::testWriteIniFile);
        }

        void testReadIniFile()
        {
            std::istringstream in(
                "[s1]\n"
                "k1=v1\n"
                "k2=v2\n"
                "[s2]\n"
                "k1=17\n"
                "k2=1.5"
                );
            cxxtools::IniFile inifile(in);

            CXXTOOLS_UNIT_ASSERT(inifile.exists(L"s1"));
            CXXTOOLS_UNIT_ASSERT(!inifile.exists(L"s3"));

            CXXTOOLS_UNIT_ASSERT(inifile.exists(L"s1", L"k1"));
            CXXTOOLS_UNIT_ASSERT(!inifile.exists(L"s1", L"k3"));

            CXXTOOLS_UNIT_ASSERT_EQUALS(inifile.getValue(L"s1", L"k1"), cxxtools::String(L"v1"));
            CXXTOOLS_UNIT_ASSERT_EQUALS(inifile.getValueT(L"s1", L"k1", 42), 42);
            CXXTOOLS_UNIT_ASSERT_EQUALS(inifile.getValueT(L"s2", L"k1", 42), 17);
            CXXTOOLS_UNIT_ASSERT_EQUALS(inifile.getValueT(L"s2", L"k2", 42.25), 1.5);
            CXXTOOLS_UNIT_ASSERT_EQUALS(inifile.getValueT(L"s2", L"k3", 42.25), 42.25);
        }

        void testWriteIniFile()
        {
            cxxtools::IniFile src;
            src.setValue(L"s1", L"k1", L"v1");
            src.setValueT(L"s2", L"k1", 17);

            std::stringstream st;
            st << src;

            cxxtools::IniFile inifile;
            st >> inifile;

            CXXTOOLS_UNIT_ASSERT_EQUALS(inifile.getValue(L"s1", L"k1"), cxxtools::String(L"v1"));
            CXXTOOLS_UNIT_ASSERT_EQUALS(inifile.getValueT(L"s2", L"k1", 42), 17);
        }

};

cxxtools::unit::RegisterTest<IniFileTest> register_IniFileTest;
