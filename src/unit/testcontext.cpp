/*
 * Copyright (C) 2005-2006 by Marc Boris Duerner
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
#include "cxxtools/unit/test.h"
#include "cxxtools/unit/testcontext.h"
#include "cxxtools/unit/testfixture.h"
#include "cxxtools/log.h"

log_define("cxxtools.unit.testcontext")

namespace cxxtools {

namespace unit {

TestContext::TestContext(TestFixture& fixture, Test& test )
: _fixture(fixture)
, _test(test)
, _setUp(false)
{ }


TestContext::~TestContext()
{
    try
    {
        if( _setUp )
            _fixture.tearDown();
    }
    catch(...)
    {}

    try
    {
        _test.reportFinish(*this);
    }
    catch(...)
    {}
}


std::string TestContext::testName() const
{
    return _test.name();
}


void TestContext::run()
{
    try
    {
        log_debug("run test " << _test.name());
        _test.reportStart(*this);
        _fixture.setUp();
        _setUp = true;
        this->exec();
        _test.reportSuccess(*this);
    }
    catch(const Assertion& assertion)
    {
        _test.reportAssertion(*this, assertion);
    }
    catch(const std::exception& ex)
    {
        _test.reportException(*this, ex);
    }
    catch(...)
    {
        _test.reportError(*this);
    }
}

}

}
