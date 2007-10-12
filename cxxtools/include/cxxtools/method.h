/***************************************************************************
 *   Copyright (C) 2005 by Dr. Marc Boris Duerner                          *
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

#ifndef cxxtools_Method_h
#define cxxtools_Method_h

#include <cxxtools/callable.h>
#include <cxxtools/connectable.h>
#include <cxxtools/slot.h>

namespace cxxtools {

template < typename R,
           class C,
           typename A1 = Void,
           typename A2 = Void,
           typename A3 = Void,
           typename A4 = Void,
           typename A5 = Void,
           typename A6 = Void,
           typename A7 = Void,
           typename A8 = Void>
class Method : public Callable<R, A1, A2, A3, A4, A5, A6, A7, A8> {
    public:
        typedef C ClassT;
        typedef R (C::*MemFuncT)(A1, A2, A3, A4, A5, A6, A7, A8);
        typedef R (C::*ConstMemFuncT)(A1, A2, A3, A4, A5, A6, A7, A8) const;

        explicit Method(C& object, MemFuncT ptr) throw()
        : _object(&object), _memFunc(ptr)
        { }

        Method(const Method& method) throw()
        :  Callable<R, A1, A2, A3, A4, A5, A6, A7, A8>()
        { this->operator=(method); }

        C& object()
        { return *_object;}

        const C& object() const
        { return *_object;}

        const MemFuncT& method() const
        { return _memFunc;}

        inline R operator()(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8) const
        { return (_object->*_memFunc)(a1, a2, a3, a4, a5, a6, a7, a8); }

        Method<R, C, A1, A2, A3, A4, A5, A6, A7, A8>* clone() const
        { return new Method(*this); }

    private:
        C* _object;
        MemFuncT _memFunc;
};


template <typename R, class ClassT, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8>
Method<R, ClassT, A1, A2, A3, A4, A5, A6, A7, A8> callable( ClassT& obj, R (ClassT::*ptr)(A1, A2, A3, A4, A5, A6, A7, A8) ) throw()
{ return Method<R, ClassT, A1, A2, A3, A4, A5, A6, A7, A8>(obj, ptr); }


template < typename R,
           class C,
           typename A1,
           typename A2,
           typename A3,
           typename A4,
           typename A5 >
class Method<R, C,
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
        typedef C ClassT;
        typedef R (C::*MemFuncT)(A1, A2, A3, A4, A5);
        typedef R (C::*ConstMemFuncT)(A1, A2, A3, A4, A5) const;

        explicit Method(C& object, MemFuncT ptr) throw()
        : _object(&object), _memFunc(ptr)
        { }

        Method(const Method& method) throw()
        : Callable<R, A1, A2, A3, A4, A5>()
        { this->operator=(method); }

        C& object()
        { return *_object;}

        const C& object() const
        { return *_object;}

        const MemFuncT& method() const
        { return _memFunc;}

        inline R operator()(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5) const
        { return (_object->*_memFunc)(a1, a2, a3, a4, a5); }

        Method<R, C, A1, A2, A3, A4, A5>* clone() const
        { return new Method(*this); }

    private:
        C* _object;
        MemFuncT _memFunc;
};


template <typename R, class ClassT, typename A1, typename A2, typename A3, typename A4, typename A5>
Method<R, ClassT, A1, A2, A3, A4, A5> callable( ClassT& obj, R (ClassT::*ptr)(A1, A2, A3, A4, A5) ) throw()
{ return Method<R, ClassT, A1, A2, A3, A4, A5>(obj, ptr); }


template < typename R,
           class C,
           typename A1,
           typename A2,
           typename A3,
           typename A4 >
class Method<R, C,
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
        typedef C ClassT;
        typedef R (C::*MemFuncT)(A1, A2, A3, A4);
        typedef R (C::*ConstMemFuncT)(A1, A2, A3, A4) const;

        explicit Method(C& object, MemFuncT ptr) throw()
        : _object(&object), _memFunc(ptr)
        { }

        Method(const Method& method) throw()
        : Callable<R, A1, A2, A3, A4>()
        { this->operator=(method); }

        C& object()
        { return *_object;}

        const C& object() const
        { return *_object;}

        const MemFuncT& method() const
        { return _memFunc;}

        inline R operator()(A1 a1, A2 a2, A3 a3, A4 a4) const
        { return (_object->*_memFunc)(a1, a2, a3, a4); }

        Method<R, C, A1, A2, A3, A4>* clone() const
        { return new Method(*this); }

    private:
        C* _object;
        MemFuncT _memFunc;
};


template <typename R, class ClassT, typename A1, typename A2, typename A3, typename A4>
Method<R, ClassT, A1, A2, A3, A4> callable( ClassT& obj, R (ClassT::*ptr)(A1, A2, A3, A4) ) throw()
{ return Method<R, ClassT, A1, A2, A3, A4>(obj, ptr); }


template < typename R,
           class C,
           typename A1,
           typename A2,
           typename A3>
class Method<R, C,
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
        typedef C ClassT;
        typedef R (C::*MemFuncT)(A1, A2, A3);
        typedef R (C::*ConstMemFuncT)(A1, A2, A3) const;

        explicit Method(C& object, MemFuncT ptr) throw()
        : _object(&object), _memFunc(ptr)
        { }

        Method(const Method& method) throw()
        : Callable<R, A1, A2, A3>()
        { this->operator=(method); }

        C& object()
        { return *_object;}

        const C& object() const
        { return *_object;}

        const MemFuncT& method() const
        { return _memFunc;}

        inline R operator()(A1 a1, A2 a2, A3 a3) const
        { return (_object->*_memFunc)(a1, a2, a3); }

        Method<R, C, A1, A2, A3>* clone() const
        { return new Method(*this); }

        Method& operator=(const Method& method)
        {
            _object = method._object;
            _memFunc = method._memFunc;
            return (*this);
        }

    private:
        C* _object;
        MemFuncT _memFunc;
};


template <typename R, class ClassT, typename A1, typename A2, typename A3>
Method<R, ClassT, A1, A2, A3> callable( ClassT& obj, R (ClassT::*ptr)(A1, A2, A3) ) throw()
{ return Method<R, ClassT, A1, A2, A3>(obj, ptr); }




template < typename R,
           class C,
           typename A1,
           typename A2 >
class Method<R, C,
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
        typedef C ClassT;
        typedef R (C::*MemFuncT)(A1, A2);
        typedef R (C::*ConstMemFuncT)(A1, A2) const;

        explicit Method(ClassT& object, MemFuncT ptr) throw()
        : _object(&object), _memFunc(ptr)
        { }

        Method(const Method& method) throw()
        : Callable<R, A1, A2, Void>()
        { this->operator=(method); }

        ClassT& object()
        { return *_object;}

        const ClassT& object() const
        { return *_object;}

        const MemFuncT& method() const
        { return _memFunc;}

        inline R operator()(A1 a1, A2 a2) const
        { return (_object->*_memFunc)(a1, a2); }

        Method<R, ClassT, A1, A2>* clone() const
        { return new Method(*this); }

        Method& operator=(const Method& method)
        {
            _object = method._object;
            _memFunc = method._memFunc;
            return (*this);
        }

    private:
        ClassT* _object;
        MemFuncT _memFunc;
};


template <typename R, class ClassT, typename A1, typename A2>
Method<R, ClassT, A1, A2> callable( ClassT& obj, R (ClassT::*ptr)(A1, A2) ) throw()
{ return Method<R, ClassT, A1, A2>(obj, ptr); }


template < typename R,
           class C,
           typename A1 >
class Method<R, C,
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
        typedef C ClassT;
        typedef R (C::*MemFuncT)(A1);
        typedef R (C::*ConstMemFuncT)(A1) const;

        explicit Method(ClassT& object, MemFuncT ptr) throw()
        : _object(&object), _memFunc(ptr)
        { }

        Method(const Method& method) throw()
        : Callable<R, A1, Void, Void>()
        { this->operator=(method); }

        ClassT& object()
        { return *_object;}

        const ClassT& object() const
        { return *_object;}

        const MemFuncT& method() const
        { return _memFunc;}

        inline R operator()(A1 a1) const
        { return (_object->*_memFunc)(a1); }

        Method<R, ClassT, A1>* clone() const
        { return new Method(*this); }

        Method& operator=(const Method& method)
        {
            _object = method._object;
            _memFunc = method._memFunc;
            return (*this);
        }

    private:
        ClassT* _object;
        MemFuncT _memFunc;
};


template <typename R, class BaseT, class ClassT, typename A1>
Method<R,ClassT, A1> callable( ClassT& obj, R (BaseT::*ptr)(A1) ) throw()
{ return Method<R, ClassT, A1>(obj, ptr); }


template < typename R,
           class C >
class Method<R, C,
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
                                     Void>
{
    public:
        typedef C ClassT;
        typedef R (C::*MemFuncT)();
        typedef R (C::*ConstMemFuncT)() const;

        explicit Method(ClassT& object, MemFuncT ptr) throw()
        : _object(&object), _memFunc(ptr)
        { }

        Method(const Method& method) throw()
        { this->operator=(method); }

        ClassT& object()
        { return *_object;}

        const ClassT& object() const
        { return *_object;}

        const MemFuncT& method() const
        { return _memFunc;}

        inline R operator()() const
        { return (_object->*_memFunc)(); }

        Method<R, ClassT>* clone() const
        { return new Method(*this); }

        Method& operator=(const Method& method)
        {
            _object = method._object;
            _memFunc = method._memFunc;
            return (*this);
        }

    private:
        ClassT* _object;
        MemFuncT _memFunc;
};


template <typename R, class BaseT, class ClassT>
Method<R,ClassT> callable( ClassT& obj, R (BaseT::*ptr)() ) throw()
{ return Method<R, ClassT>(obj, ptr); }



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
class MethodSlot : public BasicSlot<R, A1, A2, A3, A4, A5, A6, A7, A8> {
    public:
        MethodSlot(const Method<R, C, A1, A2, A3, A4, A5, A6, A7, A8>& method)
        : _method(method)
        {}

        Slot* clone() const
        {
            return new MethodSlot(*this);
        }

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
        Method<R, C, A1, A2, A3, A4, A5, A6, A7, A8> _method;
};


template <typename R, class BaseT, class ClassT>
MethodSlot<R, ClassT> slot( ClassT& obj, R (BaseT::*memFunc)() ) throw()
{ return MethodSlot<R, ClassT>( callable(obj, memFunc) ); }


template <typename R, class BaseT, class ClassT, typename A1>
MethodSlot<R, ClassT, A1> slot( ClassT& obj, R (BaseT::*method)(A1) ) throw()
{ return MethodSlot<R, ClassT, A1>( callable(obj, method) ); }


template <typename R, class BaseT, class ClassT, typename A1, typename A2>
MethodSlot<R, ClassT, A1, A2> slot( ClassT& obj, R (BaseT::*method)(A1, A2) ) throw()
{ return MethodSlot<R, ClassT, A1, A2>( callable(obj, method) ); }


template <typename R, class BaseT, class ClassT, typename A1, typename A2, typename A3>
MethodSlot<R, ClassT, A1, A2, A3> slot( ClassT& obj, R (BaseT::*method)(A1, A2, A3) ) throw()
{ return MethodSlot<R, ClassT, A1, A2, A3>( callable(obj, method) ); }


template <typename R, class BaseT, class ClassT, typename A1, typename A2, typename A3, typename A4>
MethodSlot<R, ClassT, A1, A2, A3, A4> slot( ClassT& obj, R (BaseT::*method)(A1, A2, A3, A4) ) throw()
{ return MethodSlot<R, ClassT, A1, A2, A3, A4>( callable(obj, method) ); }


template <typename R, class BaseT, class ClassT, typename A1, typename A2, typename A3, typename A4, typename A5>
MethodSlot<R, ClassT, A1, A2, A3, A4, A5> slot( ClassT& obj, R (BaseT::*method)(A1, A2, A3, A4, A5) ) throw()
{ return MethodSlot<R, ClassT, A1, A2, A3, A4, A5>( callable(obj, method) ); }

template <typename R, class BaseT, class ClassT, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8>
MethodSlot<R, ClassT, A1, A2, A3, A4, A5, A6, A7, A8> slot( ClassT& obj, R (BaseT::*method)(A1, A2, A3, A4, A5, A6, A7, A8) ) throw()
{ return MethodSlot<R, ClassT, A1, A2, A3, A4, A5, A6, A7, A8>( callable(obj, method) ); }

} // !namespace cxxtools

#endif
