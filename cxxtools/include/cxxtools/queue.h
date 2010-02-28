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
    template <typename T>
    class Queue
    {
        public:
            typedef T value_type;
            typedef typename std::deque<T>::size_type size_type;

        private:
            Mutex _mutex;
            Condition _notEmpty;
            Condition _notFull;
            std::deque<value_type> _queue;
            size_type _maxSize;
            size_type _numWaiting;

        public:
            Queue()
                : _maxSize(0),
                  _numWaiting(0)
            { }

            value_type get();
            void put(value_type element);

            bool empty() const            { return _queue.empty(); }
            size_type size() const        { return _queue.size(); }
            void maxSize(size_type m);
            size_type maxSize() const     { return _maxSize; }
            size_type numWaiting() const  { return _numWaiting; }
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
    void Queue<T>::put(Queue<T>::value_type element)
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

