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
#include "cxxtools/deserializer.h"

namespace cxxtools {

void IDeserializer::fixupEach(IDeserializer* deser, cxxtools::SerializationInfo& si, DeserializationContext& ctx)
{
    cxxtools::SerializationInfo::Iterator it;
    for(it = si.begin(); it != si.end(); ++it)
    {
        if(it->category() == cxxtools::SerializationInfo::Reference)
        {
            ctx.addFixup( it->toValue<std::string>(), it->fixupAddr(), it->fixupInfo() );
        }
    }

    if( ! si.id().empty() )
    {
        ctx.addObject( si.id(), deser->target(), deser->targetType() );
    }
}


DeserializationContext::DeserializationContext()
{
}


DeserializationContext::~DeserializationContext()
{
}


void DeserializationContext::addObject(const std::string& id, void* obj, const std::type_info& fixupInfo)
{
    FixupInfo fi;
    fi.address = obj;
    fi.type = &fixupInfo;
    _targets[id] = fi;
}


void DeserializationContext::addFixup(const std::string& id, void* obj, const std::type_info& fixupInfo)
{
    FixupInfo fi;
    fi.address = obj;
    fi.type = &fixupInfo;
    _pointers[id] = fi;
}


void DeserializationContext::clear()
{
    _targets.clear();
    _pointers.clear();
}


void DeserializationContext::fixup()
{
    std::map<std::string, FixupInfo>::iterator it;
    for(it = _pointers.begin(); it != _pointers.end(); ++it)
    {
        void* fixme = it->second.address;
        const std::type_info* fixupType = it->second.type;
        std::string id = it->first;
        void* target = _targets[id].address;
        const std::type_info* targetType = _targets[id].type ;

        bool fixupAllowed = this->checkFixup(*fixupType, *targetType);
        if( ! fixupAllowed )
            throw SerializationError("reference fixup failed, type mismatch");

        void** vp =(void**)(fixme);
        *vp = target;
    }

    clear();
}


bool DeserializationContext::checkFixup(const std::type_info& from, const std::type_info& to)
{
    return from == to;
}


} // namespace cxxtools
