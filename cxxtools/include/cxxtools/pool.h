/* cxxtools/pool.h
   Copyright (C) 2003 Tommi Mäkitalo

This file is part of cxxtools.

Cxxtools is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

Cxxtools is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with cxxtools; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330,
Boston, MA  02111-1307  USA
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
class default_creator
{
  public:
    T* operator() ()
    { return new T(); }
};

/**
 A pool is a container for reusable objects.

 It maintains a list of objects, which are not in use. If a program
 needs a object, he can request one with the get-method. The pool
 returns a object, which is convertable to the needed object. As long
 as the program holds this special object, the object is in use.

 Objects are created when needed and only deleted on request.
 A upper limit of objects can be passed to the constructor. This makes
 only sense in a multithreaded environment, because the get-method
 blocks, when the limit is reached and waits for someone to release a
 object.
 */
template <typename T, typename CreatorType = default_creator<T> >
class pool
{
  public:
    /**
     The poolobject holds a reference to the object.
     The held object is released for reuse, when the last reference
     is destroyed.
     */
    class poolobject
    {
      public:
        typedef pool<T, CreatorType> pool_type;
        typedef poolobject* ptr_type;
        typedef const poolobject* const_ptr_type;

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
                    mypool->sem.Post();
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

        void link(const poolobject& s)
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
        poolobject()
          : ptr(0),
            mypool(0),
            prev(this),
            next(this)
        { }

        poolobject(T* o, pool_type& p)
          : ptr(o),
            mypool(&p),
            prev(this),
            next(this)
        { }

        poolobject(const poolobject& s)
          : ptr(s.ptr),
            mypool(s.mypool),
            prev(this),
            next(this)
        {
          if (mypool)
            link(s);
        }

        ~poolobject()
        {
          unlink();
        }

        poolobject& operator= (const poolobject& s)
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

    friend class poolobject;

    typedef poolobject objectptr_type;

  private:
    typedef std::stack<T*> objectcontainer_type;
    unsigned maxcount;
    Semaphore sem;
    objectcontainer_type freePool;
    Mutex mutex;
    CreatorType creator;

  public:
    /// Create a pool with a maximum count.
    /// If count is 0, no limit is used
    explicit pool(unsigned _maxcount = 0, CreatorType _creator = CreatorType())
      : maxcount(_maxcount),
        sem(_maxcount),
        creator(_creator)
    { }

    /// create a pool without limit and a special creator
    explicit pool(CreatorType _creator)
      : maxcount(0),
        sem(0),
        creator(_creator)
    { }

    /// destructing the pool destroys all free objects too
    ~pool()
    {
      while (!freePool.empty())
      {
        delete freePool.top();
        freePool.pop();
      }
    }

    /**
     Returns a wrapper-class, which helds the object in use.

     This method does the actual work of the pool. It creates new
     objects, when needed and the upper limit is not reached.
     */
    objectptr_type get()
    {
      if (maxcount > 0)
        sem.Wait();

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
};

}

#endif // CXXTOOLS_POOL_H

