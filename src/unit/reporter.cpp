/*
 * Copyright (C) 2005-2007 by Dr. Marc Boris Duerner
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
#include "cxxtools/unit/reporter.h"
#include <iostream>

namespace cxxtools {

namespace unit {

BriefReporter::BriefReporter(std::ostream* out)
: _out(out)
{
}


BriefReporter::~BriefReporter()
{
}


void BriefReporter::setOutput(std::ostream& out)
{
    _out = &out;
}


void BriefReporter::reportStart(const TestContext& test)
{
    *_out << test.testName() << ": ";
}


void BriefReporter::reportFinish(const TestContext&)
{
}


void BriefReporter::reportMessage(const std::string& msg)
{
    *_out << msg << std::endl;
}


void BriefReporter::reportSuccess(const TestContext&)
{
    *_out << "OK" << std::endl;
}


void BriefReporter::reportAssertion(const TestContext&, const Assertion& a)
{
    *_out << "ASSERTION at " << a.sourceInfo().file() << ":" << a.sourceInfo().line() << std::endl;
    *_out << '\t' << a.what() << std::endl;
}


void BriefReporter::reportException(const TestContext&, const std::exception& ex)
{
    *_out << "EXCEPTION" << std::endl;
    *_out << '\t' << ex.what() << std::endl;
}


void BriefReporter::reportError(const TestContext&)
{
    *_out << "ERROR" << std::endl;
}

void BriefReporter::reportSkip(const TestContext&)
{
    *_out << "SKIP" << std::endl;
}

}

}
