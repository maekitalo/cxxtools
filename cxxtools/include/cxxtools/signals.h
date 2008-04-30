/***************************************************************************
 *   Copyright (C) 2004-2006 by Dr. Marc Boris Duerner                     *
 *   Copyright (C) 2005 Stephan Beal                                       *
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

#ifndef cxxtools_Signal_h
#define cxxtools_Signal_h

#include <cxxtools/void.h>
#include <cxxtools/function.h>
#include <cxxtools/method.h>
#include <cxxtools/constmethod.h>
#include <cxxtools/connectable.h>

#include <list>
#include <algorithm>


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

                for( ; it != end; ++it) {
                    const Slot& slot = it->slot();
                    Connection connection( *this, slot.clone()  );
                }

                return *this;
            }

            virtual bool opened(const Connection& c)
            {
                return Connectable::opened(c);
            }

            virtual void closed(const Connection& c)
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
                    Connectable::closed(c);
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

} // !namespace cxxtools

#endif
