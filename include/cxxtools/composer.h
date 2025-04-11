/*
 * Copyright (C) 2008 by Marc Boris Duerner
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
#ifndef cxxtools_Composer_h
#define cxxtools_Composer_h

#include <cxxtools/serializationinfo.h>

namespace cxxtools
{

class IComposer
{
    public:
        IComposer() = default;
        virtual ~IComposer() = default;

        virtual void fixup(SerializationInfo& si) = 0;
};


template <typename T>
class Composer : public IComposer
{
    public:
        Composer()
        : _type(0)
        {}

        void begin(T& type)
        {
            _type = &type;
        }

        void fixup(SerializationInfo& si) override
        {
            si >>= *_type;
        }

    private:
        T* _type;
};

class IComposers
{
    public:
        virtual ~IComposers() = default;
        virtual bool needMore() = 0;
        virtual IComposer* get() = 0;   // get next composer
};

class Composers : public IComposers
{
        IComposer** _composers;
        unsigned _count;
        unsigned _current;

    public:
        Composers(IComposer** composers, unsigned count)
            : _composers(composers),
              _count(count),
              _current(0)
        { }

        bool needMore() override        { return _current < _count; }
        IComposer* get() override       { return _current < _count ? _composers[_current++] : nullptr; }
};

} // namespace cxxtools

#endif
