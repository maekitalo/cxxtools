/*
 * Copyright (C) 2006-2008 Marc Boris Duerner
 * Copyright (C) 2006-2008 Tommi Maekitalo
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
#include "threadimpl.h"
#include "cxxtools/thread.h"

namespace cxxtools {

Thread::Thread()
: _state(Thread::Ready)
, _impl(0)
{
    _impl = new ThreadImpl();
}


Thread::Thread(const Callable<void>& cb)
: _state(Thread::Ready)
, _impl(0)
{
    _impl = new ThreadImpl();
    _impl->init(cb);
}


Thread::~Thread()
{
    delete _impl;
}


void Thread::init(const Callable<void>& cb)
{
    _impl->init(cb);
}


void Thread::start()
{
    if( this->state() == Ready || this->state() == Finished )
    {
        _state = Thread::Running;
        _impl->start();
    }
}


void Thread::exit()
{
    ThreadImpl::exit();
}


void Thread::yield()
{
    ThreadImpl::yield();
}


void Thread::sleep(const cxxtools::Timespan& ts)
{
    ThreadImpl::sleep(ts);
}


void Thread::detach()
{
    _impl->detach();
}


void Thread::join()
{
    if( this->state() == Running )
    {
        _impl->join();
        _state = Thread::Finished;
    }
}


bool Thread::joinNoThrow()
{
    bool ret = true;
    if( this->state() == Running )
    {
        try
        {
            _impl->join();
        }
        catch(...)
        {
            ret = false;
        }

        _state = Thread::Finished;
    }

    return ret;
}


void Thread::terminate()
{
    _impl->terminate();
    _state = Thread::Finished;
}

} // !namespace cxxtools

