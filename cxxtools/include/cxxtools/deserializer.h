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

        virtual void setName(const std::string& name) = 0;

        virtual void setValue(const cxxtools::String& value) = 0;

        virtual void setId(const std::string& id) = 0;

        virtual void setReference(const std::string& id) = 0;

        virtual IDeserializer* beginMember(const std::string& name, const std::string& type, SerializationInfo::Category category) = 0;

        virtual IDeserializer* leaveMember() = 0;

        virtual void fixup(DeserializationContext& ctx) = 0;

    protected:
        void fixupEach(IDeserializer* deser, cxxtools::SerializationInfo& si, DeserializationContext& ctx);

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

        virtual void setName(const std::string& name)
        {
            _current->setName(name);
        }

        virtual void setId(const std::string& id)
        {
            _current->setId(id);
        }

        virtual void setValue(const cxxtools::String& value)
        {
            _current->setValue(value);
        }

        virtual void setReference(const std::string& id)
        {
           _current->setValue(id);
           _current->setCategory(SerializationInfo::Reference);
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
            // SI's for unfixed pointers contain the fixup address now
            // other types may only point to _type,but not to its members
            *_current >>= *_type;

            fixupEach(this, _si, ctx);
        }

    private:
        T* _type;
        cxxtools::SerializationInfo _si;
        cxxtools::SerializationInfo* _current;
};


class CXXTOOLS_API DeserializationContext
{
    struct FixupInfo
    {
        void* address;
        const std::type_info* type;
    };

    public:
        DeserializationContext();

        virtual ~DeserializationContext();

        void addObject(const std::string& id, void* obj, const std::type_info& fixupInfo);

        void addFixup(const std::string& id, void* obj, const std::type_info& fixupInfo);

        void clear();

        void fixup();

    protected:
        virtual bool checkFixup(const std::type_info& from, const std::type_info& to);

    private:
        std::map<std::string, FixupInfo> _targets;
        std::map<std::string, FixupInfo> _pointers;
};

} // namespace cxxtools

#endif
