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

#include "threadpoolimpl.h"
#include <stdexcept>
#include <cxxtools/log.h>

log_define("cxxtools.threadpool.impl")

namespace cxxtools
{
    ThreadPoolImpl::~ThreadPoolImpl()
    {
        log_debug("delete " << _threads.size() << " threads");
        for (ThreadsType::iterator it = _threads.begin(); it != _threads.end(); ++it)
            delete *it;

        log_debug("delete " << _queue.size() << " left tasks");
        while (!_queue.empty())
            delete _queue.get();
    }

    void ThreadPoolImpl::start()
    {
        if (_state != Stopped)
            throw std::logic_error("invalid state");

        _state = Starting;

        while (_threads.size() < _size)
            _threads.push_back(new AttachedThread(callable(*this, &ThreadPoolImpl::threadFunc)));

        _state = Running;

        for (ThreadsType::iterator it = _threads.begin(); it != _threads.end(); ++it)
        {
            log_debug("start thread " << static_cast<void*>(*it));
            (*it)->start();
        }
    }

    void ThreadPoolImpl::stop(bool cancel)
    {
        if (_state != Running)
            throw std::logic_error("thread pool not running");

        log_debug("stop " << _threads.size() << " threads");
        _state = Stopping;

        if (cancel)
        {
            std::pair<Callable<void>*, bool> c;
            while ((c = _queue.tryGet()).second)
                delete c.first;
        }

        for (ThreadsType::iterator it = _threads.begin(); it != _threads.end(); ++it)
            _queue.put(0);

        for (ThreadsType::iterator it = _threads.begin(); it != _threads.end(); ++it)
        {
            (*it)->join();
            log_debug("joined thread " << static_cast<void*>(*it));
            delete *it;
        }

        _threads.clear();

        _state = Stopped;
    }

    void ThreadPoolImpl::schedule(const Callable<void>& cb)
    {
        Callable<void>* c = cb.clone();
        log_debug("queue new task " << static_cast<void*>(c));
        _queue.put(c);
        log_debug("queue size " << _queue.size());
    }

    void ThreadPoolImpl::threadFunc()
    {
        Callable<void>* c = 0;
        while ((c = _queue.get()) != 0)
        {
            log_debug("new task " << static_cast<void*>(c) << " received " << _queue.size() << " tasks left");

            try
            {
                (*c)();
                delete c;
            }
            catch (...)
            {
                delete c;
            }

            log_debug("task " << static_cast<void*>(c) << " finished");
        }

        log_debug("end thread");
    }

}
