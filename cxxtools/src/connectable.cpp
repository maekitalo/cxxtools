/***************************************************************************
 *   Copyright (C) 2004-2006 by Marc Boris Duerner                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   As a special exception, you may use this file as part of a free
 *   software library without restriction. Specifically, if other files
 *   instantiate templates or use macros or inline functions from this
 *   file, or you compile this file and link it with other files to
 *   produce an executable, this file does not by itself cause the
 *   resulting executable to be covered by the GNU General Public
 *   License. This exception does not however invalidate any other
 *   reasons why the executable file might be covered by the GNU Library
 *   General Public License.
 *   
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
#include "cxxtools/connection.h"

namespace cxxtools {

Connectable::Connectable()
{}


Connectable::Connectable(const Connectable& c)
{ 
    this->operator=(c); 
}


Connectable::~Connectable()
{
    this->clear();
}


void Connectable::clear()
{
    while( !_connections.empty() ) 
    {
        Connection connection = _connections.front();
        connection.close();
    }
}


Connectable& Connectable::operator=(const Connectable& other)
{
    return (*this);
}


bool Connectable::opened(const Connection& c)
{
    _connections.push_back(c);
    return true;
}


void Connectable::closed(const Connection& c)
{
    _connections.remove(c);
}

} // namespace cxxtools
