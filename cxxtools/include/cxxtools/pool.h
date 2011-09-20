/*
 * Copyright (C) 2011 Tommi Maekitalo
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

#include <cxxtools/smartptr.h>
#include <cxxtools/noncopyable.h>
#include <cxxtools/mutex.h>
#include <vector>

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
   A Pool is a container for pooled objects.

   It maintains a list of object instances, which are not in use. If a
   program needs a instance, it can request one with the get-method. The
   pool returns a smart pointer to an instance. When the pointer is released,
   the instance is put back into the pool or dropped, if the pool has already
   a maximum number of instances.
   */
  template <typename ObjectType,
            typename CreatorType = DefaultCreator<ObjectType>,
            template <class> class OwnershipPolicy = RefLinked,
            template <class> class DestroyPolicy = DeletePolicy>
  class Pool : private NonCopyable
  {
    public:
      class Ptr : public OwnershipPolicy<ObjectType>,
                  public DestroyPolicy<ObjectType>
      {
          ObjectType* object;
          Pool* pool;
          typedef OwnershipPolicy<ObjectType> OwnershipPolicyType;
          typedef DestroyPolicy<ObjectType> DestroyPolicyType;

          void doUnlink()
          {
            if (OwnershipPolicyType::unlink(object))
            {
              if (pool == 0 || !pool->put(*this))
                destroy(object);
            }
          }

        public:
          Ptr()
            : object(0)
            {}
          Ptr(ObjectType* ptr)
            : object(ptr)
            { OwnershipPolicyType::link(*this, ptr); }
          Ptr(const Ptr& ptr)
            : object(ptr.object),
              pool(ptr.pool)
            { OwnershipPolicyType::link(ptr, ptr.object); }
          ~Ptr()
            { doUnlink(); }

          Ptr& operator= (const Ptr& ptr)
          {
            if (object != ptr.object)
            {
              doUnlink();
              object = ptr.object;
              pool = ptr.pool;
              OwnershipPolicyType::link(ptr, object);
            }
            return *this;
          }

          /// The object can be dereferenced like the held object
          ObjectType* operator->() const              { return object; }
          /// The object can be dereferenced like the held object
          ObjectType& operator*() const               { return *object; }

          bool operator== (const ObjectType* p) const { return object == p; }
          bool operator!= (const ObjectType* p) const { return object != p; }
          bool operator< (const ObjectType* p) const  { return object < p; }
          bool operator! () const { return object == 0; }
          operator bool () const  { return object != 0; }

          ObjectType* getPointer()              { return object; }
          const ObjectType* getPointer() const  { return object; }
          operator ObjectType* ()               { return object; }
          operator const ObjectType* () const   { return object; }

          void setPool(Pool* p)
          { pool = p; }

          // don't put the object back to the pool
          void release()
          { pool = 0; }
      };

    private:
      typedef std::vector<Ptr> Container;
      Container freePool;
      unsigned maxSpare;
      mutable Mutex mutex;
      CreatorType creator;

      bool put(Ptr& po) // returns true, if object was put into the freePool vector
      {
        MutexLock lock(mutex);
        if (maxSpare == 0 || freePool.size() < maxSpare)
        {
          po.setPool(0);
          freePool.push_back(po);
          return true;
        }

        return false;
      }

    public:
      explicit Pool(unsigned maxSpare_ = 0, CreatorType creator_ = CreatorType())
        : maxSpare(maxSpare_),
          creator(creator_)
          { }

      explicit Pool(CreatorType creator_)
        : maxSpare(0),
          creator(creator_)
          { }

      ~Pool()
      {
        freePool.clear();
      }

      Ptr get()
      {
        MutexLock lock(mutex);
        Ptr po;
        if (freePool.empty())
        {
          po = creator();
        }
        else
        {
          po = freePool.back();
          freePool.pop_back();
        }

        po.setPool(this);
        return po;
      }

      void drop(unsigned keep = 0)
      {
        MutexLock lock(mutex);
        if (freePool.size() > keep)
          freePool.resize(keep);
      }

      unsigned getMaximumSize() const
      {
        MutexLock lock(mutex);
        return maxSpare;
      }

      unsigned size() const
      {
        MutexLock lock(mutex);
        return freePool.size();
      }

      unsigned getCurrentSize() const
      {
        MutexLock lock(mutex);
        return freePool.size();
      }

      void setMaximumSize(unsigned s)
      {
        MutexLock lock(mutex);
        maxSpare = s;
        if (freePool.size() > s)
          freePool.resize(s);
      }

      CreatorType& getCreator()
      { return creator; }

      const CreatorType& getCreator() const
      { return creator; }

  };

}
