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
#include <cxxtools/log.h>
#include <cxxtools/string.h>

log_define("cxxtools.xmlrpc.scanner")

namespace cxxtools {

namespace xmlrpc {

namespace
{
    void throwSerializationError(const char* msg = "invalid XML-RPC parameter")
    {
        throw SerializationError(msg);
    }
}

bool Scanner::advance(const cxxtools::xml::Node& node)
{
    switch(_state)
    {
        case OnParam:
        {
            log_debug("OnParam");
            if(node.type() == xml::Node::StartElement) // i4, struct, array...
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
            log_debug("OnValueBegin, node type " << node.type());
            if(node.type() == xml::Node::StartElement) // i4, struct, array...
            {
                const xml::StartElement& se = static_cast<const xml::StartElement&>(node);

                log_debug("-> found type " << se.name().narrow());
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
                _current->setValue( _value );
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
            log_debug("OnValueEnd, node type " << node.type());

            if(node.type() == xml::Node::EndElement)
            {
                const xml::EndElement& ee = static_cast<const xml::EndElement&>(node);

                if(ee.name() == L"member")
                {
                    log_debug("OnValueEnd member");
                    _current = _current->leaveMember();
                    _state = OnStructBegin;
                }
                else if(ee.name() == L"data")
                {
                    log_debug("OnValueEnd data");
                    _current = _current->leaveMember();
                    _state = OnDataEnd;
                }
                else if(ee.name() == L"param")
                {
                    log_debug("OnValueEnd data other " << ee.name().narrow());
                    _current->fixup(*_context);
                    _state = OnValueEnd;
                    return true;
                }
                else if(ee.name() == L"fault")
                {
                    log_debug("OnValueEnd data other " << ee.name().narrow());
                    _current->fixup(*_context);
                    _state = OnValueEnd;
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
                    log_debug("OnValueEnd data value");
                    _current = _current->leaveMember();
                    _current = _current->beginMember(std::string());
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
            log_debug("OnStructBegin");
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
            log_debug("OnStructEnd");
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
            log_debug("OnMemberBegin");
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
            log_debug("OnNameBegin");
            if(node.type() == xml::Node::Characters) // member-name
            {
                const xml::Characters& chars = static_cast<const xml::Characters&>(node);
                const std::string& name = chars.content().narrow();

                _current = _current->beginMember(name);

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
            log_debug("OnName");
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
            log_debug("OnNameEnd");
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
            log_debug("OnScalarBegin ");
            if(node.type() == xml::Node::Characters)
            {
                const xml::Characters& chars = static_cast<const xml::Characters&>(node);
                _state = OnScalar;

               log_debug("-> found value " << chars.content().narrow());
                _current->setValue( chars.content() );
            }
            else if(node.type() == xml::Node::EndElement) // no content, for example empty strings
            {
               
               log_debug("-> found empty value ");
                _current->setValue( cxxtools::String() );
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
            log_debug("OnScalar");
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
            log_debug("OnScalarEnd");
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
            log_debug("OnArrayBegin");
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
            log_debug("OnDataBegin");
            if(node.type() == xml::Node::StartElement) // value
            {
                log_debug(_current);
                _current = _current->beginMember("");
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
            log_debug("OnDataEnd");
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
            log_debug("OnArrayEnd");
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
