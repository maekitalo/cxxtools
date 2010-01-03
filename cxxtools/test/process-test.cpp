/*
 * Copyright (C) 2009 Tommi Maekitalo
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
#include "cxxtools/process.h"
#include "cxxtools/iostream.h"
#include "cxxtools/eventloop.h"
#include "cxxtools/unit/testsuite.h"
#include "cxxtools/unit/registertest.h"
#include "cxxtools/log.h"

log_define("cxxtools.test.process")


class ProcessTest : public cxxtools::unit::TestSuite
{
    public:
        ProcessTest()
        : cxxtools::unit::TestSuite("cxxtools-process-Test")
        {
            registerMethod("grepFilter", *this, &ProcessTest::grepFilter);
            registerMethod("processFail", *this, &ProcessTest::processFail);
        }

        void grepFilter()
        {
            // warning: we assume here, that the pipe has a buffer, which is big enough

            cxxtools::ProcessInfo pi("grep");
            pi.addArg("b");

            pi.setStdInput(cxxtools::ProcessInfo::Capture);
            pi.setStdOutput(cxxtools::ProcessInfo::Capture);

            cxxtools::Process process(pi);
            process.start();

            cxxtools::OStream grepIn;
            grepIn.attachDevice(*process.stdInput());

            cxxtools::IStream grepOut;
            grepOut.attachDevice(*process.stdOutput());

            grepIn << "foo\nbar\nbaz\n" << std::flush;
            process.stdInput()->close();

            std::string line;

            std::getline(grepOut, line);
            CXXTOOLS_UNIT_ASSERT_EQUALS(line, "bar");

            std::getline(grepOut, line);
            CXXTOOLS_UNIT_ASSERT_EQUALS(line, "baz");

            std::getline(grepOut, line);
            CXXTOOLS_UNIT_ASSERT(grepOut.eof());

            process.wait();

        }

        void processFail()
        {
            cxxtools::Process process(TESTDIR "/abort");
            process.start();
            CXXTOOLS_UNIT_ASSERT_THROW(process.wait(), cxxtools::ProcessFailed);
        }

};

cxxtools::unit::RegisterTest<ProcessTest> register_ProcessTest;
