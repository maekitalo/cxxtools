/*
 * Copyright (C) 2014 Tommi Maekitalo
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * is provided AS IS, WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, and
 * NON-INFRINGEMENT.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 *
 */

#ifndef CXXTOOLS_SCOPEDINCREMENT_H
#define CXXTOOLS_SCOPEDINCREMENT_H

#include <cxxtools/atomicity.h>

namespace cxxtools
{
template <typename T>
class ScopedIncrement
{
        T& _value;
        T _count;

    public:
        ScopedIncrement(T& value, T count = 1)
            : _value(value),
              _count(count)
        {
            _value += _count;
        }

        ~ScopedIncrement()
        {
            _value -= _count;
        }
};

template <>
class ScopedIncrement<atomic_t>
{
        atomic_t& _value;
        int _count;

    public:
        ScopedIncrement(atomic_t& value, int count = 1)
            : _value(value),
              _count(count)
        {
            int c = _count;
            for (; c > 0; --c)
                atomicIncrement(_value);
            for (; c < 0; ++c)
                atomicDecrement(_value);
        }

        ~ScopedIncrement()
        {
            int c = _count;
            for (c = _count; c > 0; --c)
                atomicDecrement(_value);
            for (; c < 0; ++c)
                atomicIncrement(_value);
        }
};

}

#endif // CXXTOOLS_SCOPEDINCREMENT_H

