/* 
 * cxxtools - general purpose C++-toolbox
 * Copyright (C) 2005 Tommi Maekitalo
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
 */

#ifndef CXXTOOLS_SMARTPTR_H
#define CXXTOOLS_SMARTPTR_H

namespace cxxtools
{
  template <typename objectType>
  class RefLinked
  {
      mutable const RefLinked* prev;
      mutable const RefLinked* next;

    protected:
      bool unlink(objectType* object)
      {
        bool ret = false;
        if (object)
        {
          if (next == this)
          {
            ret = true;
          }
          else
          {
            next->prev = prev;
            prev->next = next;
          }
          next = prev = this;
        }
        return ret;
      }

      void link(const RefLinked& ptr, objectType* object)
      {
        prev = &ptr;
        next = ptr.next;
        prev->next = this;
        next->prev = this;
      }
  };

  template <typename objectType>
  class InternalRefCounted
  {
    protected:
      bool unlink(objectType* object)
      {
        if (object)
          object->release();
        return false;
      }

      void link(const InternalRefCounted& ptr, objectType* object)
      {
        if (object)
          object->addRef();
      }

  };

  template <typename objectType>
  class ExternalRefCounted
  {
      unsigned* refs;

    protected:
      ExternalRefCounted()
        : refs(0)  { }

      bool unlink(objectType* object)
      {
        if (object && --*refs <= 0)
        {
          delete refs;
          refs = 0;
          return true;
        }
        else
          return false;
      }

      void link(const ExternalRefCounted& ptr, objectType* object)
      {
        if (object)
        {
          refs = ptr.refs;
          if (!refs)
            refs = new unsigned(0);
          ++*refs;
        }
      }

  };

  template <typename objectType>
  class DefaultDestroyPolicy
  {
    protected:
      void destroy(objectType* ptr)
      { delete ptr; }
  };

  template <typename objectType>
  class ArrayDestroyPolicy
  {
    protected:
      void destroy(objectType* ptr)
      { delete[] ptr; }
  };

  /**
   * Policy-based smart-pointer-class.
   *
   * This class works like a pointer, but the destructor deletes the held
   * object if this is the last reference. The policy specifies, how the class
   * counts the references. There are 3 policies:
   *
   *   ExternalRefCounted: allocates a reference-count
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
  template <typename objectType,
            template <class> class ownershipPolicy = InternalRefCounted,
            template <class> class destroyPolicy = DefaultDestroyPolicy>
  class SmartPtr : public ownershipPolicy<objectType>,
                   public destroyPolicy<objectType>
  {
      objectType* object;
      typedef ownershipPolicy<objectType> ownershipPolicyType;
      typedef destroyPolicy<objectType> destroyPolicyType;

    public:
      SmartPtr()
        : object(0)
        {}
      SmartPtr(objectType* ptr)
        : object(ptr)
        { ownershipPolicyType::link(*this, ptr); }
      SmartPtr(const SmartPtr& ptr)
        : object(ptr.object)
        { ownershipPolicyType::link(ptr, ptr.object); }
      ~SmartPtr()
        { if (ownershipPolicyType::unlink(object))
            destroy(object); }

      SmartPtr& operator= (const SmartPtr& ptr)
      {
        if (object != ptr.object)
        {
          if (ownershipPolicyType::unlink(object))
            destroy(object);

          object = ptr.object;

          ownershipPolicyType::link(ptr, object);
        }
        return *this;
      }

      /// The object can be dereferenced like the held object
      objectType* operator->() const              { return object; }
      /// The object can be dereferenced like the held object
      objectType& operator*() const               { return *object; }

      bool operator== (const objectType* p) const { return object == p; }
      bool operator!= (const objectType* p) const { return object != p; }
      bool operator< (const objectType* p) const  { return object < p; }
      bool operator! () const { return object == 0; }
      operator bool () const  { return object != 0; }

      objectType* getPointer()              { return object; }
      const objectType* getPointer() const  { return object; }
      operator objectType* ()               { return object; }
      operator const objectType* () const   { return object; }
  };

}

#endif // CXXTOOLS_SMARTPTR_H

