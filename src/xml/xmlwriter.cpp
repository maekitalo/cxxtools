/*
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
#include "cxxtools/xml/xmlwriter.h"
#include "cxxtools/xml/startelement.h"
#include "cxxtools/xml/entityresolver.h"
#include "cxxtools/utf8codec.h"
#include <stdexcept>
#include <iostream>


namespace cxxtools {

namespace xml {

namespace
{
    static const String xmlPrefix(L"<?xml version=\"1.0\" encoding=\"UTF-8\"?>");
}

XmlWriter::XmlWriter()
: _tos(new Utf8Codec)
, _flags(UseXmlDeclaration | UseIndent | UseEndl)
{
}


XmlWriter::XmlWriter(std::ostream& os, int flags)
: _tos(os, new Utf8Codec)
, _flags(flags)
{
    if (useXmlDeclaration())
    {
        _tos << xmlPrefix;
        if (useEndl())
            endl();
    }
}


void XmlWriter::begin(std::ostream& os)
{
    _tos.attach(os);
    if (useXmlDeclaration())
    {
        _tos << xmlPrefix;
        if (useEndl())
            endl();
    }
}


void XmlWriter::writeStartElement(const String& localName)
{
    writeStartElement(localName, 0, 0);
}


void XmlWriter::writeStartElement(const String& localName, const Attribute* attr, size_t attrCount)
{
    if (localName.empty())
        throw std::runtime_error("local name must not be empty in xml writer");

    if (useIndent())
        indent();

    _tos << Char('<') << localName;

    for (size_t n = 0; n < attrCount; ++n)
    {
        if (useEndl())
            endl();

        if (useIndent())
            indent(_elements.size() + 1);
        else
            _tos << Char(' ');

        _tos << attr[n].name() << Char('=') << Char('"');
        writeCharacters(attr[n].value());
        _tos << Char('"');
    }

    _tos << Char('>');

    if (useEndl())
        endl();

    _elements.push(localName);
}


void XmlWriter::writeEndElement()
{
    if( _elements.empty() )
        return;

    if (useIndent())
        indent(_elements.size() - 1);

    _tos << Char(L'<') << Char(L'/') << _elements.top() << Char(L'>');

    if (useEndl())
        endl();

    _elements.pop();
}


void XmlWriter::writeElement(const String& localName, const String& content)
{
    writeElement(localName, 0, 0, content);
}


void XmlWriter::writeElement(const String& localName, const Attribute* attr, size_t attrCount, const String& content)
{
    if (useIndent())
        indent();

    _tos << Char(L'<') << localName;

    for (size_t n = 0; n < attrCount; ++n)
    {
        if (useEndl())
            endl();

        if (useIndent())
            indent(_elements.size() + 1);
        else
            _tos << Char(' ');

        _tos << attr[n].name() << Char('=') << Char('"');
        writeCharacters(attr[n].value());
        _tos << Char('"');
    }

    _tos << Char('>');

    writeCharacters(content);
    _tos << Char('<') << Char('/') << localName << Char('>');

    if (useEndl())
        endl();
}


void XmlWriter::writeCharacters(const String& text)
{
    static EntityResolver resolver;

    String::const_iterator it;
    for (it = text.begin(); it != text.end(); ++it)
        resolver.getEntity(_tos, *it);
}


void XmlWriter::flush()
{
    _tos.flush();
}


void XmlWriter::endl()
{
    _tos << Char('\n');
}

void XmlWriter::indent(size_t size)
{
    for (size_t n = 0; n < size; ++n)
        _tos << Char(' ') << Char(' ');
}

void XmlWriter::Element::writeContent(const String& text)
{
    if (_writer.useIndent())
        _writer.indent();

    _writer.writeCharacters(text);
    if (_writer.useEndl())
        _writer.endl();
}

} // namespace xml

} // namespace cxxtools
