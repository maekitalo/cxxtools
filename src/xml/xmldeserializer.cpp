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
#include "cxxtools/xml/xmldeserializer.h"
#include "cxxtools/xml/startelement.h"
#include "cxxtools/xml/endelement.h"
#include "cxxtools/xml/characters.h"
#include "cxxtools/string.h"
#include <stdexcept>

namespace cxxtools {

namespace xml {

XmlDeserializer::XmlDeserializer(XmlReader& reader)
{
    doDeserialize(reader);
}


XmlDeserializer::XmlDeserializer(std::istream& is)
{
    XmlReader reader(is);
    doDeserialize(reader);
}


void XmlDeserializer::doDeserialize(XmlReader& reader)
{
    begin();

    if(reader.get().type() != Node::StartElement)
        reader.nextElement();

    _processNode = &XmlDeserializer::beginDocument;

    _startDepth = reader.depth();
    for(XmlReader::Iterator it = reader.current(); it != reader.end(); ++it)
    {
        (this->*_processNode)(reader);

        if( (it->type() == Node::EndElement) && (reader.depth() < _startDepth) )
        {
            break;
        }
    }
}


void XmlDeserializer::beginDocument(XmlReader& reader)
{
    const Node& node = *reader.current();
    switch( node.type() )
    {
        case Node::StartElement:
        {
            _nodeName = static_cast<const StartElement&>(node).name();
            _nodeType = static_cast<const StartElement&>(node).attribute(L"type");
            _nodeCategory = static_cast<const StartElement&>(node).attribute(L"category");
            setName( _nodeName.narrow() );

            _processNode = &XmlDeserializer::onRootElement;
            break;
        }
        default:
            throw std::logic_error("Expected start element");
    };
}


void XmlDeserializer::onRootElement(XmlReader& reader)
{
    const Node& node = *reader.current();
    switch( node.type() )
    {
        case Node::Characters:
        {
            const Characters& chars = static_cast<const cxxtools::xml::Characters&>(node);
            if(cxxtools::String::npos != chars.content().find_first_not_of(L" \t\n\r") )
            {
                setValue( chars.content() );
                _processNode = &XmlDeserializer::onContent;
            }
            else
            {
                _processNode = &XmlDeserializer::onWhitespace;
            }

            break;
        }
        case Node::StartElement:
        {
            _nodeName = static_cast<const StartElement&>(node).name();
            _nodeType = static_cast<const StartElement&>(node).attribute(L"type");
            _nodeCategory = static_cast<const StartElement&>(node).attribute(L"category");

            _processNode = &XmlDeserializer::onStartElement;
            break;
        }

        default:
            throw std::logic_error("Invalid element");
    };
}


void XmlDeserializer::onStartElement(XmlReader& reader)
{
    const Node& node = *reader.current();
    switch( node.type() )
    {
        case Node::Characters:
        {
            const Characters& chars = static_cast<const cxxtools::xml::Characters&>(node);
            if(cxxtools::String::npos != chars.content().find_first_not_of(L" \t\n\r") )
            {
                std::string nodeName = _nodeName.narrow();
                std::string nodeType = _nodeType.empty() ? nodeName : _nodeType.narrow();
                beginMember(nodeName, nodeType, nodeCategory());
                setValue( chars.content() );
                leaveMember();
                //_current->addValue( _nodeName.narrow(), chars.content() );

                _processNode = &XmlDeserializer::onContent;
            }
            else
            {
                std::string nodeName = _nodeName.narrow();
                std::string nodeType = _nodeType.empty() ? nodeName : _nodeType.narrow();
                beginMember(nodeName, nodeType, nodeCategory());

                _processNode = &XmlDeserializer::onWhitespace;
            }

            break;
        }
        case Node::StartElement:
        {
            std::string nodeName = _nodeName.narrow();
            std::string nodeType = _nodeType.empty() ? nodeName : _nodeType.narrow();
            beginMember(nodeName, nodeType, nodeCategory());
            //SerializationInfo& added = _current->addMember( _nodeName.narrow() );
            //_current = &added;

            _nodeName = static_cast<const StartElement&>(node).name();
            _nodeType = static_cast<const StartElement&>(node).attribute(L"type");
            _nodeCategory = static_cast<const StartElement&>(node).attribute(L"category");
            break;
        }
        case Node::EndElement:
        {
            if( _nodeName != static_cast<const EndElement&>(node).name() )
                throw std::logic_error("Invalid element");

            std::string nodeName = _nodeName.narrow();
            std::string nodeType = _nodeType.empty() ? nodeName : _nodeType.narrow();
            beginMember(nodeName, nodeType, nodeCategory());
            setValue( cxxtools::String() );
            leaveMember();
            //_current->addValue( _nodeName.narrow(), cxxtools::String() );

            _processNode = &XmlDeserializer::onEndElement;
            break;
        }
        default:
            throw std::logic_error("Invalid element");
    };
}


void XmlDeserializer::onWhitespace(XmlReader& reader)
{
    const Node& node = *reader.current();
    switch( node.type() )
    {
        case Node::StartElement:
        {
            _nodeName = static_cast<const StartElement&>(node).name();
            _nodeType = static_cast<const StartElement&>(node).attribute(L"type");
            _nodeCategory = static_cast<const StartElement&>(node).attribute(L"category");
            _processNode = &XmlDeserializer::onStartElement;
            break;
        }
        case Node::EndElement:
        {
            _nodeName = static_cast<const EndElement&>(node).name();

            if(reader.depth() >= _startDepth)
                leaveMember();

            _processNode = &XmlDeserializer::onEndElement;
            break;
        }
        default:
            throw std::logic_error("Expected start element");
    };
}


void XmlDeserializer::onContent(XmlReader& reader)
{
    const Node& node = *reader.current();
    switch( node.type() )
    {
        case Node::EndElement:
        {
            _processNode = &XmlDeserializer::onEndElement;
            break;
        }
        default:
            throw std::logic_error("Expected end element");
    };
}


void XmlDeserializer::onEndElement(XmlReader& reader)
{
    const Node& node = *reader.current();
    switch( node.type() )
    {
        case Node::Characters:
        {
            _processNode = &XmlDeserializer::onWhitespace;
            break;
        }
        case Node::StartElement:
        {
            _nodeName = static_cast<const StartElement&>(node).name();
            _nodeType = static_cast<const StartElement&>(node).attribute(L"type");
            _nodeCategory = static_cast<const StartElement&>(node).attribute(L"category");
            _processNode = &XmlDeserializer::onStartElement;
            break;
        }
        case Node::EndElement:
        {
            _nodeName = static_cast<const EndElement&>(node).name();

            if(reader.depth() >= _startDepth)
                leaveMember();

            break;
        }
        case Node::EndDocument:
        {
            break;
        }
        default:
        {
            throw std::logic_error("Expected start element");
        }
    }

}

SerializationInfo::Category XmlDeserializer::nodeCategory() const
{
    return _nodeCategory == L"array" ? SerializationInfo::Array :
           _nodeCategory == L"struct" || _nodeCategory == L"object" ? SerializationInfo::Object :
           _nodeCategory == L"scalar" || _nodeCategory == L"value" ? SerializationInfo::Value :
           SerializationInfo::Void;
}


} // namespace xml

} // namespace cxxtools
