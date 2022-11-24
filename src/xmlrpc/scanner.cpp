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
#include <cxxtools/serializationinfo.h>
#include <cxxtools/serializationerror.h>
#include <cxxtools/deserializer.h>
#include <cxxtools/composer.h>

namespace cxxtools
{

namespace xmlrpc
{

namespace
{
    void throwSerializationError(const char* msg = "invalid XML-RPC parameter")
    {
        SerializationError::doThrow(msg);
    }
}

void Scanner::begin(Deserializer& handler, IComposer& composer)
{
    _state = OnParam;
    _deserializer = &handler;
    _composer = &composer;
    _deserializer->begin();
}

bool Scanner::advance(cxxtools::xml::Node& node)
{
    switch(_state)
    {
        case OnParam:
        {
            if(node.type() == xml::Node::StartElement) // value
            {
                const xml::StartElement& se = static_cast<const xml::StartElement&>(node);

                if(se.name() != L"value")
                    throwSerializationError();

                _state = OnValueBegin;
            }
            else if(node.type() == xml::Node::EndElement)
            {
                throwSerializationError();
            }

            break;
        }

        case OnValueBegin:
        {
            if(node.type() == xml::Node::StartElement) // i4, struct, array...
            {
                const xml::StartElement& se = static_cast<const xml::StartElement&>(node);

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

                _value.clear();
                _type = se.name();
            }
            else if(node.type() == xml::Node::Characters)
            {
                // maybe <value>...<type>...</type>...</value>  (case 1)
                //    or <value>...</value>                     (case 2)
                const xml::Characters& chars = static_cast<const xml::Characters&>(node);
                _value = chars.content();
            }
            else if(node.type() == xml::Node::EndElement)
            {
                const xml::EndElement& ee = static_cast<const xml::EndElement&>(node);
                if(ee.name() != L"value")
                    throwSerializationError();

                // is always type string
                _deserializer->setValue(std::move(_value));
                _value.clear();

                _state = OnValueEnd;
            }
            else
            {
                throwSerializationError();
            }

            break;
        }

        case OnValueEnd:
        {
            if(node.type() == xml::Node::EndElement)
            {
                const xml::EndElement& ee = static_cast<const xml::EndElement&>(node);

                if(ee.name() == L"member")
                {
                    _deserializer->leaveMember();
                    _state = OnStructBegin;
                }
                else if(ee.name() == L"data")
                {
                    _deserializer->leaveMember();
                    _state = OnDataEnd;
                }
                else if(ee.name() == L"param"
                     || ee.name() == L"fault")
                {
                    _composer->fixup(_deserializer->si());
                    return true;
                }
                else
                {
                    throwSerializationError();
                }
            }
            else if(node.type() == xml::Node::StartElement)
            {
                const xml::StartElement& se = static_cast<const xml::StartElement&>(node);
                if(se.name() == L"value")
                {
                    _deserializer->leaveMember();
                    _deserializer->beginMember(std::string(), _type.narrow(), SerializationInfo::Value);
                    _state = OnValueBegin;
                }
                else
                {
                    throwSerializationError();
                }
            }

            break;
        }

        case OnStructBegin:
        {
            if(node.type() == xml::Node::StartElement) // <member>
            {
                const xml::StartElement& se = static_cast<const xml::StartElement&>(node);

                if(se.name() != L"member")
                    throwSerializationError();

                _state = OnMemberBegin;
            }
            else if(node.type() == xml::Node::EndElement) // </struct>
            {
                _state = OnStructEnd;
            }
            break;
        }

        case OnStructEnd:
        {
            if(node.type() == xml::Node::EndElement) // </value>
            {
                const xml::EndElement& ee = static_cast<const xml::EndElement&>(node);

                if(ee.name() != L"value")
                    throwSerializationError();

                _state = OnValueEnd;
            }
            else if(node.type() == xml::Node::StartElement)
            {
                throwSerializationError();
            }

            break;
        }

        case OnMemberBegin:
        {
            if(node.type() == xml::Node::StartElement) // name
            {
                const xml::StartElement& se = static_cast<const xml::StartElement&>(node);

                if(se.name() != L"name")
                    throwSerializationError();

                _state = OnNameBegin;
            }
            else if(node.type() == xml::Node::EndElement)
            {
                throwSerializationError();
            }

            break;
        }

        case OnNameBegin:
        {
            if(node.type() == xml::Node::Characters) // member-name
            {
                const xml::Characters& chars = static_cast<const xml::Characters&>(node);
                const std::string& name = chars.content().narrow();

                _deserializer->beginMember(name, std::string(), SerializationInfo::Object);

                _state = OnName;
            }
            else
            {
                throwSerializationError();
            }

            break;
        }

        case OnName:
        {
            if(node.type() == xml::Node::EndElement) // </name>
            {
                const xml::EndElement& ee = static_cast<const xml::EndElement&>(node);

                if(ee.name() != L"name")
                    throwSerializationError();

                _state = OnNameEnd;
            }
            else if(node.type() == xml::Node::StartElement)
            {
                throwSerializationError();
            }

            break;
        }

        case OnNameEnd:
        {
            if(node.type() == xml::Node::StartElement) // <value>
            {
                const xml::StartElement& se = static_cast<const xml::StartElement&>(node);

                if(se.name() != L"value")
                    throwSerializationError();

                _state = OnValueBegin;
            }
            else if(node.type() == xml::Node::EndElement)
            {
                throwSerializationError();
            }

            break;
        }

        case OnScalarBegin:
        {
            if(node.type() == xml::Node::Characters)
            {
                xml::Characters& chars = static_cast<xml::Characters&>(node);
                _state = OnScalar;

                _deserializer->setValue( chars.content() );
            }
            else if(node.type() == xml::Node::EndElement) // no content, for example empty strings
            {
               
                _deserializer->setValue( cxxtools::String() );
                _state = OnScalarEnd;
            }
            else
            {
                throwSerializationError();
            }

            break;
        }

        case OnScalar:
        {
            if(node.type() == xml::Node::EndElement) // </int>, boolean ...
            {
                _state = OnScalarEnd;
            }
            else if(node.type() == xml::Node::StartElement)
            {
                throwSerializationError();
            }

            break;
        }

        case OnScalarEnd:
        {
            if(node.type() == xml::Node::EndElement) // </value>
            {
                const xml::EndElement& ee = static_cast<const xml::EndElement&>(node);

                if(ee.name() != L"value")
                    throwSerializationError();

                _state = OnValueEnd;
            }
            else if(node.type() == xml::Node::StartElement)
            {
                throwSerializationError();
            }

            break;
        }

        case OnArrayBegin:
        {
            if(node.type() == xml::Node::StartElement) // <data>
            {
                const xml::StartElement& se = static_cast<const xml::StartElement&>(node);

                if(se.name() != L"data")
                    throwSerializationError();

                _state = OnDataBegin;
            }
            else if(node.type() == xml::Node::EndElement)
            {
                throwSerializationError();
            }

            break;
        }

        case OnDataBegin:
        {
            if(node.type() == xml::Node::StartElement) // value
            {
                _deserializer->beginMember(std::string(), std::string(), SerializationInfo::Array);
                _state = OnValueBegin;
            }
            else if(node.type() == xml::Node::EndElement) // empty array
            {
                const xml::EndElement& ee = static_cast<const xml::EndElement&>(node);
                if(ee.name() != L"data")
                    throwSerializationError();

                _state = OnDataEnd;
            }

            break;
        }

        case OnDataEnd:
        {
            if(node.type() == xml::Node::EndElement) // </array>
            {
                const xml::EndElement& ee = static_cast<const xml::EndElement&>(node);

                if(ee.name() != L"array")
                    throwSerializationError();

                _state = OnArrayEnd;
            }
            else if(node.type() == xml::Node::StartElement)
            {
                throwSerializationError();
            }

            break;
        }

        case OnArrayEnd:
        {
            if(node.type() == xml::Node::EndElement) // </value>
            {
                const xml::EndElement& ee = static_cast<const xml::EndElement&>(node);

                if(ee.name() != L"value")
                    throwSerializationError();

                _state = OnValueEnd;
            }
            else if(node.type() == xml::Node::StartElement)
            {
                throwSerializationError();
            }

            break;
        }
    }

    return false;
}

}

}
