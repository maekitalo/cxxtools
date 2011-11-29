/*
 * Copyright (C) 2005-2007 by Dr. Marc Boris Duerner
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
#include "settingswriter.h"

namespace cxxtools {

void SettingsWriter::write(const cxxtools::SerializationInfo& si)
{
    cxxtools::SerializationInfo::ConstIterator it;
    for(it = si.begin(); it != si.end(); ++it)
    {
        if( it->category() == cxxtools::SerializationInfo::Value )
        {
            this->writeEntry( it->name(), it->toValue<String>(), it->typeName() );
            *_os << std::endl;
        }
        else if( it->category() == cxxtools::SerializationInfo::Object)
        {
            // Array types may have no instance-names
            if( it->findMember("") )
            {
                *_os << cxxtools::String::widen( it->name() ) << cxxtools::String(L" = ");
                *_os << cxxtools::String::widen( it->typeName() ) << cxxtools::String(L"{ ");
                this->writeParent( *it, "");
                *_os << cxxtools::String(L" }") << std::endl;
                continue;
            }

            //this->writeSection( subdata->name() );
            this->writeParent( *it, it->name() );
        }
    }
}


void SettingsWriter::writeParent(const cxxtools::SerializationInfo& sd, const std::string& prefix)
{
    cxxtools::SerializationInfo::ConstIterator it;
    for(it = sd.begin(); it != sd.end(); ++it)
    {
        if( it->category() == cxxtools::SerializationInfo::Value )
        {
            *_os << cxxtools::String::widen( prefix ) << '.';
            this->writeEntry( it->name(), it->toValue<String>(), it->typeName() );
            *_os << std::endl;
        }
        else if( it->category() == cxxtools::SerializationInfo::Object )
        {
            *_os << cxxtools::String::widen( prefix ) << '.' << cxxtools::String::widen( it->name() ) << cxxtools::String(L" = ");
            *_os<< cxxtools::String::widen( it->typeName() ) << cxxtools::String(L"{ ");
            this->writeChild(*it);
            *_os << cxxtools::String(L" }") << std::endl;
        }
    }
}


void SettingsWriter::writeChild(const cxxtools::SerializationInfo& sd)
{
    bool separate = false;

    cxxtools::SerializationInfo::ConstIterator it;
    for(it = sd.begin(); it != sd.end(); ++it)
    {
        if(separate)
            *_os << cxxtools::String(L", ");

        if( it->category() == cxxtools::SerializationInfo::Value )
        {
            this->writeEntry( it->name(), it->toValue<String>(), it->typeName() );
        }
        else if( it->category() == cxxtools::SerializationInfo::Object ||
                 it->category() == cxxtools::SerializationInfo::Array)
        {
            if(it->name().empty() == false && sd.category() != cxxtools::SerializationInfo::Array)
                *_os << cxxtools::String::widen( it->name() ) << cxxtools::String(L" = ");

            *_os << cxxtools::String::widen( it->typeName() ) << cxxtools::String(L"{ ");
            this->writeChild(*it);
            *_os << cxxtools::String(L" }");
        }

        separate = true;
    }
}


void writeEscapedValue(std::basic_ostream<cxxtools::Char>& os, const cxxtools::String& value)
{
    for(size_t n = 0; n < value.size(); ++n)
    {
        switch( value[n].value() )
        {
            case '\\':
                os << cxxtools::Char('\\');

            default:
                os << value[n];
        }
    }
}


void SettingsWriter::writeEntry(const std::string& name, const cxxtools::String& value, const std::string& type)
{
    if( type.empty() )
    {
        if( name.empty() == false)
            *_os << cxxtools::String::widen(name) << cxxtools::String(L"=");

        *_os  << cxxtools::String(L"\"");
        writeEscapedValue(*_os, value);
        *_os << cxxtools::String(L"\"");

        return;
    }

    if( name.empty() == false)
        *_os << cxxtools::String::widen(name) << cxxtools::String(L" = ");

    *_os << cxxtools::String::widen(type) << cxxtools::String(L"(\"");
    writeEscapedValue(*_os, value);
    *_os << cxxtools::String(L"\")");
}


void SettingsWriter::writeSection(const cxxtools::String& prefix)
{
    *_os << cxxtools::String(L"[") << prefix << cxxtools::String(L"]") << std::endl;
}

}
