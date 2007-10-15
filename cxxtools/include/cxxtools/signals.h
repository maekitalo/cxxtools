/***************************************************************************
 *   Copyright (C) 2004-2006 by Dr. Marc Boris Duerner                     *
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
#ifndef cxxtools_Signal_h
#define cxxtools_Signal_h

#include <cxxtools/void.h>
#include <cxxtools/function.h>
#include <cxxtools/method.h>
#include <cxxtools/constmethod.h>
#include <cxxtools/connectable.h>

#include <list>
#include <algorithm>

namespace cxxtools {

    class SignalBase : public Connectable
    {
        public:
            struct Sentry
            {
                Sentry(const SignalBase* signal)
                : _signal(signal)
                {
                    _signal->_sentry = this;
                    _signal->_sending = true;
                    _signal->_dirty = false;
                }

                ~Sentry()
                {
                    if( _signal )
                        this->detach();
                }

                void detach()
                {
                    _signal->_sending = false;

                    if( _signal->_dirty == false )
                    {
                        _signal->_sentry = 0;
                        _signal = 0;
                        return;
                    }

                    std::list<Connection>::iterator it = _signal->_connections.begin();
                    while( it != _signal->_connections.end() )
                    {
                        if( it->valid() )
                        {
                            ++it;
                        }
                        else
                        {
                            it = _signal->_connections.erase(it);
                        }
                    }

                    _signal->_dirty = false;
                    _signal->_sentry = 0;
                    _signal = 0;
                }

                bool operator!() const
                { return _signal == 0; }

                const SignalBase* _signal;
            };

            SignalBase()
            : _sentry(0)
            , _sending(false)
            { }

            ~SignalBase()
            {
                if(_sentry)
                {
                    _sentry->detach();
                }
            }

            SignalBase& operator=(const SignalBase& other)
            {
                Connectable::operator=(other);
                return *this;
            }

            virtual bool opened(const Connection& c)
            {
                return Connectable::opened(c);
            }

            virtual void closed(const Connection& c)
            {
                // if the signal is currently calling its slots, do not
                // remove the connection now, but only set the cleanup flag
                // Any invalid connection objects will be removed after
                // the signal has finished calling its slots by the Sentry.
                if( _sending )
                {
                    _dirty = true;
                }
                else
                {
                    Connectable::closed(c);
                }
            }

        private:
            mutable Sentry* _sentry;
            mutable bool _sending;
            mutable bool _dirty;
    };


    /** @brief Multicast Signal

        A Signal can be connected to multiple targets. The return
        value of the target is ignored, when the signal is sent.
    */
    template <typename A1 = Void, typename A2 = Void, typename A3 = Void>
    class Signal : public SignalBase {
        public:
            typedef Invokable<A1, A2, A3> Invokable;

        public:
            Signal()
            { }

            Signal(const Signal& signal)
            {
                Signal::operator=(signal);
            }

            template <typename R>
            Connection connect(const BasicSlot<R, A1, A2, A3>& slot)
            {
                return Connection(*this, slot.clone() );
            }

            inline void send(A1 a1, A2 a2, A3 a3) const
            {
                // The sentry will set the Signal to the sending state and
                // reset it to not-sending upon destruction. In the sending
                // state, removing connection will leave invalid connections
                // in the connection list to keep the iterator valid, but mark
                // the Signal dirty. If the Signal is dirty, all invalid
                // connections will be removed by the Sentry when it destructs..
                Sentry sentry(this);

                std::list<Connection>::const_iterator it = Connectable::connections().begin();
                std::list<Connection>::const_iterator end = Connectable::connections().end();

                for(; it != end; ++it)
                {
                    if( false == it->valid() || &( it->sender() ) != this  )
                        continue;

                    // The following scenarios must be considered when the
                    // slot is called:
                    // - The slot might get deleted and thus disconnected from
                    //   this signal
                    // - The slot might delete this signal and we must end
                    //   calling any slots immediately
                    // - A new Connection might get added to this Signal in
                    //   the slot
                    const Invokable* invokable = static_cast<const Invokable*>( it->slot().callable() );
                    invokable->invoke(a1, a2, a3);

                    // if this signal gets deleted by the slot, the Sentry
                    // will be detached. In this case we bail out immediately
                    if( !sentry )
                        return;
                }
            }

            inline void operator()(A1 a1, A2 a2, A3 a3) const
            { this->send(a1, a2, a3); }
    };


    /** @internal
    */
    template < typename A1,
               typename A2 >
    class Signal<A1, A2, Void> : public SignalBase {
        public:
            typedef Invokable<A1, A2, Void> Invokable;

        public:
            Signal()
            { }

            Signal(const Signal& signal)
            {
                Signal::operator=(signal);
            }

            template <typename R>
            Connection connect(const BasicSlot<R, A1, A2, Void>& slot)
            {
                return Connection(*this, slot.clone() );
            }


            inline void send(A1 a1, A2 a2) const
            {
                SignalBase::Sentry sentry(this);

                std::list<Connection>::const_iterator it = Connectable::connections().begin();
                for(; it != _connections.end(); ++it)
                {
                    if( &( it->sender() ) != this || false == it->valid() )
                        continue;

                    const Invokable* invokable = static_cast<const Invokable*>( it->slot().callable() );
                    invokable->invoke(a1, a2);

                    if( !sentry )
                        return;
                }
            }

            inline void operator()(A1 a1, A2 a2) const
            { this->send(a1, a2); }
    };


    /** @internal
    */
    template < typename A1 >
    class Signal<A1, Void, Void> : public SignalBase {
        public:
            typedef Invokable<A1, Void, Void> Invokable;

        public:
            Signal()
            { }

            Signal(const Signal& signal)
            {
                Signal::operator=(signal);
            }

            template <typename R>
            Connection connect(const BasicSlot<R, A1, Void, Void>& slot)
            {
                return Connection(*this, slot.clone() );
            }


            inline void send(A1 a1) const
            {
                SignalBase::Sentry sentry(this);

                std::list<Connection>::const_iterator it = Connectable::connections().begin();
                for(; it != _connections.end(); ++it)
                {
                    if( &( it->sender() ) != this || false == it->valid() )
                        continue;

                    const Invokable* invokable = static_cast<const Invokable*>( it->slot().callable() );
                    invokable->invoke(a1);

                    if( !sentry )
                        return;
                }
            }

            inline void operator()(A1 a1) const
            { this->send(a1); }
    };


    /** @internal
    */
    template <>
    class Signal<Void, Void, Void> : public SignalBase {
        public:
            typedef Invokable<Void, Void, Void> Invokable;

        public:
            Signal()
            { }

            Signal(const Signal& signal)
            {
                Signal::operator=(signal);
            }

            template <typename R>
            Connection connect(const BasicSlot<R, Void, Void, Void>& slot)
            {
                return Connection(*this, slot.clone() );
            }


            inline void send() const
            {
                Sentry sentry(this);

                std::list<Connection>::const_iterator it = Connectable::connections().begin();
                std::list<Connection>::const_iterator end = Connectable::connections().end();

                for(; it != end; ++it)
                {
                    if( false == it->valid() || &( it->sender() ) != this  )
                        continue;

                    const Invokable* invokable = static_cast<const Invokable*>( it->slot().callable() );
                    invokable->invoke();

                    if( !sentry )
                        return;
                }
            }

            inline void operator()() const
            { this->send(); }
    };


    template < typename A1 = Void,
                typename A2 = Void,
                typename A3 = Void >
    class SignalSlot : public BasicSlot<void, A1, A2, A3> {
        public:
            SignalSlot(Signal<A1, A2, A3>& signal)
            : _method( signal, &Signal<A1, A2, A3>::send )
            {}

            BasicSlot<void, A1, A2, A3>* clone() const
            { return new SignalSlot(*this); }

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
            mutable ConstMethod<void, Signal<A1, A2, A3>, A1, A2, A3> _method;
    };


    template < typename A1,
               typename A2,
               typename A3 >
    SignalSlot<A1, A2, A3> slot( Signal<A1, A2, A3>& signal )
    { return SignalSlot<A1, A2, A3>( signal ); }


    template <typename R, typename A1, typename A2, typename A3>
    Connection connect(Signal<A1, A2, A3>& signal, const BasicSlot<R, A1, A2, A3>& slot)
    {
        return Connection(signal, slot.clone() );
    }


    //! Connects a Signal to a member function.
    template <typename R>
    Connection connect(Signal<>& signal, R(*func)())
    {
        return connect( signal, slot(func) );
    }


    template <typename R, typename A1>
    Connection connect(Signal<A1>& signal, R(*func)(A1))
    {
        return connect( signal, slot(func) );
    }


    template <typename R, typename A1, typename A2>
    Connection connect(Signal<A1, A2>& signal, R(*func)(A1, A2))
    {
        return connect( signal, slot(func) );
    }


    template <typename R, typename A1, typename A2, typename A3>
    Connection connect(Signal<A1, A2, A3>& signal, R(*func)(A1, A2, A3))
    {
        return connect( signal, slot(func) );
    }


    //! Connects a Signal to a member function.
    template <typename R, class BaseT, class ClassT>
    Connection connect(Signal<>& signal, BaseT& object, R(ClassT::*memFunc)())
    {
        return connect( signal, slot(object, memFunc) );
    }


    template <typename R, class BaseT, class ClassT, typename A1>
    Connection connect(Signal<A1>& signal, BaseT& object, R(ClassT::*memFunc)(A1))
    {
        return signal.connect( slot(object, memFunc) );
    }


    template <typename R, class ClassT, typename A1, typename A2>
    Connection connect(Signal<A1, A2>& signal, ClassT& object, R(ClassT::*memFunc)(A1, A2))
    {
        return connect( signal, slot(object, memFunc) );
    }


    template <typename R, class ClassT, typename A1, typename A2, typename A3>
    Connection connect(Signal<A1, A2, A3>& signal, ClassT& object, R(ClassT::*memFunc)(A1, A2, A3))
    {
        return connect( signal, slot(object, memFunc) );
    }


    //! Connects a Signal to a const member function.
    template <typename R, class ClassT>
    Connection connect(Signal<>& signal, ClassT& object, R(ClassT::*memFunc)() const)
    {
        return connect( signal, slot(object, memFunc) );
    }


    template <typename R, class ClassT, typename A1>
    Connection connect(Signal<A1>& signal, ClassT& object, R(ClassT::*memFunc)(A1) const)
    {
        return connect( signal, slot(object, memFunc) );
    }


    template <typename R, class ClassT, typename A1, typename A2>
    Connection connect(Signal<A1, A2>& signal, ClassT& object, R(ClassT::*memFunc)(A1, A2) const)
    {
        return connect( signal, slot(object, memFunc) );
    }


    template <typename R, class ClassT, typename A1, typename A2, typename A3>
    Connection connect(Signal<A1, A2, A3>& signal, ClassT& object, R(ClassT::*memFunc)(A1, A2, A3) const)
    {
        return connect( signal, slot(object, memFunc) );
    }


    /// Connects a Signal to another Signal
    template <typename A1, typename A2, typename A3>
    Connection connect(Signal<A1, A2, A3>& sender, Signal<A1, A2, A3>& receiver)
    {
        return connect( sender, slot(receiver) );
    }

} // namespace cxxtools

#endif
