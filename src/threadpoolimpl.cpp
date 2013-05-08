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
