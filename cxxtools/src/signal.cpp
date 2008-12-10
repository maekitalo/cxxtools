/*
 * Copyright (C) 2005 by Dr. Marc Boris Duerner
 * Copyright (C) 2005 Stephan Beal
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
#include "cxxtools/signal.h"

namespace cxxtools {

SignalBase::Sentry::Sentry(const SignalBase* signal)
: _signal(signal)
{
    _signal->_sentry = this;
    _signal->_sending = true;
    _signal->_dirty = false;
}


SignalBase::Sentry::~Sentry()
{
    if( _signal )
        this->detach();
}


void SignalBase::Sentry::detach()
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


SignalBase::SignalBase()
: _sentry(0)
, _sending(false)
, _dirty(false)
{ }


SignalBase::~SignalBase()
{
    if(_sentry)
    {
        _sentry->detach();
    }
}


SignalBase& SignalBase::operator=(const SignalBase& other)
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


void SignalBase::onConnectionOpen(const Connection& c)
{
    Connectable::onConnectionOpen(c);
}


void SignalBase::onConnectionClose(const Connection& c)
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


void SignalBase::disconnectSlot(const Slot& slot)
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


bool CompareEventTypeInfo::operator()(const std::type_info* t1,
                                      const std::type_info* t2) const
{
    if(t2 == 0)
        return false;

    if(t1 == 0)
        return true;

    return t1->before(*t2) != 0;
}


Signal<const cxxtools::Event&>::Sentry::Sentry(const Signal* signal)
: _signal(signal)
{
    _signal->_sentry = this;
    _signal->_sending = true;
    _signal->_dirty = false;
}


Signal<const cxxtools::Event&>::Sentry::~Sentry()
{
    if( _signal )
        this->detach();
}


void Signal<const cxxtools::Event&>::Sentry::detach()
{
    _signal->_sending = false;

    if( _signal->_dirty == false )
    {
        _signal->_sentry = 0;
        _signal = 0;
        return;
    }

    Signal::RouteMap::iterator it = _signal->_routes.begin();
    while( it != _signal->_routes.end() )
    {
        if( it->second->valid() )
        {
            ++it;
        }
        else
        {
            delete it->second;
            _signal->_routes.erase(it++);
        }
    }

    _signal->_dirty = false;
    _signal->_sentry = 0;
    _signal = 0;
}


Signal<const cxxtools::Event&>::Signal()
: _sentry(0)
, _sending(false)
, _dirty(false)
{}


Signal<const cxxtools::Event&>::~Signal()
{
	if(_sentry)
		_sentry->detach();

	while( ! _routes.empty() )
	{
		IEventRoute* route = _routes.begin()->second;
		route->connection().close();
	}
}


void Signal<const cxxtools::Event&>::send(const cxxtools::Event& ev) const
{
	// The sentry will set the Signal to the sending state and
	// reset it to not-sending upon destruction. In the sending
	// state, removing connection will leave invalid connections
	// in the connection list to keep the iterator valid, but mark
	// the Signal dirty. If the Signal is dirty, all invalid
	// connections will be removed by the Sentry when it destructs..
	Signal::Sentry sentry(this);

	RouteMap::iterator it = _routes.begin();
	while( true )
	{
		if( it == _routes.end() )
			return;

		if(it->first != 0)
			break;

		// The following scenarios must be considered when the
		// slot is called:
		// - The slot might get deleted and thus disconnected from
		//   this signal
		// - The slot might delete this signal and we must end
		//   calling any slots immediately
		// - A new Connection might get added to this Signal in
		//   the slot
		IEventRoute* route = it->second;
		if( route->valid() )
			route->route(ev);

		// if this signal gets deleted by the slot, the Sentry
		// will be detached. In this case we bail out immediately
		if( !sentry )
			return;

		++it;
	}

	const std::type_info& ti = ev.typeInfo();

	it = _routes.lower_bound( &ti );
	while(it != _routes.end() && *(it->first) == ti)
	{
		IEventRoute* route = it->second;

		if(route)
			route->route(ev);

		++it;

		// if this signal gets deleted by the slot, the Sentry
		// will be detached. In this case we bail out immediately
		if( !sentry )
			return;
	}
}


void Signal<const cxxtools::Event&>::onConnectionOpen(const Connection& c)
{
	const Connectable& sender = c.sender();
	if(&sender != this)
	{
		return Connectable::onConnectionOpen(c);
	}
}


void Signal<const cxxtools::Event&>::onConnectionClose(const Connection& c)
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
		RouteMap::iterator it;
		for(it = _routes.begin(); it != _routes.end(); ++it )
		{
			IEventRoute* route = it->second;
			if(route->connection() == c )
			{
				delete route;
				_routes.erase(it++);
				return;
			}
		}

		Connectable::onConnectionClose(c);
	}
}


void Signal<const cxxtools::Event&>::addRoute(const std::type_info* ti, IEventRoute* route)
{
	_routes.insert( std::make_pair(ti, route) );
}


void Signal<const cxxtools::Event&>::removeRoute(const Slot& slot)
{
	RouteMap::iterator it = _routes.begin();
	while( it != _routes.end() && it->first == 0 )
	{
		IEventRoute* route = it->second;
		if(route->connection().slot().equals(slot) )
		{
			route->connection().close();
			break;
		}
	}
}


void Signal<const cxxtools::Event&>::removeRoute(const std::type_info* ti, const Slot& slot)
{
	RouteMap::iterator it = _routes.lower_bound( ti );
	while(it != _routes.end() && *(it->first) == *ti)
	{
		IEventRoute* route = it->second;
		if(route->connection().slot().equals(slot) )
		{
			route->connection().close();
			break;
		}
	}
}

}
