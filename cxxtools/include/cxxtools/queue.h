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

#ifndef CXXTOOLS_QUEUE_H
#define CXXTOOLS_QUEUE_H

#include <queue>
#include <cxxtools/mutex.h>
#include <cxxtools/condition.h>

namespace cxxtools
{
    /** @brief This class implements a thread safe queue.

        A queue is a container where the elements put into the queue are
        fetched in the same order (first-in-first-out, fifo).
        The class has a optional maximum size. If the size is set to 0 the
        queue has no limit. Otherwise putting a element to the queue may
        block until another thread fetches a element or increases the limit.
     */
    template <typename T>
    class Queue
    {
        public:
            typedef T value_type;
            typedef typename std::deque<T>::size_type size_type;
            typedef typename std::deque<T>::const_reference const_reference;

        private:
            mutable Mutex _mutex;
            Condition _notEmpty;
            Condition _notFull;
            std::deque<value_type> _queue;
            size_type _maxSize;
            size_type _numWaiting;

        public:
            /// @brief Default Constructor.
            Queue()
                : _maxSize(0),
                  _numWaiting(0)
            { }

            /** @brief Returns the next element.

                This method returns the next element. If the queue is empty,
                the thread will be locked until a element is available.
             */
            value_type get();

            /** @brief Returns the next element if the queue is not empty.

                If the queue is empty, a default constructed value_type is returned.
                The returned flag is set to false, if the queue was empty.
             */
            std::pair<value_type, bool> tryGet();

            /** @brief Adds a element to the queue.

                This method adds a element to the queue. If the queue has
                reached his maximum size, the method blocks until there is
                space available.
             */
            void put(const_reference element);

            /// @brief Returns true, if the queue is empty.
            bool empty() const
            {
                MutexLock lock(_mutex);
                return _queue.empty();
            }

            /// @brief Returns the number of elements currently in queue.
            size_type size() const
            {
                MutexLock lock(_mutex);
                return _queue.size();
            }

            /** @brief sets the maximum size of the queue.

                Setting the maximum size of the queue may wake up another
                thread, if it is waiting for space to get available and the
                limit is increased.
             */
            void maxSize(size_type m);

            /// @brief returns the maximum size of the queue.
            size_type maxSize() const
            {
                MutexLock lock(_mutex);
                return _maxSize;
            }

            /// @brief returns the number of threads blocked in the get method.
            size_type numWaiting() const
            {
                MutexLock lock(_mutex);
                return _numWaiting;
            }
    };

    template <typename T>
    typename Queue<T>::value_type Queue<T>::get()
    {
        MutexLock lock(_mutex);

        ++_numWaiting;
        while (_queue.empty())
            _notEmpty.wait(lock);
        --_numWaiting;

        value_type element = _queue.front();
        _queue.pop_front();

        if (!_queue.empty())
            _notEmpty.signal();

        _notFull.signal();

        return element;
    }

    template <typename T>
    std::pair<typename Queue<T>::value_type, bool> Queue<T>::tryGet()
    {
        MutexLock lock(_mutex);

        if (_queue.empty())
            return std::pair<typename Queue<T>::value_type, bool>(Queue<T>::value_type(), false);

        value_type element = _queue.front();
        _queue.pop_front();

        if (!_queue.empty())
            _notEmpty.signal();

        _notFull.signal();

        return std::pair<typename Queue<T>::value_type, bool>(element, true);
    }

    template <typename T>
    void Queue<T>::put(typename Queue<T>::const_reference element)
    {
        MutexLock lock(_mutex);

        while (_maxSize > 0 && _queue.size() >= _maxSize)
            _notFull.wait(lock);

        _queue.push_back(element);
        _notEmpty.signal();

        if (_maxSize > 0 && _queue.size() < _maxSize)
            _notFull.signal();
    }

    template <typename T>
    void Queue<T>::maxSize(size_type m)
    {
        _maxSize = m;
        MutexLock lock(_mutex);
        if (_queue.size() < _maxSize)
            _notFull.signal();
    }
}

#endif // CXXTOOLS_QUEUE_H

