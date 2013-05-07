/*
 * Copyright (C) 2005-2008 by Dr. Marc Boris Duerner
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
    Tests::iterator it;
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
    Tests::iterator it;
    for(it = _tests.begin(); it != _tests.end(); ++it)
    {
        TestMethod* test = it->second;
        Context ctx(*this, *test, si, 0);
        ctx.run();
    }
}


TestMethod* TestSuite::findTest(const std::string& name)
{
    Tests::iterator it;
    for (it = _tests.begin(); it != _tests.end() && it->first != name; ++it)
        ;

    if( it== _tests.end() )
        return 0;

    return it->second;
}


void TestSuite::registerTest(TestMethod* test)
{
    test->setParent(this);
    std::pair<const std::string, TestMethod*> p( test->name(), test );
    _tests.push_back( p );
}


} // namespace unit

} // namespace cxxtools
