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
#include "cxxtools/xml/xmlserializer.h"
#include "cxxtools/xml/xmlwriter.h"
#include "cxxtools/xml/startelement.h"
#include "cxxtools/xml/endelement.h"
#include "cxxtools/xml/characters.h"
#include "cxxtools/string.h"
#include "cxxtools/sourceinfo.h"
#include <stdexcept>

namespace cxxtools {

namespace xml {

XmlFormatter::XmlFormatter()
: _writer(0)
, _deleter(0)
{
}

XmlFormatter::XmlFormatter(std::ostream& os)
: _writer( 0 )
, _deleter( new cxxtools::xml::XmlWriter(os) )
{
    _writer = _deleter.get();
}


XmlFormatter::XmlFormatter(cxxtools::xml::XmlWriter* writer)
: _writer(writer)
, _deleter(0)
{
}


XmlFormatter::~XmlFormatter()
{
    this->detach();
}


void XmlFormatter::attach(std::ostream& os)
{
    if (_writer)
        throw std::logic_error("XmlSerizalizer is already open." + CXXTOOLS_SOURCEINFO);

    _deleter.reset(new cxxtools::xml::XmlWriter(os));
    _writer = _deleter.get();
}


void XmlFormatter::attach(cxxtools::xml::XmlWriter& writer)
{
    if (_writer)
        throw std::logic_error("XmlSerizalizer is already open." + CXXTOOLS_SOURCEINFO);

    _deleter.reset(0);
    _writer = &writer;
}


void XmlFormatter::detach()
{
    if (_writer)
    {
        this->flush();
        _deleter.reset(0);
        _writer = 0;
    }
}


void XmlFormatter::flush()
{
    if (_writer)
        _writer->flush();
}


void XmlFormatter::addValue(const std::string& name, const std::string& type,
                             const cxxtools::String& value, const std::string& id)
{
    cxxtools::xml::Attribute attrs[2];
    size_t countAttrs = 0;

    if( ! id.empty() )
        attrs[countAttrs++] = cxxtools::xml::Attribute( cxxtools::String(L"id"), cxxtools::String::widen( id ) );

    if( ! name.empty() )
    {
        if ( ! type.empty() )
            attrs[countAttrs++] = cxxtools::xml::Attribute( cxxtools::String(L"type"), cxxtools::String::widen( type ) );

        _writer->writeElement( cxxtools::String::widen( name ), attrs, countAttrs, value );
    }
    else
    {
        _writer->writeElement( cxxtools::String::widen( type ), attrs, countAttrs, value );
    }
}


void XmlFormatter::addReference(const std::string& name, const cxxtools::String& value)
{
    cxxtools::xml::Attribute attr( cxxtools::String(L"ref"), value );
    _writer->writeElement( cxxtools::String::widen( name ), &attr, 1, cxxtools::String() );
}


void XmlFormatter::beginArray(const std::string& name, const std::string& type,
                              const std::string& id)
{
    if( ! id.empty() )
    {
        Attribute attr( cxxtools::String(L"id"), cxxtools::String::widen( id ) );

        if( ! name.empty() )
            _writer->writeStartElement( cxxtools::String::widen( name ), &attr, 1 );
        else
            _writer->writeStartElement( cxxtools::String::widen( type ), &attr, 1 );
    }
    else
    {
        if( ! name.empty() )
            _writer->writeStartElement( cxxtools::String::widen( name ) );
        else
            _writer->writeStartElement( cxxtools::String::widen( type ) );
    }
}


void XmlFormatter::finishArray()
{
    _writer->writeEndElement();
}


void XmlFormatter::beginObject(const std::string& name, const std::string& type,
                               const std::string& id)
{
    if( ! id.empty() )
    {
        cxxtools::xml::Attribute attr( cxxtools::String(L"id"), cxxtools::String::widen( id ) );

        if( ! name.empty() )
            _writer->writeStartElement( cxxtools::String::widen( name ), &attr, 1 );
        else
            _writer->writeStartElement( cxxtools::String::widen( type ), &attr, 1 );
    }
    else
    {
        if( ! name.empty() )
            _writer->writeStartElement( cxxtools::String::widen( name ) );
        else
            _writer->writeStartElement( cxxtools::String::widen( type ) );
    }
}


void XmlFormatter::beginMember(const std::string& name)
{
}


void XmlFormatter::finishMember()
{
}


void XmlFormatter::finishObject()
{
    _writer->writeEndElement();
}


void XmlFormatter::finish()
{

}




XmlSerializer::XmlSerializer()
{
}


XmlSerializer::XmlSerializer(std::ostream& os)
: _formatter(os)
{
}


XmlSerializer::XmlSerializer(cxxtools::xml::XmlWriter* writer)
: _formatter(writer)
{
}


XmlSerializer::~XmlSerializer()
{
    this->finish();
}


void XmlSerializer::attach(std::ostream& os)
{
    _formatter.attach(os);
}


void XmlSerializer::attach(cxxtools::xml::XmlWriter& writer)
{
    _formatter.attach(writer);
}


void XmlSerializer::detach()
{
    _formatter.detach();
}


void XmlSerializer::flush()
{
    _formatter.flush();
}

} // namespace xml

} // namespace cxxtools
