/*
 * Copyright (C) 2009 Tommi Maekitalo
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * is provided AS IS, WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, and
 * NON-INFRINGEMENT.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 *
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
            cxxtools::Process process("./abort");
            process.start();
            CXXTOOLS_UNIT_ASSERT_THROW(process.wait(), cxxtools::ProcessFailed);
        }

};

cxxtools::unit::RegisterTest<ProcessTest> register_ProcessTest;
