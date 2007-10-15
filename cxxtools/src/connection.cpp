/***************************************************************************
 *   Copyright (C) 2004-2006 by Marc Boris Duerner                         *
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
 **************************************************************************/
#include "cxxtools/connectable.h"
#include <memory>

namespace cxxtools {

Connection::Connection()
{
    _data = new ConnectionData();
}


Connection::Connection(Connectable& sender, Slot* slot)
{
    std::auto_ptr<ConnectionData> data( new ConnectionData(sender, slot) );
    _data = data.get();
    _data->setValid(false);

    // Return if the sender does not accept connections and keep the
    // Connection in the invalid state
    if ( false == sender.opened(*this) )
    {
        data.release();
        return;
    }

    // It the sender did accept the connection, but the receiver does not,
    // we close the connection for the sender and keep this connection
    // in invalid state
    if( false == slot->opened(*this) )
    {
        sender.closed(*this);
        data.release();
        return;
    }

    // Connection was opened successfully
    _data->setValid(true);
    data.release();
}


Connection::Connection(const Connection& connection)
{
    _data = connection._data;
    _data->ref();
}


Connection::~Connection()
{
    if( _data->unref() > 0) {
        return;
    }

    // close the connection if its still valid
    if( this->valid() ) {
        this->close();
    }

    // delete the shared data
    delete _data;
    _data = 0;
}


void Connection::close()
{
    if( !this->valid() )
        return;

    _data->slot().closed( *this );
    _data->sender().closed( *this );
    _data->setValid(false);
}


Connection& Connection::operator=(const Connection& connection)
{
    if( 0 == _data->unref()) 
    {
        this->close();        
        delete _data;   
    }
    
    _data = connection._data;
    _data->ref();
    return (*this);
}


bool Connection::operator==(const Connection& connection) const
{
    // compare pointers or callable?
    return _data == connection._data;
}

} //namespace cxxtools


