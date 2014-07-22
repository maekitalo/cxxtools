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

#ifndef cxxtools_EVENT_H
#define cxxtools_EVENT_H

#include <typeinfo>

namespace cxxtools
{

    /** \brief Base class for all event types.

        Specific Event objects, subclass from Event and implement the clone()
        and typeInfo() methods. The first is used to deep copy event objects
        for example in an EventLoop and the latter one is used to dispatch
        events by type.
     */
    class Event
    {
        public:
            /** \brief Destructor.
             */
            virtual ~Event()
            {}

            virtual Event* clone() const = 0;

            virtual void destroy() = 0;

            virtual const std::type_info& typeInfo() const = 0;
    };

    template <typename T>
    class BasicEvent : public Event
    {
        public:
            BasicEvent()
            {
            }

            BasicEvent(const BasicEvent& src)
            {
            }

            virtual const std::type_info& typeInfo() const
            {
                return typeid(T);
            }

            virtual Event* clone() const
            {
                return new T(*static_cast<const T*>(this));
            }

            virtual void destroy()
            {
                delete this;
            }
    };

} // namespace cxxtools

#endif
