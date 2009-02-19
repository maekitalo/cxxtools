/***************************************************************************
 *   Copyright (C) 2005-2008 by Dr. Marc Boris Duerner                     *
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
#include "cxxtools/unit/testsuite.h"
#include "cxxtools/unit/testcontext.h"

namespace cxxtools {

namespace unit {

TestProtocol TestSuite::defaultProtocol;


TestSuite::TestSuite(const std::string& name, TestProtocol& protocol)
: Test(name)
, _protocol(&protocol)
{
}


TestSuite::~TestSuite()
{
    std::multimap<std::string, TestMethod*>::iterator it;
    for(it = _tests.begin(); it != _tests.end(); ++it)
    {
        delete it->second;
    }
}


void TestSuite::setParameter(const std::string&, const SerializationInfo&)
{
}


void TestSuite::setProtocol(TestProtocol* protocol)
{
    _protocol = protocol;
}


void TestSuite::setUp()
{
}


void TestSuite::tearDown()
{
}


void TestSuite::run()
{
    _protocol->run(*this);
}


void TestSuite::runTest( const std::string& name, const SerializationInfo* si, size_t argCount )
{
    TestMethod* test = this->findTest(name);
    if(!test)
        throw std::runtime_error("No such test");

    Context ctx(*this, *test, si, argCount);
    ctx.run();
}


void TestSuite::runAll()
{
    const SerializationInfo* si = 0;
    std::multimap<std::string, TestMethod*>::iterator it;
    for(it = _tests.begin(); it != _tests.end(); ++it)
    {
        TestMethod* test = it->second;
        Context ctx(*this, *test, si, 0);
        ctx.run();
    }
}


TestMethod* TestSuite::findTest(const std::string& name)
{
    std::multimap<std::string, TestMethod*>::iterator it = _tests.find(name);
    if( it== _tests.end() )
        return 0;

    return it->second;
}


void TestSuite::registerTest(TestMethod* test)
{
    test->setParent(this);
    std::pair<const std::string, TestMethod*> p( test->name(), test );
    _tests.insert( p );
}


} // namespace unit

} // namespace cxxtools
