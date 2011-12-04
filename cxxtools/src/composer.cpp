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

#include "cxxtools/composer.h"
#include <stdexcept>

namespace cxxtools
{
    void IComposer::fixup(const std::string& name)
    {
        SerializationInfo* p = _current->findMember(name);
        if( !p )
            throw std::runtime_error("member " + name + " not found");
        do_fixup(*p);
    }

    void IComposer::setCategory(SerializationInfo::Category category)
    {
        _current->setCategory(category);
    }

    void IComposer::setName(const std::string& name)
    {
        _current->setName(name);
    }

    void IComposer::setId(const std::string& id)
    {
        _current->setId(id);
    }

    void IComposer::setTypeName(const std::string& type)
    {
        _current->setTypeName(type);
    }

    void IComposer::setValue(const String& value)
    {
        _current->setValue(value);
    }

    void IComposer::setValue(const std::string& value)
    {
        _current->setValue(value);
    }

    void IComposer::setValue(const char* value)
    {
        _current->setValue(value);
    }

    void IComposer::setValue(bool value)
    {
        _current->setValue(value);
    }

    void IComposer::setValue(LongInt value)
    {
        _current->setValue(value);
    }

    void IComposer::setValue(ULongInt value)
    {
        _current->setValue(value);
    }

    void IComposer::setValue(long double value)
    {
        _current->setValue(value);
    }

    IComposer* IComposer::beginMember(const std::string& name, const std::string& type, SerializationInfo::Category category)
    {
        SerializationInfo& child = _current->addMember(name);
        child.setTypeName(type);
        child.setCategory(category);
        _current = &child;
        return this;
    }

    IComposer* IComposer::leaveMember()
    {
        SerializationInfo* p = _current->parent();
        if( !p )
            throw std::runtime_error("invalid member");

        _current = p;
        return this;
    }

} // namespace cxxtools
