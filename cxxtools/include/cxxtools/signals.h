/***************************************************************************
 *   Copyright (C) 2004-2006 by Dr. Marc Boris Duerner                     *
 *   Copyright (C) 2005 Stephan Beal                                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   As a special exception, you may use this file as part of a free       *
 *   software library without restriction. Specifically, if other files    *
 *   instantiate templates or use macros or inline functions from this     *
 *   file, or you compile this file and link it with other files to        *
 *   produce an executable, this file does not by itself cause the         *
 *   resulting executable to be covered by the GNU General Public          *
 *   License. This exception does not however invalidate any other         *
 *   reasons why the executable file might be covered by the GNU Library   *
 *   General Public License.                                               *
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
#include <cxxtools/event.h>
#include <cxxtools/function.h>
#include <cxxtools/method.h>
#include <cxxtools/constmethod.h>
#include <cxxtools/connectable.h>
#include <list>
#include <map>


namespace cxxtools {

    /** @internal
    */
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
                this->clear();

                std::list<Connection>::const_iterator it = other.connections().begin();
                std::list<Connection>::const_iterator end = other.connections().end();

                for( ; it != end; ++it)
                {
                    const Connectable& signal = it->sender();
                    if( &signal == &other)
                    {
                        const Slot& slot = it->slot();
                        Connection connection( *this, slot.clone()  );
                    }
                }

                return *this;
            }

            virtual void onConnectionOpen(const Connection& c)
            {
                Connectable::onConnectionOpen(c);
            }

            virtual void onConnectionClose(const Connection& c)
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
                    Connectable::onConnectionClose(c);
                }
            }

            void disconnectSlot(const Slot& slot)
            {
                std::list<Connection>::iterator it = Connectable::connections().begin();
                std::list<Connection>::iterator end = Connectable::connections().end();

                for(; it != end; ++it)
                {
                    if( it->slot().equals(slot) )
                    {
                        it->close();
                        return;
                    }
                }
            }

        private:
            mutable Sentry* _sentry;
            mutable bool _sending;
            mutable bool _dirty;
    };


#include <cxxtools/signals.tpp>

struct CompareEventTypeInfo
{
    bool operator()( const std::type_info* t1,
                     const std::type_info* t2 ) const;
};

template <>
class Signal<const cxxtools::Event&> : public Connectable
                                      , protected NonCopyable
{
    struct Sentry
    {
        Sentry(const Signal* signal);

        ~Sentry();

        void detach();

        bool operator!() const
        { return _signal == 0; }

        const Signal* _signal;
    };

    class IEventRoute
    {
        public:
            IEventRoute(Connection& target)
            : _target(target)
            { }

            virtual ~IEventRoute() {}

            virtual void route(const cxxtools::Event& ev)
            {
                typedef Invokable<const cxxtools::Event&> InvokableT;
                const InvokableT* invokable = static_cast<const InvokableT*>( _target.slot().callable() );
                invokable->invoke(ev);
            }

            Connection& connection()
            { return _target; }

            bool valid() const
            { return _target.valid(); }

        private:
            Connection _target;
    };

    template <typename EventT>
    class EventRoute : public IEventRoute
    {
        public:
            EventRoute(Connection& target)
            : IEventRoute(target)
            { }

            virtual void route(const cxxtools::Event& ev)
            {
                typedef Invokable<const cxxtools::Event&> InvokableT;
                const InvokableT* invokable = static_cast<const InvokableT*>( connection().slot().callable() );

                const EventT& event = static_cast<const EventT&>(ev);
                invokable->invoke(event);
            }
    };

    typedef std::multimap< const std::type_info*,
                           IEventRoute*,
                           CompareEventTypeInfo > RouteMap;

    public:
        Signal();

        ~Signal();

        void send(const cxxtools::Event& ev) const;

        template <typename R>
        Connection connect(const BasicSlot<R, const cxxtools::Event&>& slot)
        {
            Connection conn( *this, slot.clone() );
            this->addRoute( 0, new IEventRoute(conn) );
            return conn;
        }

        template <typename R>
        void disconnect(const BasicSlot<R, const cxxtools::Event&>& slot)
        {
            this->removeRoute(slot);
        }

        template <typename EventT>
        void subscribe( const BasicSlot<void, const EventT&>& slot )
        {
            Connection conn( *this, slot.clone() );
            const std::type_info& ti = typeid(EventT);
            this->addRoute( &ti, new EventRoute<EventT>(conn) );
        }

        template <typename EventT>
        void unsubscribe( const BasicSlot<void, const EventT&>& slot )
        {
            const std::type_info& ti = typeid(EventT);
            this->removeRoute(&ti, slot);
        }

        virtual void onConnectionOpen(const Connection& c);

        virtual void onConnectionClose(const Connection& c);

    protected:
        void addRoute(const std::type_info* ti, IEventRoute* route);

        void removeRoute(const Slot& slot);

        void removeRoute(const std::type_info* ti, const Slot& slot);

    private:
        mutable RouteMap _routes;
        mutable Sentry* _sentry;
        mutable bool _sending;
        mutable bool _dirty;
};

} // !namespace cxxtools

#endif
