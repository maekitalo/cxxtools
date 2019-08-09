/*
 * Copyright (C) 2004-2007 by Dr. Marc Boris Duerner
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
#ifndef cxxtools_Connectable_h
#define cxxtools_Connectable_h

#include <cxxtools/connection.h>
#include <list>
#include <unistd.h>

namespace cxxtools {

    /** @brief %Connection management for signal and slot objects
        @ingroup sigslot

        This class implements connection management for signal and slot
        objects. It makes sure that all connections where this object
        is involved are closed on destruction. Deriving classes can
        overload Connectable::opened and Connectable::closed to tune
        connection managenment.
    */
    class Connectable
    {
        public:
            /** @brief Default constructor.

                Creates an empty %Connectable.
            */
            Connectable()
            { }

            /** @brief Closes all connections.

                When a %Connectable object is destroyed, it closes all its
                connections automatically.
            */
            virtual ~Connectable();

            /** @brief Registers a Connection with the %Connectable.

                This function is called when a new Connection involving
                this object is opened. The default implementation adds
                the connection to a list, so the destructor can close it.

                @param c Connection being opened
                @return True if the Connection was accepted
            */
            virtual void onConnectionOpen(const Connection& c);

            /** @brief Unregisters a Connection from the %Connectable.

                This function is called when a new Connection involving
                this object is closed. The default implementation removes
                the connection from its list of connections.

                @param c Connection being opened
            */
            virtual void onConnectionClose(const Connection& c);

            //! @internal @brief For unit tests only.
            size_t connectionCount() const
            { return _connections.size(); }

        protected:
            /** @brief Copy constructor

                @sa Connectable::operator=()
            */
            Connectable(const Connectable& /*c*/)
            { }

            /** @brief Assignment operator

                Connectables can be copy constructed if the derived class
                provides a public copy constructor. Copying a %Connectable
                will not change its connections.
            */
            Connectable& operator=(const Connectable& /*rhs*/)
            { return *this; }

            /** @brief Returns a list of all current connections
            */
            const std::list<Connection>& connections() const
            { return _connections; }

            /** @brief Returns a list of all current connections
            */
            std::list<Connection>& connections()
            { return _connections; }

        protected:
            /** @brief A list of all current connections
            */
            mutable std::list<Connection> _connections;

            //! @internal
            void clear();
    };

} // namespace cxxtools

#endif
