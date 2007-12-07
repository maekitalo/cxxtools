 
/***************************************************************************
 *   Copyright (C) 2004-2007 by Marc Boris Duerner                         *
 *   Copyright (C) 2005 Stephan Beal                                       *
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
#ifndef CXXTOOLS_Delegate_h
#define CXXTOOLS_Delegate_h

#include <cxxtools/void.h>
#include <cxxtools/function.h>
#include <cxxtools/method.h>
#include <cxxtools/constmethod.h>
#include <cxxtools/connectable.h>
#include <stdexcept>

namespace cxxtools {

    class DelegateBase : public Connectable
    {
        public:
            DelegateBase()
            { }

            DelegateBase(const DelegateBase& rhs)
            { DelegateBase::operator=(rhs); }

            DelegateBase& operator=(const DelegateBase& other)
            {
                _target.close();
                if( !other._target )
                    return *this;
                const Slot& slot = other._target.slot();
                _target = Connection( *this, slot.clone()  );
                return *this;
            }

            virtual bool opened(const Connection& c)
            {
                const Connectable& sender = c.sender();
                if( &sender == this )
                {
                    _target.close();
                    bool accept = Connectable::opened(c);
                    if(accept)
                        _target = c;
                    return accept;
                }
                return Connectable::opened(c);
            }

            virtual void closed(const Connection& c)
            {
                Connectable::closed(c);
            }

        protected:
            Connection _target;
    };

    /** @internal
    */
    /** @brief Single-cast signal type
        A Delegate can only be connected to one target, but forwards the return
        value of the target, when called.
    */
    template < typename R,
               typename A1 = Void,
               typename A2 = Void,
               typename A3 = Void>
    class Delegate : public DelegateBase
    {
        public:
            typedef Callable<R, A1, A2, A3> Callable;

        public:
            Delegate()
            { }

            Delegate(const Delegate& other)
            {
                DelegateBase::operator=(other);
            }

            Connection connect(const BasicSlot<R, A1, A2, A3>& slot)
            {
                return Connection(*this, slot.clone() );
            }

            inline R call(A1 a1, A2 a2, A3 a3) const
            {
                if( !_target.valid() ) {
                    throw std::logic_error("Delegate not connected" + PT_SOURCEINFO);
                }
                const Callable* cb = static_cast<const Callable*>( _target.slot().callable() );
                return cb->call(a1, a2, a3);
            }

            inline void invoke(A1 a1, A2 a2, A3 a3) const
            {
                if( !_target.valid() ) {
                    return;
                }
                const Callable* cb = static_cast<const Callable*>( _target.slot().callable() );
                cb->call(a1, a2, a3);
            }

            R operator()(A1 a1, A2 a2, A3 a3) const
            { return this->call(a1, a2, a3); }
    };

    /** @internal
    */
    template < typename R,
               typename A1,
               typename A2 >
    class Delegate<R, A1, A2, Void> : public DelegateBase
    {
        public:
            typedef Callable<R, A1, A2, Void> Callable;
        public:
            Delegate()
            { }
            Delegate(const Delegate& other)
            {
                DelegateBase::operator=(other);
            }
            Connection connect(const BasicSlot<R, A1, A2, Void>& slot)
            {
                return Connection(*this, slot.clone() );
            }
            inline R call(A1 a1, A2 a2) const
            {
                if( !_target.valid() ) {
                    throw std::logic_error("Delegate not connected" + PT_SOURCEINFO);
                }
                const Callable* cb = static_cast<const Callable*>( _target.slot().callable() );
                return cb->call(a1, a2);
            }
            inline void invoke(A1 a1, A2 a2) const
            {
                if( !_target.valid() ) {
                    return;
                }
                const Callable* cb = static_cast<const Callable*>( _target.slot().callable() );
                cb->call(a1, a2);
            }
            R operator()(A1 a1, A2 a2) const
            { return this->call(a1, a2); }
    };

    /** @internal
    */
    template < typename R,
               typename A1 >
    class Delegate<R, A1, Void, Void> : public DelegateBase
    {
        public:
            typedef Callable<R, A1, Void, Void> Callable;
        public:
            Delegate()
            { }
            Delegate(const Delegate& other)
            {
                DelegateBase::operator=(other);
            }
            Connection connect(const BasicSlot<R, A1, Void, Void>& slot)
            {
                return Connection(*this, slot.clone() );
            }
            inline R call(A1 a1) const
            {
                if( !_target.valid() ) {
                    throw std::logic_error("Delegate not connected" + PT_SOURCEINFO);
                }
                const Callable* cb = static_cast<const Callable*>( _target.slot().callable() );
                return cb->call(a1);
            }
            inline void invoke(A1 a1) const
            {
                if( !_target.valid() ) {
                    return;
                }
                const Callable* cb = static_cast<const Callable*>( _target.slot().callable() );
                cb->call(a1);
            }
            R operator()(A1 a1) const
            { return this->call(a1); }
    };

    /** @internal
    */
    template < typename R >
    class Delegate<R, Void, Void, Void> : public DelegateBase
    {
        public:
            typedef Callable<R, Void, Void, Void> Callable;
        public:
            Delegate()
            { }
            Delegate(const Delegate& other)
            {
                DelegateBase::operator=(other);
            }
            Connection connect(const BasicSlot<R, Void, Void, Void>& slot)
            {
                return Connection(*this, slot.clone() );
            }
            inline R call() const
            {
                if( !_target.valid() ) {
                    throw std::logic_error("Delegate not connected" + PT_SOURCEINFO);
                }
                const Callable* cb = static_cast<const Callable*>( _target.slot().callable() );
                return cb->call();
            }
            inline void invoke() const
            {
                if( !_target.valid() ) {
                    return;
                }
                const Callable* cb = static_cast<const Callable*>( _target.slot().callable() );
                cb->call();
            }
            R operator()() const
            { return this->call(); }
    };
    /** @internal
    */
    template < typename R,
               typename A1 = Void,
               typename A2 = Void,
               typename A3 = Void >
    class DelegateSlot : public BasicSlot<R, A1, A2, A3> {
        public:
            DelegateSlot(Delegate<R, A1, A2, A3>& delegate)
            : _method( delegate, &Delegate<R, A1, A2, A3>::call )
            {}
            BasicSlot<R, A1, A2, A3>* clone() const
            { return new DelegateSlot(*this); }
            virtual const void* callable() const
            {
                return &_method;
            }
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
            mutable ConstMethod<R, Delegate<R, A1, A2, A3>, A1, A2, A3> _method;
    };

    template < typename R,
               typename A1,
               typename A2,
               typename A3 >
    DelegateSlot<R, A1, A2, A3> slot( Delegate<R, A1, A2, A3>& delegate )
    { return DelegateSlot<R, A1, A2, A3>( delegate ); }

    /** @brief Connect a Delegate to another Delegate
    */
    template <typename R, typename A1, typename A2, typename A3>
    Connection connect(Delegate<R, A1, A2, A3>& delegate, Delegate<R, A1, A2, A3>& receiver)
    {
        return connect( delegate,  slot(receiver) );
    }

    /** @brief Connect a Delegate to a slot
    */
    template <typename R, typename A1, typename A2, typename A3>
    Connection connect(Delegate<R, A1, A2, A3>& delegate, const BasicSlot<R, A1, A2, A3>& slot)
    {
        return Connection(delegate, slot.clone() );
    }

    /** @brief Connect a Delegate to a function
    */
    template <typename R>
    Connection connect(Delegate<R>& delegate, R(*func)())
    {
        return connect( delegate, slot(func) );
    }

    template <typename R, typename A1>
    Connection connect(Delegate<R, A1>& delegate, R(*func)(A1))
    {
        return connect( delegate, slot(func) );
    }

    template <typename R, typename A1, typename A2>
    Connection connect(Delegate<R, A1, A2>& delegate, R(*func)(A1, A2))
    {
        return connect( delegate, slot(func) );
    }

    template <typename R, typename A1, typename A2, typename A3>
    Connection connect(Delegate<R, A1, A2, A3>& delegate, R(*func)(A1, A2, A3))
    {
        return connect( delegate, slot(func) );
    }

    /** @brief Connect a Delegate to a member function
    */
    template <typename R, class BaseT, class ClassT>
    Connection connect(Delegate<R>& delegate, BaseT& object, R(ClassT::*memFunc)())
    {
        return connect( delegate, slot(object, memFunc) );
    }

    template <typename R, class BaseT, class ClassT, typename A1>
    Connection connect(Delegate<R, A1>& delegate, BaseT& object, R(ClassT::*memFunc)(A1))
    {
        return connect( delegate, slot(object, memFunc) );
    }

    template <typename R, class ClassT, typename A1, typename A2>
    Connection connect(Delegate<R, A1, A2>& delegate, ClassT& object, R(ClassT::*memFunc)(A1, A2))
    {
        return connect( delegate, slot(object, memFunc) );
    }

    template <typename R, class ClassT, typename A1, typename A2, typename A3>
    Connection connect(Delegate<R, A1, A2, A3>& delegate, ClassT& object, R(ClassT::*memFunc)(A1, A2, A3))
    {
        return connect( delegate, slot(object, memFunc) );
    }
    /** @brief Connect a Delegate to a const member function
    */
    template <typename R, class ClassT>
    Connection connect(Delegate<R>& delegate, ClassT& object, R(ClassT::*memFunc)() const)
    {
        return connect( delegate, slot(object, memFunc) );
    }

    template <typename R, class ClassT, typename A1>
    Connection connect(Delegate<R, A1>& delegate, ClassT& object, R(ClassT::*memFunc)(A1) const)
    {
        return connect( delegate, slot(&object, memFunc) );
    }

    template <typename R, class ClassT, typename A1, typename A2>
    Connection connect(Delegate<R, A1, A2>& delegate, ClassT& object, R(ClassT::*memFunc)(A1, A2) const)
    {
        return connect( delegate, slot(&object, memFunc) );
    }

    template <typename R, class ClassT, typename A1, typename A2, typename A3>
    Connection connect(Delegate<R, A1, A2, A3>& delegate, ClassT& object, R(ClassT::*memFunc)(A1, A2, A3) const)
    {
        return connect( delegate, slot(&object, memFunc) );
    }
} // !namespace cxxtools
#endif
