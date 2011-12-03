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

#include <cxxtools/api.h>
#include <cxxtools/serializationinfo.h>

namespace cxxtools
{

class CXXTOOLS_API IComposer
{
    public:
#ifdef HAVE_LONG_LONG
        typedef long long LongInt;
#else
        typedef long LongInt;
#endif
#ifdef HAVE_UNSIGNED_LONG_LONG
        typedef unsigned long long ULongInt;
#else
        typedef unsigned long LongInt;
#endif

        IComposer()
        : _parent(0),
          _current(&_si)
        {}

        virtual ~IComposer()
        {}

        void setParent(IComposer* parent)
        { _parent = parent; }

        IComposer* parent()
        { return _parent; }

        void setCategory(SerializationInfo::Category category);

        void setName(const std::string& name);

        void setValue(const String& value);

        void setValue(const std::string& value);

        void setValue(const char* value);

        void setValue(bool value);

        void setValue(LongInt value);

        void setValue(ULongInt value);

        void setValue(long double value);

        void setId(const std::string& id);

        void setTypeName(const std::string& type);

        IComposer* beginMember(const std::string& name, const std::string& type, SerializationInfo::Category category);

        IComposer* leaveMember();

        virtual void fixup() = 0;

        void fixupMember(const std::string& name);

    protected:
        SerializationInfo _si;
        SerializationInfo* _current;

    private:
        IComposer* _parent;
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
            _current = &_si;
            _si.clear();
        }

        virtual void fixup()
        {
            *_current >>= *_type;
        }

    private:
        T* _type;
};


} // namespace cxxtools

#endif
