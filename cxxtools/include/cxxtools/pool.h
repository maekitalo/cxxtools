////////////////////////////////////////////////////////////////////////
// pool.h
//

#ifndef POOL_H
#define POOL_H

#include <cxxtools/thread.h>
#include <stack>

template <class T>
class default_creator
{
  public:
    T* operator() ()
    { return new T(); }
};

template <typename T, typename CreatorType = default_creator<T> >
class pool
{
  public:
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

        void unlink()
        {
          if (mypool)
          {
            {
              MutexLock lock(mypool->mutex);
              if (next == this)
              {
                mypool->freePool.push(ptr);
                mypool->sem.Post();
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

        void release()
        {
          unlink();
          mypool = 0;
        }

        T* operator->()                    { return ptr; }
        const T* operator->() const        { return ptr; }
        T& operator*()                     { return *ptr; }
        const T& operator*() const         { return *ptr; }
        operator T* ()                     { return ptr; }
        operator const T* () const         { return ptr; }
        operator T& ()                     { return *ptr; }
        operator const T& () const         { return *ptr; }
        bool operator== (const T* p) const { return ptr == p; }
    };

    friend class poolobject;

    typedef poolobject objectptr_type;

  private:
    typedef std::stack<T*> objectcontainer_type;
    unsigned count;
    Semaphore sem;
    objectcontainer_type freePool;
    Mutex mutex;
    CreatorType creator;

  public:
    pool(unsigned _count, CreatorType _creator = CreatorType())
      : count(_count),
        sem(_count),
        creator(_creator)
    { }

    ~pool()
    {
      while (!freePool.empty())
      {
        delete freePool.top();
        freePool.pop();
      }
    }

    objectptr_type get()
    {
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

    void drop(unsigned keep = 0)
    {
      MutexLock lock(mutex);
      unsigned n = count;
      while (n-- > keep && !freePool.empty())
      {
        delete freePool.top();
        freePool.pop();
      }
    }
};

#endif // POOL_H

