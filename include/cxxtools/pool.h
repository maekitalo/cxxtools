/*
 * Copyright (C) 2011,2023 Tommi Maekitalo
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

#ifndef CXXTOOLS_POOL_H
#define CXXTOOLS_POOL_H

#include <mutex>
#include <vector>
#include <memory>

namespace cxxtools
{
/// This class is a factory for objects wich are default constructable.
template <class T>
class DefaultCreator
{
public:
  T* operator() ()
  { return new T(); }
};

template <typename ObjectType,
          typename CreatorType = DefaultCreator<ObjectType>>
class Pool
{
    friend class Deleter;

    class Deleter
    {
        Pool* _pool;
    public:
        explicit Deleter(Pool* pool)
            : _pool(pool)
            { }

        void operator() (ObjectType* o)
        {
            std::lock_guard<std::mutex> lock(_pool->_mutex);
            if (_pool->_maxSpare == 0 || _pool->_freePool.size() < _pool->_maxSpare)
                _pool->_freePool.emplace_back(o);
            else
                delete o;
        }
    };

    Pool(const Pool&) = delete;
    Pool& operator=(const Pool&) = delete;

    std::vector<std::unique_ptr<ObjectType>> _freePool;
    unsigned _maxSpare;
    mutable std::mutex _mutex;
    CreatorType _creator;

public:
    typedef std::shared_ptr<ObjectType> Ptr;

    explicit Pool(unsigned maxSpare = 0, const CreatorType& creator = CreatorType())
        : _maxSpare(maxSpare),
          _creator(creator)
          { }

    explicit Pool(const CreatorType& creator)
        : _maxSpare(0),
          _creator(creator)
          { }

    std::shared_ptr<ObjectType> get()
    {
        std::lock_guard<std::mutex> lock(_mutex);

        ObjectType* ptr;

        if (_freePool.empty())
        {
            ptr = _creator();
        }
        else
        {
            ptr = std::move(_freePool.back()).release();
            _freePool.pop_back();
        }

        return std::shared_ptr<ObjectType> (ptr, Deleter(this));
    }

    unsigned getMaximumSize() const
    {
        std::lock_guard<std::mutex> lock(_mutex);
        return _maxSpare;
    }

    unsigned size() const
    {
        return _freePool.size();
    }

    unsigned getCurrentSize() const
    {
        std::lock_guard<std::mutex> lock(_mutex);
        return _freePool.size();
    }

    void drop(unsigned keep = 0)
    {
        std::lock_guard<std::mutex> lock(_mutex);
        if (_freePool.size() > keep)
          _freePool.resize(keep);
    }

    void setMaximumSize(unsigned s)
    {
        std::lock_guard<std::mutex> lock(_mutex);
        _maxSpare = s;
        if (_freePool.size() > s)
            _freePool.resize(s);
    }

    CreatorType& getCreator()
    { return _creator; }

    const CreatorType& getCreator() const
    { return _creator; }

};

}

#endif
