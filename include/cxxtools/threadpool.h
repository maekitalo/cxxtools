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

#ifndef CXXTOOLS_THREADPOOL_H
#define CXXTOOLS_THREADPOOL_H

#include <cxxtools/callable.h>
#include <cxxtools/timespan.h>

namespace cxxtools
{
    class ThreadPoolImpl;

    class ThreadPool
    {
            ThreadPool(const ThreadPool&) { }
            ThreadPool& operator=(const ThreadPool&) { return *this; }

        public:
            /**
                The Future class monitors the state of a job, which runs in the thread pool.

                When a job is run on the thread pool, a future is returned. The
                user can wait for the future to be finished. Futures can be
                copied, in which case all instances point to the same instance.
             */
            class Future
            {
                    friend class ThreadPoolImpl;

                    class FutureImpl;
                    FutureImpl* _impl;
                    Future(FutureImpl* impl);

                public:
                    /// A Future is default constructable.
                    Future()
                        : _impl(0)
                    { }

                    /// A Future is copyable.
                    Future(const Future& f);

                    /// A Future is assignable.
                    Future& operator=(const Future& f);

                    ~Future();

                    /** Wait up to timeout seconds for termination.

                        Wait up to timeout seconds for termination.
                        Return true when job was finished before timeout.
                     */
                    bool wait(Seconds timeout = -1) const;

                    /// Returns true, when the job is waiting to be run on a thread.
                    bool isWaiting() const;

                    /// Returns true, when the job is currently running on a thread.
                    bool isRunning() const;

                    /** Returns true, when the job is finished.

                        Finished may be either successfully finished, finished by exception
                        or canceled because the thread pool is stopped before the job started
                        to run.
                     */
                    bool isFinished() const;

                    /** Returns true, when the job is canceled.
                        Canceled means, that the thread pool is stopped before the job
                        started.
                     */
                    bool isCanceled() const;

                    /// Returns true, when the job was finished with exceptoin.
                    bool isFailed() const;

            };

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
            Future schedule(const Callable<void>& cb);

            /** @brief Returns true, if the threadpool is in running state.
             */
            bool running() const;

            /** @brief Returns true, if the threadpool is in stopped state.
             */
            bool stopped() const;

        private:
            ThreadPoolImpl* _impl;
    };
}

#endif // CXXTOOLS_THREADPOOL_H
