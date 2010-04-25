/*
 * Copyright (C) 2010 Tommi Maekitalo
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * is provided AS IS, WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, and
 * NON-INFRINGEMENT.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 *
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

    void ThreadPool::schedule(const Callable<void>& cb)
    {
        _impl->schedule(cb);
    }

    bool ThreadPool::running() const
    {
        return _impl->running();
    }

    bool ThreadPool::stopped() const
    {
        return _impl->stopped();
    }

}
