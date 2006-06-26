/* cxxtools/pool.h
 *
 * cxxtools - general purpose C++-toolbox
 * Copyright (C) 2003 Tommi Maekitalo
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#ifndef CXXTOOLS_POOL_H
#define CXXTOOLS_POOL_H

#include <cxxtools/thread.h>
#include <stack>

namespace cxxtools
{

/**
 This class is a factory for objects wich are default constructable.
 */
template <class T>
class DefaultCreator
{
  public:
    T* operator() ()
    { return new T(); }
};

/**
 A Pool is a container for reusable objects.

 It maintains a list of objects, which are not in use. If a program
 needs a object, he can request one with the get-method. The Pool
 returns a object, which is convertable to the needed object. As long
 as the program holds this special object, the object is in use.

 Objects are created when needed and only deleted on request.
 A upper limit of objects can be passed to the constructor. This makes
 only sense in a multithreaded environment, because the get-method
 blocks, when the limit is reached and waits for someone to release a
 object.
 */
template <typename T, typename CreatorType = DefaultCreator<T> >
class Pool
{
  public:
    /**
     The PoolObject holds a reference to the object.
     The held object is released for reuse, when the last reference
     is destroyed.
     */
    class PoolObject
    {
      public:
        typedef Pool<T, CreatorType> pool_type;
        typedef PoolObject* ptr_type;
        typedef const PoolObject* const_ptr_type;

      private:
        T* ptr;
        pool_type* mypool;

        mutable const_ptr_type prev;
        mutable const_ptr_type next;

        void unlink(bool reuse = true)
        {
          if (mypool)
          {
            {
              MutexLock lock(mypool->mutex);
              if (next == this)
              {
                if (reuse)
                {
                  mypool->freePool.push(ptr);
                  if (mypool->maxcount > 0)
                    mypool->sem.post();
                }
                else
                  delete ptr;
              }
              else
              {
                next->prev = prev;
                prev->next = next;
              }
            }
            next = prev = this;
          }
        }

        void link(const PoolObject& s)
        {
          mypool = s.mypool;
          ptr = s.ptr;
          if (mypool)
          {
            prev = &s;
            next = s.next;
            MutexLock lock(mypool->mutex);
            prev->next = this;
            next->prev = this;
          }
        }

      public:
        PoolObject()
          : ptr(0),
            mypool(0),
            prev(this),
            next(this)
        { }

        PoolObject(T* o, pool_type& p)
          : ptr(o),
            mypool(&p),
            prev(this),
            next(this)
        { }

        PoolObject(const PoolObject& s)
          : ptr(s.ptr),
            mypool(s.mypool),
            prev(this),
            next(this)
        {
          if (mypool)
            link(s);
        }

        ~PoolObject()
        {
          unlink();
        }

        PoolObject& operator= (const PoolObject& s)
        {
          if (ptr != s.ptr)
          {
            unlink();
            link(s);
          }
          return *this;
        }

        /**
         The reference can be dropped before destruction.
         */
        void release(bool reuse = true)
        {
          unlink(reuse);
          mypool = 0;
        }

        /// The object can be dereferenced like the held object
        T* operator->() const              { return ptr; }
        /// The object can be dereferenced like the held object
        T& operator*() const               { return *ptr; }
        /// The object is convertable to a pointer to the held object
        operator T* () const               { return ptr; }
        /// The object is convertable to a reference to the held object
        operator T& () const               { return *ptr; }
        bool operator== (const T* p) const { return ptr == p; }
    };

    friend class PoolObject;

    typedef PoolObject objectptr_type;

  private:
    typedef std::stack<T*> objectcontainer_type;
    unsigned maxcount;
    Semaphore sem;
    objectcontainer_type freePool;
    mutable Mutex mutex;
    CreatorType creator;

    // make non-copyable
    Pool(const Pool&); // no implementation
    Pool& operator= (const Pool&); // no implementation

  public:
    /// Create a Pool with a maximum count.
    /// If count is 0, no limit is used
    explicit Pool(unsigned _maxcount = 0, CreatorType _creator = CreatorType())
      : maxcount(_maxcount),
        sem(_maxcount),
        creator(_creator)
    { }

    /// create a Pool without limit and a special creator
    explicit Pool(CreatorType _creator)
      : maxcount(0),
        sem(0),
        creator(_creator)
    { }

    /// destructing the Pool destroys all free objects too
    ~Pool()
    {
      while (!freePool.empty())
      {
        delete freePool.top();
        freePool.pop();
      }
    }

    /**
     Returns a wrapper-class, which helds the object in use.

     This method does the actual work of the Pool. It creates new
     objects, when needed and the upper limit is not reached.
     */
    objectptr_type get()
    {
      if (maxcount > 0)
        sem.wait();

      T* obj;

      {
        MutexLock lock(mutex);

        if (freePool.empty())
          obj = creator();
        else
        {
          obj = freePool.top();
          freePool.pop();
        }
      }

      objectptr_type po(obj, *this);
      return po;
    }

    /**
     Release all free objects.

     A maximum number of objects kept can be passed.
     Objects in use are not counted at all.
     */
    void drop(unsigned keep = 0)
    {
      MutexLock lock(mutex);
      while (freePool.size() > keep)
      {
        delete freePool.top();
        freePool.pop();
      }
    }

    unsigned getCurrentSize() const
    {
      MutexLock lock(mutex);
      return freePool.size();
    }
};

}

#endif // CXXTOOLS_POOL_H

