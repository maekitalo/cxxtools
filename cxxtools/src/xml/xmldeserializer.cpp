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
#include "cxxtools/sourceinfo.h"
#include <stdexcept>

namespace cxxtools {

namespace xml {

XmlDeserializer::XmlDeserializer(cxxtools::xml::XmlReader& reader)
: _reader(&reader)
, _deser(0)
{
}


XmlDeserializer::XmlDeserializer(std::istream& is)
: _reader( 0 )
, _deleter( new cxxtools::xml::XmlReader(is) )
, _deser(0)
{
    _reader = _deleter.get();
}


XmlDeserializer::~XmlDeserializer()
{
    this->finish();
}


void XmlDeserializer::get(cxxtools::IDeserializer* deser)
{
    _deser = deser;

    if(_reader->get().type() != cxxtools::xml::Node::StartElement)
        _reader->nextElement();

    _processNode = &XmlDeserializer::beginDocument;

    _startDepth = _reader->depth();
    for(cxxtools::xml::XmlReader::Iterator it = _reader->current(); it != _reader->end(); ++it)
    {
        (this->*_processNode)(*it);

        if( (it->type() == cxxtools::xml::Node::EndElement) && (_reader->depth() < _startDepth) )
        {
            break;
        }
    }
}


void XmlDeserializer::beginDocument(const cxxtools::xml::Node& node)
{
    switch( node.type() )
    {
        case cxxtools::xml::Node::StartElement:
        {
            _nodeName = static_cast<const cxxtools::xml::StartElement&>(node).name();
            _nodeType = static_cast<const cxxtools::xml::StartElement&>(node).attribute(L"type");
            _nodeCategory = static_cast<const cxxtools::xml::StartElement&>(node).attribute(L"category");
            _deser->setName( _nodeName.narrow() );

            _nodeId = static_cast<const cxxtools::xml::StartElement&>(node).attribute(L"id");
            if( ! _nodeId.empty() )
            {
                _deser->setId( _nodeId.narrow() );
            }

            _processNode = &XmlDeserializer::onRootElement;
            break;
        }
        default:
            throw std::logic_error("Expected start element");
    };
}


void XmlDeserializer::onRootElement(const cxxtools::xml::Node& node)
{
    switch( node.type() )
    {
        case cxxtools::xml::Node::Characters:
        {
            const cxxtools::xml::Characters& chars = static_cast<const cxxtools::xml::Characters&>(node);
            if(cxxtools::String::npos != chars.content().find_first_not_of(L" \t\n\r") )
            {
                /// OLD: throw std::logic_error("Invalid element" + CXXTOOLS_SOURCEINFO);
                _deser->setValue( chars.content() ); /// NEW
                _processNode = &XmlDeserializer::onContent;
            }
            else
            {
                _processNode = &XmlDeserializer::onWhitespace;
            }

            break;
        }
        case cxxtools::xml::Node::StartElement:
        {
            _nodeName = static_cast<const cxxtools::xml::StartElement&>(node).name();
            _nodeType = static_cast<const cxxtools::xml::StartElement&>(node).attribute(L"type");
            _nodeCategory = static_cast<const cxxtools::xml::StartElement&>(node).attribute(L"category");

            _processNode = &XmlDeserializer::onStartElement;
            break;
        }

        default:
            throw std::logic_error("Invalid element");
    };
}


void XmlDeserializer::onStartElement(const cxxtools::xml::Node& node)
{
    switch( node.type() )
    {
        case cxxtools::xml::Node::Characters:
        {
            const cxxtools::xml::Characters& chars = static_cast<const cxxtools::xml::Characters&>(node);
            if(cxxtools::String::npos != chars.content().find_first_not_of(L" \t\n\r") )
            {
                std::string nodeName = _nodeName.narrow();
                std::string nodeType = _nodeType.empty() ? nodeName : _nodeType.narrow();
                _deser = _deser->beginMember(nodeName, nodeType, nodeCategory());
                _deser->setValue( chars.content() );
                _deser = _deser->leaveMember();
                //_current->addValue( _nodeName.narrow(), chars.content() );

                _processNode = &XmlDeserializer::onContent;
            }
            else
            {
                if(_deser == 0)
                    throw std::logic_error("Element outside document tree");

                std::string nodeName = _nodeName.narrow();
                std::string nodeType = _nodeType.empty() ? nodeName : _nodeType.narrow();
                _deser = _deser->beginMember(nodeName, nodeType, nodeCategory());
                //SerializationInfo& added = _current->addMember( _nodeName.narrow() );
                //_current = &added;

                _processNode = &XmlDeserializer::onWhitespace;
            }

            break;
        }
        case cxxtools::xml::Node::StartElement:
        {
            if(_deser == 0)
                throw std::logic_error("Element outside document tree");

            std::string nodeName = _nodeName.narrow();
            std::string nodeType = _nodeType.empty() ? nodeName : _nodeType.narrow();
            _deser = _deser->beginMember(nodeName, nodeType, nodeCategory());
            //SerializationInfo& added = _current->addMember( _nodeName.narrow() );
            //_current = &added;

            _nodeName = static_cast<const cxxtools::xml::StartElement&>(node).name();
            _nodeType = static_cast<const cxxtools::xml::StartElement&>(node).attribute(L"type");
            _nodeCategory = static_cast<const cxxtools::xml::StartElement&>(node).attribute(L"category");
            break;
        }
        case cxxtools::xml::Node::EndElement:
        {
            if( _nodeName != static_cast<const cxxtools::xml::EndElement&>(node).name() )
                throw std::logic_error("Invalid element");

            std::string nodeName = _nodeName.narrow();
            std::string nodeType = _nodeType.empty() ? nodeName : _nodeType.narrow();
            _deser = _deser->beginMember(nodeName, nodeType, nodeCategory());
            _deser->setValue( cxxtools::String() );
            _deser = _deser->leaveMember();
            //_current->addValue( _nodeName.narrow(), cxxtools::String() );

            _processNode = &XmlDeserializer::onEndElement;
            break;
        }
        default:
            throw std::logic_error("Invalid element");
    };
}


void XmlDeserializer::onWhitespace(const cxxtools::xml::Node& node)
{
    switch( node.type() )
    {
        case cxxtools::xml::Node::StartElement:
        {
            _nodeName = static_cast<const cxxtools::xml::StartElement&>(node).name();
            _nodeType = static_cast<const cxxtools::xml::StartElement&>(node).attribute(L"type");
            _nodeCategory = static_cast<const cxxtools::xml::StartElement&>(node).attribute(L"category");
            _processNode = &XmlDeserializer::onStartElement;
            break;
        }
        case cxxtools::xml::Node::EndElement:
        {
            if(_deser == 0)
                throw std::logic_error("Element outside document tree");

            _nodeName = static_cast<const cxxtools::xml::EndElement&>(node).name();

            if(_reader->depth() >= _startDepth)
                _deser = _deser->leaveMember();

            _processNode = &XmlDeserializer::onEndElement;
            break;
        }
        default:
            throw std::logic_error("Expected start element");
    };
}


void XmlDeserializer::onContent(const cxxtools::xml::Node& node)
{
    switch( node.type() )
    {
        case cxxtools::xml::Node::EndElement:
        {
            _processNode = &XmlDeserializer::onEndElement;
            break;
        }
        default:
            throw std::logic_error("Expected end element");
    };
}


void XmlDeserializer::onEndElement(const cxxtools::xml::Node& node)
{
    switch( node.type() )
    {
        case cxxtools::xml::Node::Characters:
        {
            _processNode = &XmlDeserializer::onWhitespace;
            break;
        }
        case cxxtools::xml::Node::StartElement:
        {
            _nodeName = static_cast<const cxxtools::xml::StartElement&>(node).name();
            _nodeType = static_cast<const cxxtools::xml::StartElement&>(node).attribute(L"type");
            _nodeCategory = static_cast<const cxxtools::xml::StartElement&>(node).attribute(L"category");
            _processNode = &XmlDeserializer::onStartElement;
            break;
        }
        case cxxtools::xml::Node::EndElement:
        {
            if(_deser == 0)
                throw std::logic_error("Invalid parent");

            _nodeName = static_cast<const cxxtools::xml::EndElement&>(node).name();

            if(_reader->depth() >= _startDepth)
                _deser = _deser->leaveMember();

            break;
        }
        case cxxtools::xml::Node::EndDocument:
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
