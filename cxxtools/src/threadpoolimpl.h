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

#ifndef CXXTOOLS_THREADPOOLIMPL_H
#define CXXTOOLS_THREADPOOLIMPL_H

#include <cxxtools/queue.h>
#include <cxxtools/thread.h>
#include <vector>

namespace cxxtools
{
    class ThreadPoolImpl
    {
        public:
            explicit ThreadPoolImpl(unsigned size)
                : _state(Stopped),
                  _size(size)
                  { }

            ~ThreadPoolImpl();

            void start();

            void stop(bool cancel);

            void schedule(const Callable<void>& cb);

            bool running() const
            { return _state == Running; }

            bool stopped() const
            { return _state == Stopped; }

        private:
            void threadFunc();

            enum {
                Stopped,
                Starting,
                Running,
                Stopping
            } _state;

            Queue<Callable<void>*> _queue;
            typedef std::vector<AttachedThread*> ThreadsType;
            ThreadsType _threads;
            unsigned _size;
    };
}

#endif // CXXTOOLS_THREADPOOLIMPL_H
