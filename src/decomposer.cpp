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

namespace cxxtools
{

void IDecomposer::formatEach(const SerializationInfo& si, Formatter& formatter)
{
    if (si.category() == SerializationInfo::Void)
    {
        formatter.addNull( si.name(), si.typeName() );
    }
    else if (si.category() == SerializationInfo::Value)
    {
        if (si.isInt())
        {
            int_type value;
            si.getValue(value);
            formatter.addValueInt( si.name(), si.typeName(), value );
        }
        else if (si.isUInt())
        {
            unsigned_type value;
            si.getValue(value);
            formatter.addValueUnsigned( si.name(), si.typeName(), value );
        }
        else if (si.isBool())
        {
            bool value;
            si.getValue(value);
            formatter.addValueBool( si.name(), si.typeName(), value );
        }
        else if (si.isFloat())
        {
            float value;
            si.getValue(value);
            formatter.addValueFloat( si.name(), si.typeName(), value );
        }
        else if (si.isDouble())
        {
            double value;
            si.getValue(value);
            formatter.addValueDouble( si.name(), si.typeName(), value );
        }
        else if (si.isLongDouble())
        {
            long double value;
            si.getValue(value);
            formatter.addValueLongDouble( si.name(), si.typeName(), value );
        }
        else if (si.isString8())
        {
            std::string value;
            si.getValue(value);
            formatter.addValueStdString( si.name(), si.typeName(), value );
        }
        else
        {
            String value;
            si.getValue(value);
            formatter.addValueString( si.name(), si.typeName(), value );
        }
    }
    else if(si.category() == SerializationInfo::Object)
    {
        formatter.beginObject( si.name(), si.typeName() );

        SerializationInfo::ConstIterator it;
        for(it = si.begin(); it != si.end(); ++it)
        {
            formatter.beginMember( it->name() );
            formatEach(*it, formatter);
            formatter.finishMember();
        }

        formatter.finishObject();
    }
    else if(si.category() == SerializationInfo::Array)
    {
        formatter.beginArray( si.name(), si.typeName() );

        SerializationInfo::ConstIterator it;
        for(it = si.begin(); it != si.end(); ++it)
        {
            formatEach(*it, formatter);
        }

        formatter.finishArray();
    }
}


} // namespace cxxtools
