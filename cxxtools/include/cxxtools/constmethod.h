/***************************************************************************
 *   Copyright (C) 2005-2007 by Dr. Marc Boris Duerner                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this program; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef cxxtools_ConstMethod_h
#define cxxtools_ConstMethod_h

#include <cxxtools/callable.h>
#include <cxxtools/connectable.h>
#include <cxxtools/slot.h>

namespace cxxtools {

template < typename R,
           class Object,
           typename A1 = Void,
           typename A2 = Void,
           typename A3 = Void,
           typename A4 = Void,
           typename A5 = Void,
           typename A6 = Void,
           typename A7 = Void,
           typename A8 = Void >
class ConstMethod : public Callable<R, A1, A2, A3, A4, A5, A6, A7, A8> {
    public:
        typedef Object ObjectT;
        typedef R (Object::*MethodT)(A1, A2, A3, A4, A5, A6, A7, A8) const;

        ConstMethod(Object& object, MethodT ptr) throw()
        : _object(&object), _method(ptr)
        { }

        ConstMethod(const ConstMethod& method) throw()
        : Callable<R, A1, A2, A3, A4, A5, A6, A7, A8>()
        { this->operator=(method); }

        Object& object()
        { return *_object;}

        const Object& object() const
        { return *_object;}

        R operator()(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8) const
        { return (_object->*_method)(a1, a2, a3, a4, a5, a6, a7, a8); }

        ConstMethod<R, Object, A1, A2, A3, A4, A5, A6, A7, A8>* clone() const
        { return new ConstMethod(*this); }

    private:
        Object* _object;
        MethodT _method;
};


template <typename R, class Object, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8>
ConstMethod<R, Object, A1, A2, A3, A4, A5, A6, A7, A8> callable( Object& obj, R (Object::*ptr)(A1, A2, A3, A4, A5, A6, A7, A8) const ) throw()
{ return ConstMethod<R, Object, A1, A2, A3, A4, A5, A6, A7, A8>(obj, ptr); }


template < typename R,
           class Object,
           typename A1,
           typename A2,
           typename A3,
           typename A4,
           typename A5 >
class ConstMethod<R, Object,
                  A1,
                  A2,
                  A3,
                  A4,
                  A5,
                  Void,
                  Void,
                  Void> : public Callable<R,
                                              A1,
                                              A2,
                                              A3,
                                              A4,
                                              A5,
                                              Void,
                                              Void,
                                              Void> {
    public:
        typedef Object ObjectT;
        typedef R (Object::*MethodT)(A1, A2, A3, A4, A5) const;

        ConstMethod(Object& object, MethodT ptr) throw()
        : _object(&object), _method(ptr)
        { }

        ConstMethod(const ConstMethod& method) throw()
        : Callable<R, A1, A2, A3, A4, A5>()
        { this->operator=(method); }

        Object& object()
        { return *_object;}

        const Object& object() const
        { return *_object;}

        R operator()(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5) const
        { return (_object->*_method)(a1, a2, a3, a4, a5); }

        ConstMethod<R, Object, A1, A2, A3, A4, A5>* clone() const
        { return new ConstMethod(*this); }

    private:
        Object* _object;
        MethodT _method;
};


template <typename R, class Object, typename A1, typename A2, typename A3, typename A4, typename A5>
ConstMethod<R, Object, A1, A2, A3, A4, A5> callable( Object& obj, R (Object::*ptr)(A1, A2, A3, A4, A5) const ) throw()
{ return ConstMethod<R, Object, A1, A2, A3, A4, A5>(obj, ptr); }


template < typename R,
           class Object,
           typename A1,
           typename A2,
           typename A3,
           typename A4 >
class ConstMethod<R, Object,
                  A1,
                  A2,
                  A3,
                  A4,
                  Void,
                  Void,
                  Void,
                  Void> : public Callable<R,
                                              A1,
                                              A2,
                                              A3,
                                              A4,
                                              Void,
                                              Void,
                                              Void,
                                              Void> {
    public:
        typedef Object ObjectT;
        typedef R (Object::*MethodT)(A1, A2, A3, A4) const;

        ConstMethod(Object& object, MethodT ptr) throw()
        : _object(&object), _method(ptr)
        { }

        ConstMethod(const ConstMethod& method) throw()
        : Callable<R, A1, A2, A3>()
        { this->operator=(method); }

        Object& object()
        { return *_object;}

        const Object& object() const
        { return *_object;}

        R operator()(A1 a1, A2 a2, A3 a3, A4 a4) const
        { return (_object->*_method)(a1, a2, a3, a4); }

        ConstMethod<R, Object, A1, A2, A3, A4>* clone() const
        { return new ConstMethod(*this); }

    private:
        Object* _object;
        MethodT _method;
};


template <typename R, class Object, typename A1, typename A2, typename A3, typename A4>
ConstMethod<R, Object, A1, A2, A3, A4> callable( Object& obj, R (Object::*ptr)(A1, A2, A3, A4) const ) throw()
{ return ConstMethod<R, Object, A1, A2, A3, A4>(obj, ptr); }


template < typename R,
           class Object,
           typename A1,
           typename A2,
           typename A3 >
class ConstMethod<R, Object,
                  A1,
                  A2,
                  A3,
                  Void,
                  Void,
                  Void,
                  Void,
                  Void> : public Callable<R,
                                              A1,
                                              A2,
                                              A3,
                                              Void,
                                              Void,
                                              Void,
                                              Void,
                                              Void> {
    public:
        typedef Object ObjectT;
        typedef R (Object::*MethodT)(A1, A2, A3) const;

        ConstMethod(Object& object, MethodT ptr) throw()
        : _object(&object), _method(ptr)
        { }

        ConstMethod(const ConstMethod& method) throw()
        : Callable<R, A1, A2, A3>()
        { this->operator=(method); }

        Object& object()
        { return *_object;}

        const Object& object() const
        { return *_object;}

        R operator()(A1 a1, A2 a2, A3 a3) const
        { return (_object->*_method)(a1, a2, a3); }

        ConstMethod<R, Object, A1, A2, A3>* clone() const
        { return new ConstMethod(*this); }

    private:
        Object* _object;
        MethodT _method;
};


template <typename R, class Object, typename A1, typename A2, typename A3>
ConstMethod<R, Object, A1, A2, A3> callable( Object& obj, R (Object::*ptr)(A1, A2, A3) const ) throw()
{ return ConstMethod<R, Object, A1, A2, A3>(obj, ptr); }


template < typename R,
           class Object,
           typename A1,
           typename A2 >
class ConstMethod<R, Object,
                  A1,
                  A2,
                  Void,
                  Void,
                  Void,
                  Void,
                  Void,
                  Void> : public Callable<R,
                                              A1,
                                              A2,
                                              Void,
                                              Void,
                                              Void,
                                              Void,
                                              Void,
                                              Void> {
    public:
        typedef Object ObjectT;
        typedef R (Object::*MethodT)(A1, A2) const;

        ConstMethod(Object& object, MethodT ptr) throw()
        : _object(&object), _method(ptr)
        { }

        ConstMethod(const ConstMethod& method) throw()
        : Callable<R, A1, A2, Void>()
        { this->operator=(method); }

        Object& object()
        { return *_object;}

        const Object& object() const
        { return *_object;}

        R operator()(A1 a1, A2 a2) const
        { return (_object->*_method)(a1, a2); }

        ConstMethod<R, Object, A1, A2>* clone() const
        { return new ConstMethod(*this); }

    private:
        Object* _object;
        MethodT _method;
};


template <typename R, class Object, typename A1, typename A2>
ConstMethod<R, Object, A1, A2> callable( Object& obj, R (Object::*ptr)(A1, A2) const ) throw()
{ return ConstMethod<R, Object, A1, A2>(obj, ptr); }


template < typename R,
           class Object,
           typename A1 >
class ConstMethod<R, Object,
                  A1,
                  Void,
                  Void,
                  Void,
                  Void,
                  Void,
                  Void,
                  Void> : public Callable<R,
                                              A1,
                                              Void,
                                              Void,
                                              Void,
                                              Void,
                                              Void,
                                              Void,
                                              Void> {
    public:
        typedef Object ObjectT;
        typedef R (Object::*MethodT)(A1) const;

        ConstMethod(Object& object, MethodT ptr) throw()
        : _object(&object), _method(ptr)
        { }

        Object& object()
        { return *_object;}

        const Object& object() const
        { return *_object;}

        ConstMethod(const ConstMethod& method) throw()
        : Callable<R, A1, Void, Void>()
        { this->operator=(method); }

        R operator()(A1 a1) const
        { return (_object->*_method)(a1); }

        ConstMethod<R, Object, A1>* clone() const
        { return new ConstMethod(*this); }

    private:
        Object* _object;
        MethodT _method;
};


template <typename R, class Object, typename A1>
ConstMethod<R,Object, A1> callable( Object& obj, R (Object::*ptr)(A1) const ) throw()
{ return ConstMethod<R, Object, A1>(obj, ptr); }


template < typename R,
           class C >
class ConstMethod<R, C,
                  Void,
                  Void,
                  Void,
                  Void,
                  Void,
                  Void,
                  Void,
                  Void> : public Callable<R,
                                              Void,
                                              Void,
                                              Void,
                                              Void,
                                              Void,
                                              Void,
                                              Void,
                                              Void> {
    public:
        typedef C ClassT;
        typedef R (C::*MemFuncT)() const;

        ConstMethod(ClassT& object, MemFuncT memFunc) throw()
        : _object(&object), _memFunc(memFunc)
        { }

        ConstMethod(const ConstMethod& method) throw()
        { this->operator=(method); }

        ClassT& object()
        { return *_object; }

        const ClassT& object() const
        { return _object;}

        const MemFuncT& method() const
        { return _memFunc;}

        inline R operator()() const
        { return this->call(); }

        inline R call() const
        { return (_object->*_memFunc)(); }

        ConstMethod* clone() const
        { return new ConstMethod(*this); }

    private:
        ClassT* _object;
        MemFuncT _memFunc;
};


template <typename R, class Object>
ConstMethod<R,Object> callable( Object& obj, R (Object::*ptr)() const ) throw()
{ return ConstMethod<R, Object>(obj, ptr); }


template < typename R,
           class C,
           typename A1 = Void,
           typename A2 = Void,
           typename A3 = Void,
           typename A4 = Void,
           typename A5 = Void,
           typename A6 = Void,
           typename A7 = Void,
           typename A8 = Void
         >
class ConstMethodSlot : public BasicSlot<R, A1, A2, A3, A4, A5, A6, A7, A8> {
    public:
        ConstMethodSlot(const ConstMethod<R, C, A1, A2, A3, A4, A5, A6, A7, A8>& method)
        : _method( method )
        {}

        Slot* clone() const
        { return new ConstMethodSlot(*this); }

        virtual const void* callable() const
        { return &_method; }

        virtual bool opened(const Connection& c)
        {
            Connectable& connectable = _method.object();
            return connectable.opened(c);
        }

        virtual void closed(const Connection& c) 
        {
            Connectable& connectable = _method.object();
            connectable.closed(c);
        }

    private:
        ConstMethod<R, C, A1, A2, A3, A4, A5, A6, A7, A8> _method;
};


template <typename R, class BaseT, class ClassT>
ConstMethodSlot<R, ClassT> slot( ClassT& obj, R (BaseT::*memFunc)() const ) throw()
{ return ConstMethodSlot<R, ClassT>( callable(obj, memFunc) ); }


template <typename R, class BaseT, class ClassT, typename A1>
ConstMethodSlot<R, ClassT, A1> slot( ClassT& obj, R (BaseT::*method)(A1) const ) throw()
{ return ConstMethodSlot<R, ClassT, A1>( callable(obj, method) ); }


template <typename R, class BaseT, class ClassT, typename A1, typename A2>
ConstMethodSlot<R, ClassT, A1, A2> slot( ClassT& obj, R (BaseT::*method)(A1, A2) const ) throw()
{ return ConstMethodSlot<R, ClassT, A1, A2>( callable(obj, method) ); }


template <typename R, class BaseT, class ClassT, typename A1, typename A2, typename A3>
ConstMethodSlot<R, ClassT, A1, A2, A3> slot( ClassT& obj, R (BaseT::*method)(A1, A2, A3) const ) throw()
{ return ConstMethodSlot<R, ClassT, A1, A2, A3>( callable(obj, method) ); }


template <typename R, class BaseT, class ClassT, typename A1, typename A2, typename A3, typename A4>
ConstMethodSlot<R, ClassT, A1, A2, A3, A4> slot( ClassT& obj, R (BaseT::*method)(A1, A2, A3, A4) const ) throw()
{ return ConstMethodSlot<R, ClassT, A1, A2, A3, A4>( callable(obj, method) ); }


template <typename R, class BaseT, class ClassT, typename A1, typename A2, typename A3, typename A4, typename A5>
ConstMethodSlot<R, ClassT, A1, A2, A3, A4, A5> slot( ClassT& obj, R (BaseT::*method)(A1, A2, A3, A4, A5) const ) throw()
{ return ConstMethodSlot<R, ClassT, A1, A2, A3, A4, A5>( callable(obj, method) ); }

template <typename R, class BaseT, class ClassT, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8>
ConstMethodSlot<R, ClassT, A1, A2, A3, A4, A5, A6, A7, A8> slot( ClassT& obj, R (BaseT::*method)(A1, A2, A3, A4, A5, A6, A7, A8) const ) throw()
{ return ConstMethodSlot<R, ClassT, A1, A2, A3, A4, A5, A6, A7, A8>( callable(obj, method) ); }

} // !namespace cxxtools

#endif
