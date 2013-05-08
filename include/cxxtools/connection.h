/*
 * Copyright (C) 2006-2008 by Marc Boris Duerner
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

#ifndef cxxtools_Connection_h
#define cxxtools_Connection_h

#include <cxxtools/slot.h>
#include <cxxtools/refcounted.h>

namespace cxxtools {

    class Connectable;

    /** @internal
    */
    class ConnectionData : public RefCounted {
        public:
            ConnectionData()
            : _refs(1)
            , _valid(false)
            , _slot(0)
            , _sender(0)
            { }

            ConnectionData(Connectable& sender, Slot* slot)
            : _refs(1)
            , _valid(true)
            , _slot(slot)
            , _sender(&sender)
            { }

            ~ConnectionData()
            { delete _slot; }

            unsigned ref()
            { return ++_refs; }

            unsigned unref()
            { return --_refs; }

            unsigned refs() const
            { return _refs; }

            bool valid() const
            { return _valid; }

            void setValid(bool valid)
            { _valid = valid; }

            Connectable& sender()
            { return *_sender; }

            const Connectable& sender() const
            { return *_sender; }

            Slot& slot()
            { return *_slot; }

            const Slot& slot() const
            { return *_slot; }

        private:
            unsigned _refs;
            bool _valid;
            Slot* _slot;
            Connectable* _sender;
    };

    /** @brief Represents a connection between a Signal/Delegate and a slot
        @ingroup sigslot
    */
    class Connection
    {
        public:
            Connection();

            Connection(Connectable& sender, Slot* slot);

            Connection(const Connection& connection);

            ~Connection();

            bool valid() const
            { return _data->valid(); }

            const Connectable& sender() const
            { return _data->sender(); }

            const Slot& slot() const
            { return _data->slot(); }

            bool operator!() const
            { return this->valid() == false; }

            void close();

            Connection& operator=(const Connection& connection);

            bool operator==(const Connection& connection) const;

        private:
            ConnectionData* _data;
    };

} // namespace cxxtools

#endif
