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
#ifndef cxxtools_Function_h
#define cxxtools_Function_h

#include <cxxtools/callable.h>
#include <cxxtools/connectable.h>
#include <cxxtools/slot.h>

namespace cxxtools {

template < typename R,
           typename A1 = Void,
           typename A2 = Void,
           typename A3 = Void >
class Function : public Callable<R, A1,
                                  A2,
                                  A3,
                                  Void,
                                  Void,
                                  Void,
                                  Void,
                                  Void> {
    public:
        typedef R (*FuncT)(A1, A2, A3);

        Function(FuncT func) throw()
        : _funcPtr(func) { }

        Function(const Function& f) throw()
        { this->operator=(f); }

        R operator()(A1 a1, A2 a2, A3 a3) const
        { return (*_funcPtr)(a1, a2, a3); }

        Function<R, A1, A2, A3>* clone() const
        { return new Function(*this); }

        Function& operator=(const Function& function)
        {
            _funcPtr = function._funcPtr;
            return (*this);
        }

    private:
        FuncT _funcPtr;
};


template <typename R, typename A1, typename A2, typename A3>
Function<R, A1, A2, A3> callable(R (*func)(A1, A2, A3)) throw()
{ return Function<R, A1, A2, A3>(func); }


template < typename R,
           typename A1,
           typename A2>
class Function<R, A1, A2, Void> : public Callable<R, A1,
                                                            A2,
                                                            Void,
                                                            Void,
                                                            Void,
                                                            Void,
                                                            Void,
                                                            Void> {
    public:
        typedef R (*FuncT)( A1, A2);

        Function(FuncT func) throw()
        : _funcPtr(func) { }

        Function(const Function& f) throw()
        { this->operator=(f); }

        R operator()(A1 a1, A2 a2) const
        { return (*_funcPtr)(a1, a2); }

        Function<R, A1, A2>* clone() const
        { return new Function(*this); }

        Function& operator=(const Function& function)
        {
            _funcPtr = function._funcPtr;
            return (*this);
        }

    private:
        FuncT _funcPtr;
};


template <typename R, typename A1, typename A2>
Function<R, A1, A2> callable(R (*func)(A1, A2)) throw()
{ return Function<R, A1, A2>(func); }


template < typename R,
           typename A1 >
class Function<R, A1, Void, Void> : public Callable<R, A1,
                                                            Void,
                                                            Void,
                                                            Void,
                                                            Void,
                                                            Void,
                                                            Void,
                                                            Void> {
    public:
        typedef R (*FuncT)(A1);

        Function(FuncT func) throw()
        : _funcPtr(func) { }

        Function(const Function& f) throw()
        { this->operator=(f); }

        R operator()(A1 a1) const
        { return (*_funcPtr)(a1); }

        Function<R, A1>* clone() const
        { return new Function(*this); }

        Function& operator=(const Function& function)
        {
            _funcPtr = function._funcPtr;
            return (*this);
        }

    private:
        FuncT _funcPtr;
};


template <typename R, typename A1>
Function<R, A1> callable(R (*func)(A1)) throw()
{ return Function<R, A1>(func); }


template < typename R >
class Function<R, Void, Void, Void> : public Callable<R,
                                                                  Void,
                                                                  Void,
                                                                  Void,
                                                                  Void,
                                                                  Void,
                                                                  Void,
                                                                  Void,
                                                                  Void> {
    public:
        typedef R (*FuncT)();

        Function(FuncT func) throw()
        : _funcPtr(func) { }

        Function(const Function& f) throw()
        { this->operator=(f); }

        R operator()() const
        { return this->call(); }

        R call() const
        { return (*_funcPtr)(); }

        Function<R>* clone() const
        { return new Function(*this); }

        Function& operator=(const Function& function)
        {
            _funcPtr = function._funcPtr;
            return (*this);
        }

    private:
        FuncT _funcPtr;
};


template <typename R>
Function<R> callable(R (*func)()) throw()
{ return Function<R>(func); }




template < typename R,
           typename A1 = Void,
           typename A2 = Void,
           typename A3 = Void >
class FunctionSlot : public BasicSlot<R, A1, A2, A3> {
    public:
        FunctionSlot(const Function<R, A1, A2, A3>& func)
        : _func( func )
        {}

        virtual const void* callable() const
        { return &_func; }

        Slot* clone() const
        { return new FunctionSlot(*this); }

        virtual bool opened(const Connection& c)
        { return true; }

        virtual void closed(const Connection& c)
        { }

    private:
        Function<R, A1, A2, A3> _func;
};


template <typename R>
FunctionSlot<R> slot( R (*func)() ) throw()
{ return FunctionSlot<R>( callable(func) ); }


template <typename R, typename A1>
FunctionSlot<R, A1> slot( R (*func)(A1) ) throw()
{ return FunctionSlot<R, A1>( callable(func) ); }


template <typename R, typename A1, typename A2>
FunctionSlot<R, A1, A2> slot( R (*func)(A1, A2) ) throw()
{ return FunctionSlot<R, A1, A2>( callable(func) ); }


template <typename R, typename A1, typename A2, typename A3>
FunctionSlot<R, A1, A2, A3> slot( R (*func)(A1, A2, A3) ) throw()
{ return FunctionSlot<R, A1, A2, A3>( callable(func) ); }

} // !namespace cxxtools


#endif
