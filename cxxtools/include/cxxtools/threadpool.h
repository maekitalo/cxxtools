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

#ifndef CXXTOOLS_THREADPOOL_H
#define CXXTOOLS_THREADPOOL_H

#include <cxxtools/callable.h>

namespace cxxtools
{
    class ThreadPoolImpl;

    class ThreadPool
    {
        public:
            /** @brief Creates a thread pool structure.

                When the argument \a doStart is set to true (which is the
                default), the threads are started.
             */
            explicit ThreadPool(unsigned size, bool doStart = true);

            /** @brief Destroys a thread pool structure.

                Before destruction all jobs are processed and the threads are
                stopped.
             */
            ~ThreadPool();

            /** @brief Explict start of the thread pool.
             */
            void start();

            /** @brief Stops the thread of the thread pool.

                All threads of this pool are stopped. The current job is
                finished.  Remaining jobs are discarded, when cancel is set.
                Otherwise the threads stop, when the queue is empty.
             */
            void stop(bool cancel = false);

            /** @brief Schedules a task to be processed.

                The task is processed by the next available thread.
             */
            void schedule(const Callable<void>& cb);

            /** @brief Returns true, if the threadpool is in running state.
             */
            bool running() const;

            /** @brief Returns true, if the threadpool is in stopped state.
             */
            bool stopped() const;

        private:
            ThreadPoolImpl* _impl;
    };
};

#endif // CXXTOOLS_THREADPOOL_H
