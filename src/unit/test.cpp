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
#include <cxxtools/unit/test.h>

namespace cxxtools {

namespace unit {

const std::string& Test::name() const
{
    return _name;
}

void Test::reportStart(const TestContext& ctx)
{
    std::list<Reporter*>::iterator it;
    for(it = _reporter.begin(); it != _reporter.end(); ++it)
    {
        (*it)->reportStart(ctx);
    }

    if(_parent)
        _parent->reportStart(ctx);
}


void Test::reportFinish(const TestContext& ctx)
{
    std::list<Reporter*>::iterator it;
    for(it = _reporter.begin(); it != _reporter.end(); ++it)
    {
        (*it)->reportFinish(ctx);
    }

    if(_parent)
        _parent->reportFinish(ctx);
}


void Test::reportSuccess(const TestContext& ctx)
{
    std::list<Reporter*>::iterator it;
    for(it = _reporter.begin(); it != _reporter.end(); ++it)
    {
        (*it)->reportSuccess(ctx);
    }

    if(_parent)
        _parent->reportSuccess(ctx);
}


void Test::reportSkip(const TestContext& ctx)
{
    std::list<Reporter*>::iterator it;
    for(it = _reporter.begin(); it != _reporter.end(); ++it)
    {
        (*it)->reportSkip(ctx);
    }

    if(_parent)
        _parent->reportSkip(ctx);
}


void Test::reportAssertion(const TestContext& ctx, const Assertion& ass)
{
    std::list<Reporter*>::iterator it;
    for(it = _reporter.begin(); it != _reporter.end(); ++it)
    {
        (*it)->reportAssertion(ctx, ass);
    }

    if(_parent)
        _parent->reportAssertion(ctx, ass);
}


void Test::reportException(const TestContext& ctx, const std::exception& ex)
{
    std::list<Reporter*>::iterator it;
    for(it = _reporter.begin(); it != _reporter.end(); ++it)
    {
        (*it)->reportException(ctx, ex);
    }

    if(_parent)
        _parent->reportException(ctx, ex);
}


void Test::reportError(const TestContext& ctx)
{
    std::list<Reporter*>::iterator it;
    for(it = _reporter.begin(); it != _reporter.end(); ++it)
    {
        (*it)->reportError(ctx);
    }

    if(_parent)
        _parent->reportError(ctx);
}


void Test::reportMessage(const std::string& msg)
{
    std::list<Reporter*>::iterator it;
    for(it = _reporter.begin(); it != _reporter.end(); ++it)
    {
        (*it)->reportMessage(msg);
    }

    if(_parent)
        _parent->reportMessage(msg);
}


void Test::setParent(Test* test)
{
    _parent = test;
}


Test* Test::parent()
{
    return _parent;
}


const Test* Test::parent() const
{
    return _parent;
}


void Test::attachReporter(Reporter& r)
{
    connect(r.destroyed, *this, &Test::detachReporter);
    _reporter.push_back(&r);
}


void Test::detachReporter(Reporter& r)
{
    _reporter.remove(&r);
}

}

}
