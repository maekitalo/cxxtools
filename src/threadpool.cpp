/*
 * Copyright (C) 2010 Tommi Maekitalo
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

#include <cxxtools/threadpool.h>
#include "threadpoolimpl.h"

namespace cxxtools
{
    ThreadPool::ThreadPool(unsigned size, bool doStart)
        : _impl(new ThreadPoolImpl(size))
    {
        if (doStart)
            start();
    }

    ThreadPool::~ThreadPool()
    {
        if (running())
            stop();
        delete _impl;
    }

    void ThreadPool::start()
    {
        _impl->start();
    }

    void ThreadPool::stop(bool cancel)
    {
        _impl->stop(cancel);
    }

    ThreadPool::Future ThreadPool::schedule(const Callable<void>& cb)
    {
        return _impl->schedule(cb);
    }

    bool ThreadPool::running() const
    {
        return _impl->running();
    }

    bool ThreadPool::stopped() const
    {
        return _impl->stopped();
    }

    ThreadPool::Future::Future(FutureImpl* impl)
        : _impl(impl)
    {
        if (impl)
            _impl->addRef();
    }

    ThreadPool::Future::Future(const Future& f)
        : _impl(f._impl)
    {
        if (_impl)
            _impl->addRef();
    }

    ThreadPool::Future& ThreadPool::Future::operator=(const Future& f)
    {
        if (_impl != f._impl)
        {
            if (_impl && _impl->release() == 0)
                delete _impl;

            _impl = f._impl;

            if (_impl)
                _impl->addRef();
        }

        return *this;
    }

    ThreadPool::Future::~Future()
    {
        if (_impl && _impl->release() == 0)
            delete _impl;
    }

    bool ThreadPool::Future::wait(Seconds timeout) const
    {
        return _impl->wait(timeout);
    }

    bool ThreadPool::Future::isWaiting() const
    {
        return _impl->state() == ThreadPool::Future::FutureImpl::Waiting;
    }

    bool ThreadPool::Future::isRunning() const
    {
        return _impl->state() == ThreadPool::Future::FutureImpl::Running;
    }

    bool ThreadPool::Future::isFinished() const
    {
        ThreadPool::Future::FutureImpl::State state = _impl->state();
        return state == ThreadPool::Future::FutureImpl::Finished
            || state == ThreadPool::Future::FutureImpl::Failed
            || state == ThreadPool::Future::FutureImpl::Canceled;
    }

    bool ThreadPool::Future::isCanceled() const
    {
        return _impl->state() == ThreadPool::Future::FutureImpl::Canceled;
    }

    bool ThreadPool::Future::isFailed() const
    {
        return _impl->state() == ThreadPool::Future::FutureImpl::Failed;
    }

}
