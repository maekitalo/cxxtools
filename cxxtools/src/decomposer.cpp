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
#include "cxxtools/decomposer.h"
#include "cxxtools/formatter.h"

namespace cxxtools {

void IDecomposer::fixdownEach(cxxtools::SerializationInfo& si, SerializationContext& context)
{
    if(si.category() == cxxtools::SerializationInfo::Object)
    {
        cxxtools::SerializationInfo::Iterator it;
        for(it = si.begin(); it != si.end(); ++it)
        {
            fixdownEach(*it, context);
        }
    }
}


void IDecomposer::formatEach(const cxxtools::SerializationInfo& si, Formatter& formatter)
{
    if(si.category() == SerializationInfo::Value)
    {
        if (si.isInt())
        {
            SerializationInfo::LongInt value;
            si.getValue(value);
            formatter.addValue( si.name(), si.typeName(), value, si.id() );
        }
        else if (si.isUInt())
        {
            SerializationInfo::ULongInt value;
            si.getValue(value);
            formatter.addValue( si.name(), si.typeName(), value, si.id() );
        }
        else if (si.isFloat())
        {
            long double value;
            si.getValue(value);
            formatter.addValue( si.name(), si.typeName(), value, si.id() );
        }
        else if (si.isString8())
        {
            std::string value;
            si.getValue(value);
            formatter.addValue( si.name(), si.typeName(), value, si.id() );
        }
        else
        {
            String value;
            si.getValue(value);
            formatter.addValue( si.name(), si.typeName(), value, si.id() );
        }
    }
    else if(si.category() == SerializationInfo::Object)
    {
        formatter.beginObject( si.name(), si.typeName(), si.id() );

        SerializationInfo::ConstIterator it;
        for(it = si.begin(); it != si.end(); ++it)
        {
            formatter.beginMember( it->name() );
            formatEach(*it, formatter);
            formatter.finishMember();
        }

        formatter.finishObject();
    }
    else if(si.category() == cxxtools::SerializationInfo::Array)
    {
        formatter.beginArray( si.name(), si.typeName(), si.id() );

        SerializationInfo::ConstIterator it;
        for(it = si.begin(); it != si.end(); ++it)
        {
            formatEach(*it, formatter);
        }

        formatter.finishArray();
    }
}


SerializationContext::SerializationContext()
{
}


SerializationContext::~SerializationContext()
{
    this->clear();
}


void SerializationContext::clear()
{
    _omap.clear();

    std::vector<IDecomposer*>::iterator it;
    for(it = _stack.begin(); it != _stack.end(); ++it)
    {
        delete *it;
    }
    _stack.clear();
}


IDecomposer* SerializationContext::find(const void* p) const
{
    std::map<const void*, IDecomposer*>::const_iterator it;
    it = _omap.find(p);
    if(it == _omap.end())
        return 0;

    return it->second;
}


void SerializationContext::fixdown(Formatter& formatter)
{
    std::vector<IDecomposer*>::iterator it;
    for(it = _stack.begin(); it != _stack.end(); ++it)
    {
        IDecomposer* decomposer = *it;
        decomposer->fixdown(*this);
    }

    _omap.clear();

    for(it = _stack.begin(); it != _stack.end(); ++it)
    {
        (*it)->format(formatter);
    }
}


void SerializationContext::do_begin(const void* target, IDecomposer* decomposer)
{
    _omap[target] = decomposer;
    _stack.push_back(decomposer);
}

} // namespace cxxtools
