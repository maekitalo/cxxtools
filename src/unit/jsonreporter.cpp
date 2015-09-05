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

#include <cxxtools/unit/jsonreporter.h>
#include <cxxtools/json.h>

namespace cxxtools
{

namespace unit
{

void JsonReporter::reportStart(const cxxtools::unit::TestContext& test)
{
    current = &si.addMember(test.testName());
}

void JsonReporter::reportFinish(const cxxtools::unit::TestContext& /*test*/)
{
}

void JsonReporter::reportMessage(const std::string& msg)
{
    current->addMember("message") <<= msg;
}

void JsonReporter::reportSuccess(const cxxtools::unit::TestContext& /*test*/)
{
    current->addMember("success") <<= true;
}

void JsonReporter::reportAssertion(const cxxtools::unit::TestContext& /*test*/, const cxxtools::unit::Assertion& a)
{
    current->addMember("success") <<= false;
    current->addMember("assertion") <<= true;
    current->addMember("exception") <<= false;
    current->addMember("file") <<= a.sourceInfo().file();
    current->addMember("line") <<= a.sourceInfo().line();
    current->addMember("message") <<= a.what();
}

void JsonReporter::reportException(const cxxtools::unit::TestContext& /*test*/, const std::exception& ex)
{
    current->addMember("success") <<= false;
    current->addMember("exception") <<= true;
    current->addMember("message") <<= ex.what();
}

void JsonReporter::reportError(const cxxtools::unit::TestContext& /*test*/)
{
    current->addMember("success") <<= false;
    current->addMember("error") <<= true;
}

void JsonReporter::reportSkip(const cxxtools::unit::TestContext& /*test*/)
{
    current->addMember("success") <<= false;
    current->addMember("skip") <<= true;
}

void operator <<= (cxxtools::SerializationInfo& si, const JsonReporter& result)
{
    si.setTypeName("JsonReporter");
    si = result.si;
}

}
}
