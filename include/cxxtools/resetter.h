/*
 * Copyright (C) 2017 Tommi Maekitalo
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

#ifndef CXXTOOLS_RESETTER_H
#define CXXTOOLS_RESETTER_H

namespace cxxtools
{

/**
 * Template class which resets a value to its original or specified state on destruction.
 *
 * An instance should be put on the stack.
 * The value is reset in the destructor so that it is guaranteed to be
 * executed even on exception.
 *
 * \code
 *      int p = 42;
 *      {
 *          Resetter resetter(p);
 *          p = 43;
 *      }
 *      // the value of p is 42 here.
 * \endcode
 */
template <typename T>
class Resetter
{
        T& _t;
        T _sav;

    public:
        explicit Resetter(T& t)
            : _t(t),
              _sav(t)
        { }
        Resetter(T& t, T value)
            : _t(t),
              _sav(value)
        { }
        ~Resetter()
        {
            _t = _sav;
        }
};

}

#endif
