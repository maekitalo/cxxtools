/*
 * Copyright (C) 2006-2008 by Marc Boris Duerner
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

#include "processimpl.h"
#include "cxxtools/process.h"

namespace cxxtools {

Process::Process(const std::string& command)
{
    _impl = new ProcessImpl( ProcessInfo(command) );
}


Process::Process(const ProcessInfo& procInfo)
{
    _impl = new ProcessImpl( procInfo);
}


Process::~Process()
{
    try {
        this->kill();
    } 
    catch(...) {}

    delete _impl;
}


const ProcessInfo& Process::procInfo() const
{
    return _impl->procInfo();
}


Process::State Process::state() const
{
    return _impl->state();
}


void Process::start()
{
     _impl->start();
}


void Process::kill()
{
     _impl->kill();
}


int Process::wait() 
{
    return _impl->wait();
}


void Process::setEnvVar(const std::string& name, const std::string& value)
{
    ProcessImpl::setEnvVar(name, value);
}


void Process::unsetEnvVar(const std::string& name)
{
     ProcessImpl::unsetEnvVar(name);
}


std::string Process::getEnvVar(const std::string& name)
{
     return ProcessImpl::getEnvVar(name);
}


void Process::sleep(size_t milliSec)
{
     ProcessImpl::sleep(milliSec);
}


unsigned long Process::usedMemory()
{
     return ProcessImpl::usedMemory();
}

} // namespace cxxtools
