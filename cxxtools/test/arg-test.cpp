/*
 * Copyright (C) 2010 Tommi Maekitalo
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
#include <vector>
#include <string.h>
#include "cxxtools/arg.h"
#include "cxxtools/unit/testsuite.h"
#include "cxxtools/unit/registertest.h"

class ArgTest : public cxxtools::unit::TestSuite
{
        std::vector<char*> argp;

        char* arg(const char* value)
        {
            argp.push_back(new char [strlen(value) + 1]);
            strcpy(argp.back(), value);
            return argp.back();
        }

    public:
        ArgTest()
        : cxxtools::unit::TestSuite("cxxtools-arg-Test")
        {
            registerMethod("testArgBool", *this, &ArgTest::testArgBool);
            registerMethod("testArgCharP", *this, &ArgTest::testArgCharP);
            registerMethod("testArgpInt", *this, &ArgTest::testArgpInt);
            registerMethod("testArgpCharp", *this, &ArgTest::testArgpCharp);
            registerMethod("testArgpStdString", *this, &ArgTest::testArgpStdString);
        }

        void setUp()
        {
        }

        void tearDown()
        {
            for (std::vector<char*>::iterator it = argp.begin(); it != argp.end(); ++it)
                delete[] *it;
            argp.clear();
        }

        void testArgBool()
        {
            int argc = 7;
            char* argv[] = { arg("prog"), arg("-i"), arg("-j"), arg("-k"),
                arg("-cf"), arg("--foo"), arg("foo"), 0 };

            cxxtools::Arg<bool> optionJ(argc, argv, 'j');
            cxxtools::Arg<bool> optionL(argc, argv, 'l');
            cxxtools::Arg<bool> optionC(argc, argv, 'c');
            cxxtools::Arg<bool> optionFoo(argc, argv, "--foo");
            cxxtools::Arg<bool> optionBar(argc, argv, "--bar");

            CXXTOOLS_UNIT_ASSERT(optionJ);
            CXXTOOLS_UNIT_ASSERT(!optionL);
            CXXTOOLS_UNIT_ASSERT(optionC);
            CXXTOOLS_UNIT_ASSERT(optionFoo);
            CXXTOOLS_UNIT_ASSERT(!optionBar);
            CXXTOOLS_UNIT_ASSERT_EQUALS(argc, 5);
            CXXTOOLS_UNIT_ASSERT_EQUALS(strcmp(argv[0], "prog"), 0);
            CXXTOOLS_UNIT_ASSERT_EQUALS(strcmp(argv[1], "-i"), 0);
            CXXTOOLS_UNIT_ASSERT_EQUALS(strcmp(argv[2], "-k"), 0);
            CXXTOOLS_UNIT_ASSERT_EQUALS(strcmp(argv[3], "-f"), 0);
            CXXTOOLS_UNIT_ASSERT_EQUALS(strcmp(argv[4], "foo"), 0);
            CXXTOOLS_UNIT_ASSERT_EQUALS(argv[5], 0);
        }

        void testArgCharP()
        {
            int argc = 7;
            char* argv[] = { arg("prog"), arg("-i"), arg("foo"), arg("--foo"),
                arg("inp"), arg("-k"), arg("bar"), 0 };

            cxxtools::Arg<const char*> optionI(argc, argv, 'i');
            cxxtools::Arg<const char*> optionL(argc, argv, 'l', "blah");
            cxxtools::Arg<const char*> optionFoo(argc, argv, "--foo");
            cxxtools::Arg<const char*> optionBar(argc, argv, "--bar");

            CXXTOOLS_UNIT_ASSERT(optionI.isSet());
            CXXTOOLS_UNIT_ASSERT(!optionL.isSet());
            CXXTOOLS_UNIT_ASSERT(optionFoo.isSet());
            CXXTOOLS_UNIT_ASSERT(!optionBar.isSet());
            CXXTOOLS_UNIT_ASSERT_EQUALS(strcmp(optionI.getValue(), "foo"), 0);
            CXXTOOLS_UNIT_ASSERT_EQUALS(strcmp(optionL.getValue(), "blah"), 0);
            CXXTOOLS_UNIT_ASSERT_EQUALS(strcmp(optionFoo.getValue(), "inp"), 0);
            CXXTOOLS_UNIT_ASSERT_EQUALS(optionBar.getValue(), 0);
            CXXTOOLS_UNIT_ASSERT_EQUALS(argc, 3);
            CXXTOOLS_UNIT_ASSERT_EQUALS(strcmp(argv[0], "prog"), 0);
            CXXTOOLS_UNIT_ASSERT_EQUALS(strcmp(argv[1], "-k"), 0);
            CXXTOOLS_UNIT_ASSERT_EQUALS(strcmp(argv[2], "bar"), 0);
            CXXTOOLS_UNIT_ASSERT_EQUALS(argv[3], 0);
        }

        void testArgpInt()
        {
            int argc = 6;
            char* argv[] = { arg("prog"), arg("-v"), arg("42"), arg("-i"), arg("-j5"), arg("--foo"), 0 };

            cxxtools::Arg<int> optionJ(argc, argv, 'j');
            cxxtools::Arg<int> optionV(argc, argv, 'v');

            CXXTOOLS_UNIT_ASSERT(optionJ.isSet());
            CXXTOOLS_UNIT_ASSERT(optionV.isSet());
            CXXTOOLS_UNIT_ASSERT_EQUALS(optionJ, 5);
            CXXTOOLS_UNIT_ASSERT_EQUALS(optionV, 42);
            CXXTOOLS_UNIT_ASSERT_EQUALS(argc, 3);
            CXXTOOLS_UNIT_ASSERT_EQUALS(strcmp(argv[0], "prog"), 0);
            CXXTOOLS_UNIT_ASSERT_EQUALS(strcmp(argv[1], "-i"), 0);
            CXXTOOLS_UNIT_ASSERT_EQUALS(strcmp(argv[2], "--foo"), 0);
            CXXTOOLS_UNIT_ASSERT_EQUALS(argv[3], 0);
        }

        void testArgpCharp()
        {
            int argc = 7;
            char* argv[] = { arg("prog"), arg("-v"), arg("42"),
                arg("-I"), arg("include"), arg("-Jfoo"), arg("-Khello world"), 0 };

            cxxtools::Arg<const char*> optionI(argc, argv, 'I');
            cxxtools::Arg<const char*> optionJ(argc, argv, 'J');
            cxxtools::Arg<const char*> optionK(argc, argv, 'K');

            CXXTOOLS_UNIT_ASSERT(optionI.isSet());
            CXXTOOLS_UNIT_ASSERT(optionJ.isSet());
            CXXTOOLS_UNIT_ASSERT(optionK.isSet());
            CXXTOOLS_UNIT_ASSERT_EQUALS(strcmp(optionI, "include"), 0);
            CXXTOOLS_UNIT_ASSERT_EQUALS(strcmp(optionJ, "foo"), 0);
            CXXTOOLS_UNIT_ASSERT_EQUALS(strcmp(optionK, "hello world"), 0);
            CXXTOOLS_UNIT_ASSERT_EQUALS(argc, 3);
            CXXTOOLS_UNIT_ASSERT_EQUALS(strcmp(argv[0], "prog"), 0);
            CXXTOOLS_UNIT_ASSERT_EQUALS(strcmp(argv[1], "-v"), 0);
            CXXTOOLS_UNIT_ASSERT_EQUALS(strcmp(argv[2], "42"), 0);
            CXXTOOLS_UNIT_ASSERT_EQUALS(argv[3], 0);
        }

        void testArgpStdString()
        {
            int argc = 7;
            char* argv[] = { arg("prog"), arg("-v"), arg("42"), arg("-Jfoo"),
                arg("-I"), arg("include"), arg("-Khello world"), 0 };

            cxxtools::Arg<std::string> optionI(argc, argv, 'I');
            cxxtools::Arg<std::string> optionJ(argc, argv, 'J');
            cxxtools::Arg<std::string> optionK(argc, argv, 'K');

            CXXTOOLS_UNIT_ASSERT(optionI.isSet());
            CXXTOOLS_UNIT_ASSERT(optionJ.isSet());
            CXXTOOLS_UNIT_ASSERT(optionK.isSet());
            CXXTOOLS_UNIT_ASSERT_EQUALS(optionI.getValue(), "include");
            CXXTOOLS_UNIT_ASSERT_EQUALS(optionJ.getValue(), "foo");
            CXXTOOLS_UNIT_ASSERT_EQUALS(optionK.getValue(), "hello world");
            CXXTOOLS_UNIT_ASSERT_EQUALS(argc, 3);
            CXXTOOLS_UNIT_ASSERT_EQUALS(strcmp(argv[0], "prog"), 0);
            CXXTOOLS_UNIT_ASSERT_EQUALS(strcmp(argv[1], "-v"), 0);
            CXXTOOLS_UNIT_ASSERT_EQUALS(strcmp(argv[2], "42"), 0);
            CXXTOOLS_UNIT_ASSERT_EQUALS(argv[3], 0);
        }

};

cxxtools::unit::RegisterTest<ArgTest> register_ArgTest;
