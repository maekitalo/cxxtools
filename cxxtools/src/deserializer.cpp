/*
 * Copyright (C) 2011 by Tommi Maekitalo
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

#include "cxxtools/deserializer.h"
#include <stdexcept>

namespace cxxtools
{
    void DeserializerBase::setCategory(SerializationInfo::Category category)
    {
        _current->setCategory(category);
    }

    void DeserializerBase::setName(const std::string& name)
    {
        _current->setName(name);
    }

    void DeserializerBase::setId(const std::string& id)
    {
        _current->setId(id);
    }

    void DeserializerBase::setTypeName(const std::string& type)
    {
        _current->setTypeName(type);
    }

    void DeserializerBase::setValue(const String& value)
    {
        _current->setValue(value);
    }

    void DeserializerBase::setValue(const std::string& value)
    {
        _current->setValue(value);
    }

    void DeserializerBase::setValue(const char* value)
    {
        _current->setValue(value);
    }

    void DeserializerBase::setValue(bool value)
    {
        _current->setValue(value);
    }

    void DeserializerBase::setValue(int_type value)
    {
        _current->setValue(value);
    }

    void DeserializerBase::setValue(unsigned_type value)
    {
        _current->setValue(value);
    }

    void DeserializerBase::setValue(long double value)
    {
        _current->setValue(value);
    }

    void DeserializerBase::setNull()
    {
        _current->setNull();
    }

    void DeserializerBase::beginMember(const std::string& name, const std::string& type, SerializationInfo::Category category)
    {
        SerializationInfo& child = _current->addMember(name);
        child.setTypeName(type);
        child.setCategory(category);
        _current = &child;
    }

    void DeserializerBase::leaveMember()
    {
        SerializationInfo* p = _current->parent();
        if( !p )
            throw std::runtime_error("invalid member");

        _current = p;
    }

} // namespace cxxtools
