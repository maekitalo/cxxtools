/*
 * Copyright (C) 2008 Marc Boris Duerner
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
#include "applicationimpl.h"
#include "cxxtools/application.h"
#include "cxxtools/event.h"
#include <string>
#include <iostream>
#include <stdexcept>

namespace
{

cxxtools::Application*& getSystemAppPtr()
{
    static cxxtools::Application* _app = 0;
    return _app;
}

}

namespace cxxtools
{

void Application::construct()
{
    if( getSystemAppPtr() )
        throw std::logic_error("application already initialized");

    // base class already throws if constructed twice
    ::getSystemAppPtr() = this;

    _impl = new ApplicationImpl;

    _owner = new EventLoop();
    init(*_owner);
}

Application::Application()
: _argc(0)
, _argv(0)
, _loop(0)
, _owner(0)
{
    construct();
}


Application::Application(int argc, char** argv)
: _argc(argc)
, _argv(argv)
, _loop(0)
, _owner(0)
{
    construct();
}


Application::Application(EventLoopBase* loop)
: _argc(0)
, _argv(0)
, _loop(loop)
, _owner(0)
{
    construct();
}


Application::Application(EventLoopBase* loop, int argc, char** argv)
: _argc(argc)
, _argv(argv)
, _loop(loop)
, _owner(0)
{
    construct();
}


Application::~Application()
{
    delete _owner;
    ::getSystemAppPtr() = 0;

    delete _impl;
}


Application& Application::instance()
{
    Application* app = ::getSystemAppPtr();
    if( ! app )
        throw std::logic_error("application not initialized");

    return *app;
}


bool Application::catchSystemSignal(int sig)
{
    return _impl->catchSystemSignal(sig);
}


bool Application::raiseSystemSignal(int sig)
{
    return _impl->raiseSystemSignal(sig);
}


void Application::init(EventLoopBase& loop)
{
    _loop = &loop;
    _impl->init(*_loop);
}

} // namespace cxxtools
