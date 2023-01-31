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
    std::mutex _mutex;
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

    unsigned size() const
    {
        return _freePool.size();
    }

    void drop(unsigned keep = 0)
    {
        std::lock_guard<std::mutex> lock(_mutex);
        if (_freePool.size() > keep)
          _freePool.resize(keep);
    }
};

}

#endif
