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

#include "cxxtools/envsubst.h"
#include "cxxtools/unit/testsuite.h"
#include "cxxtools/unit/registertest.h"
#include <stdlib.h>
#include <assert.h>

class EnvSubstTest : public cxxtools::unit::TestSuite
{
    public:
        EnvSubstTest()
        : cxxtools::unit::TestSuite("envsubst")
        {
            registerMethod("testSimple", *this, &EnvSubstTest::testSimple);
            registerMethod("testSimpleEnvVar", *this, &EnvSubstTest::testSimpleEnvVar);
            registerMethod("testBracketedEnvVar", *this, &EnvSubstTest::testBracketedEnvVar);
            registerMethod("testUnsetEnvVar", *this, &EnvSubstTest::testUnsetEnvVar);
            registerMethod("testEsc", *this, &EnvSubstTest::testEsc);
            registerMethod("testPlainEsc", *this, &EnvSubstTest::testPlainEsc);
            registerMethod("testUseDefvalue", *this, &EnvSubstTest::testUseDefvalue);
            registerMethod("testSkipDefvalue", *this, &EnvSubstTest::testSkipDefvalue);
            registerMethod("testUseOtherVar", *this, &EnvSubstTest::testUseOtherVar);
            registerMethod("testUseOtherBVar", *this, &EnvSubstTest::testUseOtherBVar);
        }

        void testSimple()
        {
            std::string value = "abc def foobar";
            std::string exvalue = cxxtools::envSubst(value);
            CXXTOOLS_UNIT_ASSERT_EQUALS(value, exvalue);
        }

        void testSimpleEnvVar()
        {
            std::string value = "abc $USER def foobar";
            std::string exvalue = cxxtools::envSubst(value);
            const char* user = getenv("USER");
            assert(user != 0);
            std::string expected = "abc ";
            expected += user;
            expected += " def foobar";

            CXXTOOLS_UNIT_ASSERT_EQUALS(exvalue, expected);
        }

        void testBracketedEnvVar()
        {
            std::string value = "abc ${USER}def foobar";
            std::string exvalue = cxxtools::envSubst(value);
            const char* user = getenv("USER");
            assert(user != 0);
            std::string expected = "abc ";
            expected += user;
            expected += "def foobar";

            CXXTOOLS_UNIT_ASSERT_EQUALS(exvalue, expected);
        }

        void testUnsetEnvVar()
        {
            std::string value = "abc $UNSET_VAR def foobar";
            std::string exvalue = cxxtools::envSubst(value);
            const char* unsetVar = getenv("UNSET_VAR");
            assert(unsetVar == 0);

            CXXTOOLS_UNIT_ASSERT_EQUALS(exvalue, "abc  def foobar");
        }

        void testEsc()
        {
            std::string value = "abc \\$USER def foobar";
            std::string exvalue = cxxtools::envSubst(value);
            const char* user = getenv("USER");
            assert(user != 0);
            std::string expected = "abc $USER def foobar";

            CXXTOOLS_UNIT_ASSERT_EQUALS(exvalue, expected);
        }

        void testPlainEsc()
        {
            {
                std::string value = "\\";
                std::string exvalue = cxxtools::envSubst(value);
                std::string expected = "\\";

                CXXTOOLS_UNIT_ASSERT_EQUALS(exvalue, expected);
            }

            {
                std::string value = "\\ t";
                std::string exvalue = cxxtools::envSubst(value);
                std::string expected = "\\ t";

                CXXTOOLS_UNIT_ASSERT_EQUALS(exvalue, expected);
            }

        }

        void testUseDefvalue()
        {
            std::string value = "abc ${UNSET_VAR:-no user} def foobar";
            std::string exvalue = cxxtools::envSubst(value);
            const char* unsetVar = getenv("UNSET_VAR");
            assert(unsetVar == 0);
            std::string expected = "abc no user def foobar";

            CXXTOOLS_UNIT_ASSERT_EQUALS(exvalue, expected);
        }

        void testSkipDefvalue()
        {
            std::string value = "abc ${USER:-no user}def foobar";
            std::string exvalue = cxxtools::envSubst(value);
            const char* user = getenv("USER");
            assert(user != 0);
            std::string expected = "abc ";
            expected += user;
            expected += "def foobar";

            CXXTOOLS_UNIT_ASSERT_EQUALS(exvalue, expected);
        }

        void testUseOtherVar()
        {
            std::string value = "abc ${UNSET_VAR:-$USER} foobar";
            std::string exvalue = cxxtools::envSubst(value);

            const char* unsetVar = getenv("UNSET_VAR");
            assert(unsetVar == 0);
            const char* user = getenv("USER");
            assert(user != 0);

            std::string expected = "abc ";
            expected += user;
            expected += " foobar";

            CXXTOOLS_UNIT_ASSERT_EQUALS(exvalue, expected);
        }

        void testUseOtherBVar()
        {
            std::string value = "abc ${UNSET_VAR:-${USER}} foobar";
            std::string exvalue = cxxtools::envSubst(value);

            const char* unsetVar = getenv("UNSET_VAR");
            assert(unsetVar == 0);
            const char* user = getenv("USER");
            assert(user != 0);

            std::string expected = "abc ";
            expected += user;
            expected += " foobar";

            CXXTOOLS_UNIT_ASSERT_EQUALS(exvalue, expected);
        }

};

cxxtools::unit::RegisterTest<EnvSubstTest> register_EnvSubstTest;
