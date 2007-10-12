/***************************************************************************
 *   Copyright (C) 2005-2007 by Marc Boris Duerner                         *
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

#ifndef cxxtools_invokable_h
#define cxxtools_invokable_h

#include <cxxtools/void.h>


namespace cxxtools {

template < typename A1 = Void,
           typename A2 = Void,
           typename A3 = Void,
           typename A4 = Void,
           typename A5 = Void,
           typename A6 = Void,
           typename A7 = Void,
           typename A8 = Void >
class Invokable {
    public:
        virtual ~Invokable()
        {}

        virtual void invoke(A1, A2, A3, A4, A5, A6, A7, A8) const = 0;
};


template < typename A1,
           typename A2,
           typename A3,
           typename A4,
           typename A5 >
class Invokable<A1, A2, A3, A4, A5, Void, Void, Void> {
    public:
        virtual ~Invokable()
        {}

        virtual void invoke(A1, A2, A3, A4, A5) const = 0;
};


template < typename A1,
           typename A2,
           typename A3,
           typename A4 >
class Invokable<A1, A2, A3, A4, Void, Void, Void, Void> {
    public:
        virtual ~Invokable()
        {}

        virtual void invoke(A1, A2, A3, A4) const = 0;
};


template < typename A1,
           typename A2,
           typename A3 >
class Invokable<A1, A2, A3, Void, Void, Void, Void, Void> {
    public:
        virtual ~Invokable()
        {}

        virtual void invoke(A1, A2, A3) const = 0;
};


template < typename A1,
           typename A2 >
class Invokable<A1, A2, Void, Void, Void, Void, Void, Void> {
    public:
        virtual ~Invokable()
        {}

        virtual void invoke(A1, A2) const = 0;
};


template <typename A1>
class Invokable<A1, Void, Void, Void, Void, Void, Void, Void> {
    public:
        virtual ~Invokable()
        {}

        virtual void invoke(A1) const = 0;
};


template <>
class Invokable<Void, Void, Void, Void, Void, Void, Void, Void> {
    public:
        virtual ~Invokable()
        {}

        virtual void invoke() const = 0;
};

} // namespace cxxtools


#endif
