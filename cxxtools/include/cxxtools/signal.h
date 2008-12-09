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
                Sentry(const SignalBase* signal);

                ~Sentry();

                void detach();

                bool operator!() const
                { return _signal == 0; }

                const SignalBase* _signal;
            };

            SignalBase();

            ~SignalBase();

            SignalBase& operator=(const SignalBase& other);

            virtual void onConnectionOpen(const Connection& c);

            virtual void onConnectionClose(const Connection& c);

            void disconnectSlot(const Slot& slot);

        private:
            mutable Sentry* _sentry;
            mutable bool _sending;
            mutable bool _dirty;
    };


#include <cxxtools/signal.tpp>

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

template <typename R>
Connection connect(Signal<const cxxtools::Event&>& signal, R(*func)(const cxxtools::Event&))
{
    return signal.connect( slot(func) );
}

template <typename R, class BaseT, class ClassT>
Connection connect( Signal<const cxxtools::Event&>& signal,
                    BaseT& object, R(ClassT::*memFunc)(const cxxtools::Event&) )
{
    return signal.connect( slot(object, memFunc) );
}

template <typename R, class BaseT, class ClassT>
Connection connect( Signal<const cxxtools::Event&>& signal,
                    BaseT& object, R(ClassT::*memFunc)(const cxxtools::Event&) const )
{
    return signal.connect( slot(object, memFunc) );
}

inline Connection connect(Signal<const cxxtools::Event&>& sender, Signal<const cxxtools::Event&>& receiver)
{
    return sender.connect( slot(receiver) );
}

} // !namespace cxxtools

#endif
