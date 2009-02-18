/***************************************************************************
 *   Copyright (C) 2005-2006 by Marc Boris Dürner                          *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this program; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "cxxtools/unit/test.h"
#include "cxxtools/unit/testcontext.h"
#include "cxxtools/unit/testfixture.h"

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
