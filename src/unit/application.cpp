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
#include <cxxtools/unit/application.h>
#include <cxxtools/regex.h>

namespace cxxtools {

namespace unit {

class SuccessCounter : public Reporter
{
    public:
        SuccessCounter()
        : _success(0),
          _skipped(0),
          _errors(0)
        {}

        unsigned success() const
        { return _success; }

        unsigned skipped() const
        { return _skipped; }

        unsigned errors() const
        { return _errors; }

        virtual void reportStart(const TestContext& /*test*/)
        {
            _successCurrent = true;
            _skipCurrent = false;
        }

        virtual void reportFinish(const TestContext& /*test*/)
        {
            if (_skipCurrent)
                ++_skipped;
            else if (_successCurrent)
                ++_success;
            else
                ++_errors;
        }

        virtual void reportSkip(const TestContext& /*test*/)
        { _skipCurrent = true; }

        virtual void reportAssertion(const TestContext& /*test*/, const Assertion& /*a*/)
        { _successCurrent = false; }

        virtual void reportException(const TestContext& /*test*/, const std::exception& /*ex*/)
        { _successCurrent = false; }

        virtual void reportError(const TestContext& /*test*/)
        { _successCurrent = false; }

    private:
        bool _successCurrent;
        bool _skipCurrent;
        unsigned _success;
        unsigned _skipped;
        unsigned _errors;
};


Application* Application::_app = 0;


Application::Application()
: Test(""),
  _success(0),
  _skipped(0),
  _errors(0)
{
    _app = this;

    std::list<Test*>::iterator it;
    for(it = Application::tests().begin(); it != Application::tests().end(); ++it)
    {
        (*it)->setParent( this );
    }
}


Application::~Application()
{
    std::list<Test*>::iterator it;
    for(it = Application::tests().begin(); it != Application::tests().end(); ++it)
    {
        (*it)->setParent( 0 );
    }
}


Application& Application::instance()
{
    if( ! _app )
        throw std::logic_error("application not initialized");

    return *_app;
}


Test* Application::findTest(const std::string& testname)
{
    std::list<Test*>::iterator it;
    for(it = Application::tests().begin(); it != Application::tests().end(); ++it)
    {
        if( (*it)->name() == testname)
            return *it;
    }

    return 0;
}


void Application::attachReporter(Reporter& r)
{
    Test::attachReporter(r);
}


void Application::attachReporter(Reporter& r, const std::string& testname)
{
    Test* test = this->findTest(testname);
    if( ! test )
        return;

    test->attachReporter(r);
}


void Application::run(const Regex& testName)
{
    SuccessCounter ec;
    this->attachReporter(ec);

    std::list<Test*>::iterator it;
    for(it = Application::tests().begin(); it != Application::tests().end(); ++it)
    {
        if (testName.match((*it)->name()))
            (*it)->run();
    }

    this->detachReporter(ec);

    _success += ec.success();
    _skipped += ec.skipped();
    _errors += ec.errors();
}


void Application::run()
{
    SuccessCounter ec;
    this->attachReporter(ec);

    std::list<Test*>::iterator it;
    for(it = Application::tests().begin(); it != Application::tests().end(); ++it)
    {
        (*it)->run();
    }

    this->detachReporter(ec);

    _success = ec.success();
    _skipped = ec.skipped();
    _errors = ec.errors();
}


void Application::staticRegisterTest(Test& test)
{
    for (std::list<Test*>::iterator it = Application::tests().begin(); it != Application::tests().end(); ++it)
    {
        if ((*it)->name() > test.name())
        {
            Application::tests().insert(it, &test);
            return;
        }
    }

    Application::tests().push_back(&test);
}


void Application::registerTest(Test& test)
{
    test.setParent(this);
    staticRegisterTest(test);
}


void Application::deregisterTest(Test& test)
{
    Application::tests().remove(&test);
    test.setParent(0);
}


std::list<Test*>& Application::tests()
{
    static std::list<Test*> _allTests;
    return _allTests;
}

} // namespace unit

} // namespace cxxtools
