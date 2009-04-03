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
#include <cxxtools/xmlrpc/scanner.h>
#include <cxxtools/xml/startelement.h>
#include <cxxtools/xml/endelement.h>
#include <cxxtools/xml/characters.h>

namespace cxxtools {

namespace xmlrpc {

bool Scanner::advance(const cxxtools::xml::Node& node)
{
    switch(_state)
    {
        case OnParam:
        { //std::cerr << "OnParam" << std::endl;
            if(node.type() == xml::Node::StartElement) // i4, struct, array...
            {
                const xml::StartElement& se = static_cast<const xml::StartElement&>(node);

                if(se.name() != L"value")
                    throw SerializationError("invalid XML-RPC parameter");

                _state = OnValueBegin;
            }
            else if(node.type() == xml::Node::EndElement)
            {
                throw SerializationError("invalid XML-RPC parameter");
            }

            break;
        }

        case OnValueBegin:
        { //std::cerr << "OnValueBegin" << std::endl;
            if(node.type() == xml::Node::StartElement) // i4, struct, array...
            {
                const xml::StartElement& se = static_cast<const xml::StartElement&>(node);
                //std::cerr << "-> found type" << se.name().narrow() << std::endl;
                if(se.name() == L"struct")
                {
                    _state = OnStructBegin;
                }
                else if(se.name() == L"array")
                {
                    _state = OnArrayBegin;
                }
                else
                {
                    _state = OnScalarBegin;
                }
            }
            else if(node.type() == xml::Node::EndElement)
            {
                throw SerializationError("invalid XML-RPC parameter");
            }

            break;
        }

        case OnValueEnd:
        { //std::cerr << "OnValueEnd" << std::endl;

            if(node.type() == xml::Node::EndElement)
            {
                const xml::EndElement& ee = static_cast<const xml::EndElement&>(node);

                if(ee.name() == L"member")
                { //std::cerr << "OnValueEnd member" << std::endl;
                    _current = _current->leaveMember();
                    _state = OnStructBegin;
                }
                else if(ee.name() == L"data")
                { //std::cerr << "OnValueEnd data" << std::endl;
                    _current = _current->leaveMember();
                    _state = OnDataEnd;
                }
                else if(ee.name() == L"param")
                { //std::cerr << "OnValueEnd data other " << ee.name().narrow() << std::endl;
                    _current->fixup(*_context);
                    _state = OnValueEnd;
                    return true;
                }
                else if(ee.name() == L"fault")
                { //std::cerr << "OnValueEnd data other " << ee.name().narrow() << std::endl;
                    _current->fixup(*_context);
                    _state = OnValueEnd;
                    return true;
                }
                else
                {
                    throw SerializationError("invalid XML-RPC parameter");
                }
            }
            else if(node.type() == xml::Node::StartElement)
            {
                const xml::StartElement& se = static_cast<const xml::StartElement&>(node);
                if(se.name() == L"value")
                { //std::cerr << "OnValueEnd data value" << std::endl;
                    _current = _current->leaveMember();
                    _current = _current->beginMember("");
                    _state = OnValueBegin;
                }
                else
                {
                    throw SerializationError("invalid XML-RPC parameter");
                }
            }

            break;
        }

        case OnStructBegin:
        { //std::cerr << "OnStructBegin" << std::endl;
            if(node.type() == xml::Node::StartElement) // <member>
            {
                const xml::StartElement& se = static_cast<const xml::StartElement&>(node);

                if(se.name() != L"member")
                    throw SerializationError("invalid XML-RPC parameter");

                _state = OnMemberBegin;
            }
            else if(node.type() == xml::Node::EndElement) // </struct>
            {
                _state = OnStructEnd;
            }
            break;
        }

        case OnStructEnd:
        { //std::cerr << "OnStructEnd" << std::endl;
            if(node.type() == xml::Node::EndElement) // </value>
            {
                const xml::EndElement& ee = static_cast<const xml::EndElement&>(node);

                if(ee.name() != L"value")
                    throw SerializationError("invalid XML-RPC parameter");

                _state = OnValueEnd;
            }
            else if(node.type() == xml::Node::StartElement)
            {
                throw SerializationError("invalid XML-RPC parameter");
            }

            break;
        }

        case OnMemberBegin:
        { //std::cerr << "OnMemberBegin" << std::endl;
            if(node.type() == xml::Node::StartElement) // name
            {
                const xml::StartElement& se = static_cast<const xml::StartElement&>(node);

                if(se.name() != L"name")
                    throw SerializationError("invalid XML-RPC parameter");

                _state = OnNameBegin;
            }
            else if(node.type() == xml::Node::EndElement)
            {
                throw SerializationError("invalid XML-RPC parameter");
            }

            break;
        }

        case OnNameBegin:
        { //std::cerr << "OnNameBegin" << std::endl;
            if(node.type() == xml::Node::Characters) // member-name
            {
                const xml::Characters& chars = static_cast<const xml::Characters&>(node);
                const std::string& name = chars.content().narrow();

                _current = _current->beginMember(name);

                _state = OnName;
            }
            else
            {
                throw SerializationError("invalid XML-RPC parameter");
            }

            break;
        }

        case OnName:
        { //std::cerr << "OnName" << std::endl;
            if(node.type() == xml::Node::EndElement) // </name>
            {
                const xml::EndElement& ee = static_cast<const xml::EndElement&>(node);

                if(ee.name() != L"name")
                    throw SerializationError("invalid XML-RPC parameter");

                _state = OnNameEnd;
            }
            else if(node.type() == xml::Node::StartElement)
            {
                throw SerializationError("invalid XML-RPC parameter");
            }

            break;
        }

        case OnNameEnd:
        { //std::cerr << "OnNameEnd" << std::endl;
            if(node.type() == xml::Node::StartElement) // <value>
            {
                const xml::StartElement& se = static_cast<const xml::StartElement&>(node);

                if(se.name() != L"value")
                    throw SerializationError("invalid XML-RPC parameter");

                _state = OnValueBegin;
            }
            else if(node.type() == xml::Node::EndElement)
            {
                throw SerializationError("invalid XML-RPC parameter");
            }

            break;
        }

        case OnScalarBegin:
        { //std::cerr << "OnScalarBegin " << std::endl;
            if(node.type() == xml::Node::Characters)
            {
                const xml::Characters& chars = static_cast<const xml::Characters&>(node);
                _state = OnScalar;
                //std::cerr << "-> found value " << chars.content().narrow() << std::endl;
                _current->setValue( chars.content() );
            }
            else
            {
                throw SerializationError("invalid XML-RPC parameter");
            }

            break;
        }

        case OnScalar:
        { //std::cerr << "OnScalar" << std::endl;
            if(node.type() == xml::Node::EndElement) // </int>, boolean ...
            {
                _state = OnScalarEnd;
            }
            else if(node.type() == xml::Node::StartElement)
            {
                throw SerializationError("invalid XML-RPC parameter");
            }

            break;
        }

        case OnScalarEnd:
        { //std::cerr << "OnScalarEnd" << std::endl;
            if(node.type() == xml::Node::EndElement) // </value>
            {
                const xml::EndElement& ee = static_cast<const xml::EndElement&>(node);

                if(ee.name() != L"value")
                    throw SerializationError("invalid XML-RPC parameter");

                _state = OnValueEnd;
            }
            else if(node.type() == xml::Node::StartElement)
            {
                throw SerializationError("invalid XML-RPC parameter");
            }

            break;
        }

        case OnArrayBegin:
        { //std::cerr << "OnArrayBegin" << std::endl;
            if(node.type() == xml::Node::StartElement) // <data>
            {
                const xml::StartElement& se = static_cast<const xml::StartElement&>(node);

                if(se.name() != L"data")
                    throw SerializationError("invalid XML-RPC parameter");

                _state = OnDataBegin;
            }
            else if(node.type() == xml::Node::EndElement)
            {
                throw SerializationError("invalid XML-RPC parameter");
            }

            break;
        }

        case OnDataBegin:
        { //std::cerr << "OnDataBegin" << std::endl;
            if(node.type() == xml::Node::StartElement) // value
            {
                //std::cerr << _current << std::endl;
                _current = _current->beginMember("");
                _state = OnValueBegin;
            }
            else if(node.type() == xml::Node::EndElement)
            {
                throw SerializationError("invalid XML-RPC parameter");
            }

            break;
        }

        case OnDataEnd:
        { //std::cerr << "OnDataEnd" << std::endl;
            if(node.type() == xml::Node::EndElement) // </array>
            {
                const xml::EndElement& ee = static_cast<const xml::EndElement&>(node);

                if(ee.name() != L"array")
                    throw SerializationError("invalid XML-RPC parameter");

                _state = OnArrayEnd;
            }
            else if(node.type() == xml::Node::StartElement)
            {
                throw SerializationError("invalid XML-RPC parameter");
            }

            break;
        }

        case OnArrayEnd:
        { //std::cerr << "OnArrayEnd" << std::endl;
            if(node.type() == xml::Node::EndElement) // </value>
            {
                const xml::EndElement& ee = static_cast<const xml::EndElement&>(node);

                if(ee.name() != L"value")
                    throw SerializationError("invalid XML-RPC parameter");

                _state = OnValueEnd;
            }
            else if(node.type() == xml::Node::StartElement)
            {
                throw SerializationError("invalid XML-RPC parameter");
            }

            break;
        }
    }

    return false;
}

}

}
