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
#ifndef cxxtools_Deserializer_h
#define cxxtools_Deserializer_h

#include <cxxtools/api.h>
#include <cxxtools/serializationinfo.h>
#include <map>
#include <typeinfo>

namespace cxxtools {

class DeserializationContext;


class CXXTOOLS_API IDeserializer
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

        IDeserializer()
        : _parent(0)
        {}

        virtual ~IDeserializer()
        {}

        void setParent(IDeserializer* parent)
        { _parent = parent; }

        IDeserializer* parent()
        { return _parent; }

        virtual void* target() = 0;

        virtual const std::type_info& targetType() = 0;

        virtual void setCategory(SerializationInfo::Category category) = 0;

        virtual void setName(const std::string& name) = 0;

        virtual void setValue(const String& value) = 0;

        virtual void setValue(const std::string& value) = 0;

        virtual void setValue(const char* value) = 0;

        virtual void setValue(bool value) = 0;

        virtual void setValue(LongInt value) = 0;

        virtual void setValue(ULongInt value) = 0;

        virtual void setValue(long double value) = 0;

        virtual void setId(const std::string& id) = 0;

        virtual void setTypeName(const std::string& type) = 0;

        virtual IDeserializer* beginMember(const std::string& name, const std::string& type, SerializationInfo::Category category) = 0;

        virtual IDeserializer* leaveMember() = 0;

        virtual void fixup(DeserializationContext& ctx) = 0;

    protected:
        void fixupEach(IDeserializer* deser, SerializationInfo& si, DeserializationContext& ctx);

    private:
        IDeserializer* _parent;
};


template <typename T>
class Deserializer : public IDeserializer
{
    public:
        Deserializer()
        : _type(0)
        , _current(&_si)
        {}

        void begin(T& type)
        {
            _type = &type;
            _current = &_si;
            _si.clear();
        }

        virtual void* target()
        {
            return _type;
        }

        virtual const std::type_info& targetType()
        {
            return typeid(T);
        }

        virtual void setCategory(SerializationInfo::Category category)
        {
            _current->setCategory(category);
        }

        virtual void setName(const std::string& name)
        {
            _current->setName(name);
        }

        virtual void setId(const std::string& id)
        {
            _current->setId(id);
        }

        virtual void setTypeName(const std::string& type)
        {
            _current->setTypeName(type);
        }

        virtual void setValue(const String& value)
        {
            _current->setValue(value);
        }

        virtual void setValue(const std::string& value)
        {
            _current->setValue(value);
        }

        virtual void setValue(const char* value)
        {
            _current->setValue(value);
        }

        virtual void setValue(bool value)
        {
            _current->setValue(value);
        }

        virtual void setValue(LongInt value)
        {
            _current->setValue(value);
        }

        virtual void setValue(ULongInt value)
        {
            _current->setValue(value);
        }

        virtual void setValue(long double value)
        {
            _current->setValue(value);
        }

        virtual IDeserializer* beginMember(const std::string& name, const std::string& type, SerializationInfo::Category category)
        {
            SerializationInfo& child = _current->addMember(name);
            child.setTypeName(type);
            child.setCategory(category);
            _current = &child;
            return this;
        }

        virtual IDeserializer* leaveMember()
        {
            if( ! _current->parent() )
            {
                *_current >>= *_type;

                if( ! this->parent() )
                    throw std::runtime_error("invalid member");

                return this->parent();
            }

            _current = _current->parent();
            return this;
        }

        virtual void fixup(DeserializationContext& ctx)
        {
            *_current >>= *_type;
        }

    private:
        T* _type;
        SerializationInfo _si;
        SerializationInfo* _current;
};


class CXXTOOLS_API DeserializationContext
{
    struct FixupInfo
    {
        void* address;
        const std::type_info* type;
    };

    public:
        DeserializationContext()
        { }

        virtual ~DeserializationContext()
        { }

        void clear();

        void fixup();

};

} // namespace cxxtools

#endif
