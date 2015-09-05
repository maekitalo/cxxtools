/*
 * Copyright (C) 2004-2007 by Marc Boris Duerner
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

#ifndef cxxtools_Delegate_h
#define cxxtools_Delegate_h

#include <cxxtools/void.h>
#include <cxxtools/function.h>
#include <cxxtools/method.h>
#include <cxxtools/constmethod.h>
#include <cxxtools/connectable.h>
#include <stdexcept>


namespace cxxtools {

    /** @internal
    */
    class DelegateBase : public Connectable
    {
        public:
            DelegateBase()
            { }

            DelegateBase(const DelegateBase& rhs)
                : Connectable(rhs)
            { operator=(rhs); }

            DelegateBase& operator=(const DelegateBase& other)
            {
                _target.close();

                if( !other._target )
                    return *this;

                const Slot& slot = other._target.slot();
                _target = Connection( *this, slot.clone()  );

                return *this;
            }

            virtual void onConnectionOpen(const Connection& c)
            {
                const Connectable& sender = c.sender();

                if( &sender == this )
                {
                    _target.close();
                    _target = c;
                }

                Connectable::onConnectionOpen(c);
            }

            virtual void onConnectionClose(const Connection& c)
            {
                Connectable::onConnectionClose(c);
            }

            bool isConnected() const
            {
                return _target.valid();
            }

        protected:
            Connection _target;
    };


#include <cxxtools/delegate.tpp>

} // !namespace cxxtools

#endif
