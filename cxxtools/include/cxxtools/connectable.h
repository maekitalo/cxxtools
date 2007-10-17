/***************************************************************************
 *   Copyright (C) 2004-2007 by Dr. Marc Boris Duerner                     *
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
#ifndef cxxtools_Connectable_h
#define cxxtools_Connectable_h

#include <cxxtools/connection.h>
#include <list>

namespace cxxtools {

    /** @brief Connection management for signal and slot objects

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
            Connectable();

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
            virtual bool opened(const Connection& c);

            /** @brief Unregisters a Connection from the %Connectable.

                This function is called when a new Connection involving
                this object is closed. The default implementation removes
                the connection from its list of connections.

                @param c Connection being opened
            */
            virtual void closed(const Connection& c);

            //! @internal @brief For unit tests only.
            size_t connectionCount() const
            { return _connections.size(); }

        protected:
            /** @brief Copy constructor

                @sa Connectable::operator=()
            */
            Connectable(const Connectable& c);

            /** @brief Assignment operator

                Connectables can be copy constructed if the derived class
                provides a public copy constructor. Copying a %Connectable
                will close all current connections and copy all the
                Connections of the other %Connectable.

                @param c %Connectable to copy
            */
            Connectable& operator=(const Connectable& rhs);

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

        private:
            //! @internal
            void clear();
    };

} // namespace cxxtools

#endif
