/*
 * Copyright (C) 2005 Tommi Maekitalo
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

#ifndef CXXTOOLS_SMARTPTR_H
#define CXXTOOLS_SMARTPTR_H

#include <cxxtools/atomicity.h>

namespace cxxtools
{
  /**
      \param ObjectType The managed object type
  */
  template <typename ObjectType>
  /** \brief Reference linking.

      Reference linking means that no counter is required to keep track of
      the smart pointer objects, but all smart pointers form a linked
      list. When the list becomes empty the raw pointer si deleted. This
      Model has the advantage that it does not need to allocate memory,
      but is prone to circular dependencies.
  */
  class RefLinked
  {
      mutable const RefLinked* prev;
      mutable const RefLinked* next;

    protected:
      RefLinked()
        : prev(0),
          next(0)
          { }

      //! \brief Unlink a smart pointer from a managed object
      bool unlink(ObjectType* object)
      {
        if (object)
        {
          if (next == this)
          {
            next = prev = 0;
            return true;
          }
          else
          {
            next->prev = prev;
            prev->next = next;
            next = prev = this;
          }
        }
        return false;
      }

      //! \brief Link a smart pointer to a managed object
      void link(const RefLinked& ptr, ObjectType* object)
      {
        if (object)
        {
          if (ptr.next)
          {
            prev = &ptr;
            next = ptr.next;
            prev->next = this;
            next->prev = this;
          }
          else
          {
            prev = next = this;
          }
        }
      }

  };

  /**
      \param ObjectType The managed object type
  */
  template <typename ObjectType>
  /** \brief Intrusive reference counting.

      Intrusive reference couting means that the reference count is part of the
      managed heap object. Linking and unlinking will only increase and decrease this
      counter, but not delete it. The managed object needs to implement the functions
      release() and addRef() and must delete itself if the counter reaches zero.
  */
  class InternalRefCounted
  {
    protected:
      //! \brief unlink a smart pointer from a managed object
      bool unlink(ObjectType* object)
      {
        if (object)
          object->release();
        return false;
      }

      //! \brief link a smart pointer to a managed object
      void link(const InternalRefCounted& ptr, ObjectType* object)
      {
        if (object)
          object->addRef();
      }

  };

  /**
      \param ObjectType The managed object type
  */
  template <typename ObjectType>
  /** \brief Non-intrusive reference counting.

      Non-intrusive reference couting means that the reference count is not part of the
      managed heap object but part of the policy. Linking and unlinking will increase and
      decrease the policies counter and delete the managed object if it reaches zero. A
      small amount of memory needs to be allocated for the counter variable.
  */
  class ExternalRefCounted
  {
      unsigned* rc;

    protected:
      ExternalRefCounted()
        : rc(0)  { }

      //! \brief unlink a smart pointer from a managed object
      bool unlink(ObjectType* object)
      {
        if (object && --*rc <= 0)
        {
          delete rc;
          rc = 0;
          return true;
        }
        else
          return false;
      }

      //! \brief link a smart pointer to a managed object
      void link(const ExternalRefCounted& ptr, ObjectType* object)
      {
        if (object)
        {
          if (ptr.rc == 0)
            rc = new unsigned(1);
          else
          {
            rc = ptr.rc;
            ++*rc;
          }
        }
        else
          rc = 0;
      }

    public:
      unsigned refs() const
        { return rc ? *rc : 0; }
  };

  template <typename ObjectType>
  class ExternalAtomicRefCounted
  {
      volatile atomic_t* rc;

    protected:
      ExternalAtomicRefCounted()
        : rc(0)  { }

      bool unlink(ObjectType* object)
      {
        if (object && atomicDecrement(*rc) <= 0)
        {
          delete rc;
          rc = 0;
          return true;
        }
        else
          return false;
      }

      void link(const ExternalAtomicRefCounted& ptr, ObjectType* object)
      {
        if (object)
        {
          if (ptr.rc == 0)
            rc = new atomic_t(1);
          else
          {
            rc = ptr.rc;
            atomicIncrement(*rc);
          }
        }
        else
          rc = 0;
      }

    public:
      atomic_t refs() const
        { return rc ? atomicGet(*rc) : 0; }
  };

  /**
      \param ObjectType The managed object type
  */
  template <typename ObjectType>
  /** \brief deleter policy for smart pointer

      The DeletePolicy implements the method, which instructs the SmartPtr to free the
      object which it helds by deleting it.
  */
  class DeletePolicy
  {
    public:
      static void destroy(ObjectType* ptr)
      { delete ptr; }
  };

  template <typename T>
  class FreeDestroyPolicy
  {
    public:
      static void destroy(T* ptr)
      { free(ptr); }
  };

  template <typename ObjectType>
  class ArrayDestroyPolicy
  {
    public:
      static void destroy(ObjectType* ptr)
      { delete[] ptr; }
  };

  /**
   * Policy-based smart-pointer-class.
   *
   * This class works like a pointer, but the destructor deletes the held
   * object if this is the last reference. The policy specifies, how the class
   * counts the references. There are 4 policies:
   *
   *   ExternalRefCounted: allocates a reference-count
   *
   *   ExternalAtomicRefCounted: like ExternalRefCounted, but thread safe
   *
   *   InternalRefCounted: the pointed object needs to have a reference-counter
   *     with methods addRef() and release(). The release-method deletes the
   *     object, when the reference-count drops to 0.
   *
   *   RefLinked: all pointers to a object are linked
   *
   * The default policy is InternalRefCounted. Another class
   * cxxtools::RefCounted implements proper methods for the pointer, which
   * makes it straight-forward to use.
   *
   */
  template <typename ObjectType,
            template <class> class OwnershipPolicy = InternalRefCounted,
            template <class> class DestroyPolicy = DeletePolicy>
  class SmartPtr : public OwnershipPolicy<ObjectType>,
                   public DestroyPolicy<ObjectType>
  {
      ObjectType* object;
      typedef OwnershipPolicy<ObjectType> OwnershipPolicyType;
      typedef DestroyPolicy<ObjectType> DestroyPolicyType;

    public:
      SmartPtr()
        : object(0)
        {}
      SmartPtr(ObjectType* ptr)
        : object(ptr)
        { OwnershipPolicyType::link(*this, ptr); }
      SmartPtr(const SmartPtr& ptr)
        : object(ptr.object)
        { OwnershipPolicyType::link(ptr, ptr.object); }
      ~SmartPtr()
        { if (OwnershipPolicyType::unlink(object))
            destroy(object); }

      SmartPtr& operator= (const SmartPtr& ptr)
      {
        if (object != ptr.object)
        {
          if (OwnershipPolicyType::unlink(object))
            destroy(object);

          object = ptr.object;

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
  };

}

#endif // CXXTOOLS_SMARTPTR_H

