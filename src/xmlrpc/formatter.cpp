/*
 * Copyright (C) 2009 by Dr. Marc Boris Duerner
 * Copyright (C) 2009 by Tommi Meakitalo
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
#include <cxxtools/xmlrpc/formatter.h>
#include <cxxtools/serializationinfo.h>

namespace cxxtools
{

namespace xmlrpc
{

void Formatter::addValueString(const std::string& /*name*/, const std::string& type,
                         const cxxtools::String& value)
{
    _writer->writeStartElement( L"value" );

    if (type == "string" || type.empty())
    {
        _writer->writeCharacters(value);
    }
    else
    {
        std::map<std::string, std::string>::iterator it = _typemap.find(type);
        if( it != _typemap.end() )
            _writer->writeElement( cxxtools::String::widen(it->second), value );
        else
            _writer->writeElement( cxxtools::String::widen(type), value );
    }

    _writer->writeEndElement();
}


void Formatter::beginArray(const std::string&, const std::string&)
{
    _writer->writeStartElement( L"value" );
    _writer->writeStartElement( L"array" );
    _writer->writeStartElement( L"data" );
}


void Formatter::finishArray()
{
    _writer->writeEndElement();
    _writer->writeEndElement();
    _writer->writeEndElement();
}


void Formatter::beginObject(const std::string& /*name*/, const std::string& /*type*/)
{
    _writer->writeStartElement( L"value" );
    _writer->writeStartElement( L"struct" );
}


void Formatter::beginMember(const std::string& name)
{
    _writer->writeStartElement( L"member" );
    _writer->writeElement( L"name", cxxtools::String::widen(name) );
}


void Formatter::finishMember()
{
    _writer->writeEndElement();
}


void Formatter::finishObject()
{
    _writer->writeEndElement();
    _writer->writeEndElement();
}


void Formatter::finish()
{
    _writer->writeEndElement();
}

}

}
