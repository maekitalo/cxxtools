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
#include "cxxtools/xml/xmlformatter.h"
#include "cxxtools/xml/startelement.h"
#include <stdexcept>

namespace cxxtools {

namespace xml {

XmlFormatter::XmlFormatter()
: _writer(0)
, _deleter(0)
, _useAttributes(true)
{
}

XmlFormatter::XmlFormatter(std::ostream& os)
: _writer( 0 )
, _deleter( new XmlWriter(os) )
, _useAttributes(true)
{
    _writer = _deleter.get();
}


XmlFormatter::XmlFormatter(XmlWriter* writer)
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
        throw std::logic_error("XmlSerizalizer is already open");

    _deleter.reset(new XmlWriter(os));
    _writer = _deleter.get();
}


void XmlFormatter::attach(XmlWriter& writer)
{
    if (_writer)
        throw std::logic_error("XmlSerizalizer is already open");

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
    cxxtools::String tag(name.empty() ? type : name);

    Attribute attrs[2];
    size_t countAttrs = 0;

    if (_useAttributes)
    {
        if ( ! name.empty() && ! type.empty() )
        {
            attrs[countAttrs].name() = L"type";
            attrs[countAttrs].value() = type;
            ++countAttrs;
        }

        if( ! id.empty() )
        {
            attrs[countAttrs].name() = L"id";
            attrs[countAttrs].value() = id;
            ++countAttrs;
        }
    }

    _writer->writeElement( tag, attrs, countAttrs, value );
}


void XmlFormatter::beginComplexElement(const std::string& name, const std::string& type,
                              const std::string& id, const String& category)
{
    cxxtools::String tag(name.empty() ? type : name);

    if (tag.empty())
        throw std::logic_error("type name or element name must be set in xml formatter");

    Attribute attrs[3];
    size_t countAttrs = 0;

    if (_useAttributes)
    {
        if ( ! name.empty() && ! type.empty() )
        {
            attrs[countAttrs].name() = L"type";
            attrs[countAttrs].value() = type;
            ++countAttrs;
        }

        if( ! id.empty() )
        {
            attrs[countAttrs].name() = L"id";
            attrs[countAttrs].value() = id;
            ++countAttrs;
        }

        if ( ! category.empty() )
        {
            attrs[countAttrs].name().assign(L"category");
            attrs[countAttrs].value().assign(category);
            ++countAttrs;
        }
    }

    _writer->writeStartElement( tag, attrs, countAttrs );
}

void XmlFormatter::beginArray(const std::string& name, const std::string& type,
                              const std::string& id)
{
    beginComplexElement(name, type, id, L"array");
}


void XmlFormatter::finishArray()
{
    _writer->writeEndElement();
}


void XmlFormatter::beginObject(const std::string& name, const std::string& type,
                               const std::string& id)
{
    beginComplexElement(name, type, id, L"struct");
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


} // namespace xml

} // namespace cxxtools
