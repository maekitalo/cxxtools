/*
 * Copyright (C) 2004-2006 by Marc Boris Duerner
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
#include "cxxtools/connectable.h"
#include "cxxtools/connection.h"
#include "cxxtools/log.h"

log_define("cxxtools.connectable")

namespace cxxtools {

Connectable::~Connectable()
{
    this->clear();
}


void Connectable::clear()
{
    while( !_connections.empty() )
    {
        Connection* c = &_connections.front();
        c->close();
        if (&_connections.front() == c)
        {
            // this should not really happen bug just in case we do not want to loop endlessly
            log_fatal("connection " << static_cast<void*>(c) << " was not removed from " << static_cast<void*>(this));
            _connections.pop_front();
        }
    }
}


void Connectable::onConnectionOpen(const Connection& c)
{
    _connections.push_back(c);
}


void Connectable::onConnectionClose(const Connection& c)
{
    _connections.remove(c);
}

} // namespace cxxtools
