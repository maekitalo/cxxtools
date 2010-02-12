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

#include "cxxtools/xmlrpc/client.h"
#include "clientimpl.h"
#include "cxxtools/xmlrpc/remoteprocedure.h"
#include "cxxtools/xml/xmlerror.h"
#include "cxxtools/xml/startelement.h"
#include "cxxtools/xml/characters.h"
#include "cxxtools/xml/endelement.h"
#include "cxxtools/http/replyheader.h"
#include "cxxtools/selector.h"
#include "cxxtools/utf8codec.h"
#include "cxxtools/log.h"


log_define("cxxtools.xmlrpc.client.impl")

namespace cxxtools {

namespace xmlrpc {

ClientImpl::ClientImpl()
: _state(OnBegin)
, _ts( new Utf8Codec )
, _reader(_ts)
, _formatter(_writer)
, _method(0)
, _timeout(Selectable::WaitInfinite)
{
    _writer.useIndent(false);
    _writer.useEndl(false);

    _formatter.addAlias("bool", "boolean");
}

ClientImpl::~ClientImpl()
{
}


void ClientImpl::beginCall(IDeserializer& r, IRemoteProcedure& method, ISerializer** argv, unsigned argc)
{
    _method = &method;
    _state = OnBegin;

    prepareRequest(method.name(), argv, argc);

    beginExecute();

    _reader.reset(_ts);
    _scanner.begin(r, _context);
}


void ClientImpl::call(IDeserializer& r, IRemoteProcedure& method, ISerializer** argv, unsigned argc)
{
    _method = &method;
    _state = OnBegin;

    prepareRequest(method.name(), argv, argc);

    std::istringstream is(execute());
    _ts.attach(is);
    _reader.reset(_ts);
    _scanner.begin(r, _context);

    while( _reader.get().type() !=  cxxtools::xml::Node::EndDocument )
    {
        const cxxtools::xml::Node& node = _reader.get();
        advance(node);
        _reader.next();
    }

    // let xml::ParseError SerializationError, ConversionError propagate

    if (_method->failed() )
    {
        _method = 0;
        _state = OnBegin;
        throw _fault;
    }

    _method = 0;
    _state = OnBegin;

    // _method contains a valid return value now
}


const IRemoteProcedure* ClientImpl::activeProcedure() const
{
    return _method;
}

void ClientImpl::cancel()
{
    _method = 0;
}

void ClientImpl::onReadReplyBegin(std::istream& is)
{
    _ts.attach(is);
}

std::size_t ClientImpl::onReadReply()
{
    std::size_t n = 0;

    try
    {
        while(true)
        {
            std::streamsize m = _ts.buffer().import();
            if( ! m )
                break;

            n += m;

            while( _reader.advance() ) // xml::ParseError
            {
                const cxxtools::xml::Node& node = _reader.get();
                advance(node); // SerializationError, ConversionError
            }
        }
    }
    catch(const xml::XmlError& error)
    {
        _method->setFault(Fault::invalidXmlRpc, error.what());
        throw;
    }
    catch(const SerializationError& error)
    {
        _method->setFault(Fault::invalidMethodParameters, error.what());
        throw;
    }
    catch(const ConversionError& error)
    {
        _method->setFault(Fault::invalidMethodParameters, error.what());
        throw;
    }

    return n;
}


void ClientImpl::onReplyFinished()
{
    log_debug("onReplyFinished; method=" << static_cast<void*>(_method));
    IRemoteProcedure* method = _method;
    _method = 0;
    method->onFinished();
}


void ClientImpl::onErrorOccured(const std::exception& e)
{
    log_debug("onErrorOccured; method="  << static_cast<void*>(_method) << ": " << e.what());
    if (_method)
    {
        // TODO do not map local exceptions to cxxtools::xmlrpc::Fault

        if (!_method->failed())
            _method->setFault(Fault::systemError, e.what());

        IRemoteProcedure* method = _method;
        _method = 0;
        method->onFinished();
    }
    else
    {
        throw;
    }
}


void ClientImpl::prepareRequest(const std::string& name, ISerializer** argv, unsigned argc)
{
    _writer.begin( prepareRequest() );
    _writer.writeStartElement( L"methodCall" );
    _writer.writeElement( L"methodName", cxxtools::String::widen(name) );
    _writer.writeStartElement( L"params" );

    for(unsigned n = 0; n < argc; ++n)
    {
        _writer.writeStartElement( L"param" );
        argv[n]->format(_formatter);
        _writer.writeEndElement();
    }

    _writer.writeEndElement();
    _writer.writeEndElement();
    _writer.flush();
}


void ClientImpl::advance(const cxxtools::xml::Node& node)
{
    switch(_state)
    {
        case OnBegin:
        {
            if(node.type() == xml::Node::StartElement)
            {
                const xml::StartElement& se = static_cast<const xml::StartElement&>(node);
                if( se.name() != L"methodResponse" )
                    throw SerializationError("invalid XML-RPC methodCall");

                _state = OnMethodResponseBegin;
            }

            break;
        }

        case OnMethodResponseBegin:
        {
            if(node.type() == xml::Node::StartElement) // <params> or <fault>
            {
                const xml::StartElement& se = static_cast<const xml::StartElement&>(node);
                if( se.name() == L"params")
                {
                    _state = OnParamsBegin;
                    break;
                }

                else if( se.name() == L"fault")
                {
                    _fh.begin(_fault);
                    _scanner.begin(_fh, _context);
                    _state = OnFaultBegin;
                    break;
                }

                throw SerializationError("invalid XML-RPC methodCall");
            }
            break;
        }

        case OnFaultBegin:
        {
            bool finished = _scanner.advance(node); // start with <value>
            if(finished)
            {
                // </fault>
                _state = OnFaultEnd;
            }

            break;
        }

        case OnFaultEnd:
        {
            if(node.type() == xml::Node::EndElement) // </methodResponse>
            {
                const xml::EndElement& ee = static_cast<const xml::EndElement&>(node);
                if( ee.name() != L"methodResponse" )
                    throw SerializationError("invalid XML-RPC methodCall");

                _method->setFault(_fault.rc(), _fault.text());

                _state = OnFaultResponseEnd;
            }
            break;
        }

        case OnFaultResponseEnd:
        {
            _state = OnFaultResponseEnd;
            break;
        }

        case OnParamsBegin:
        {
            if(node.type() == xml::Node::StartElement) // <param>
            {
                const xml::StartElement& se = static_cast<const xml::StartElement&>(node);
                if( se.name() != L"param" )
                    throw SerializationError("invalid XML-RPC methodCall");

                _state = OnParam;
            }

            break;
        }

        case OnParam:
        {
            bool finished = _scanner.advance(node); // start with <value>
            if(finished)
            {
                // </param>
                _state = OnParamEnd;
            }

            break;
        }

        case OnParamEnd:
        {
            if(node.type() == xml::Node::EndElement) // </params>
            {
                const xml::EndElement& ee = static_cast<const xml::EndElement&>(node);
                if( ee.name() != L"params" )
                    throw SerializationError("invalid XML-RPC methodCall");

                _state = OnParamsEnd;
            }
            break;
        }

        case OnParamsEnd:
        {
            if(node.type() == xml::Node::EndElement) // </methodResponse>
            {
                const xml::EndElement& ee = static_cast<const xml::EndElement&>(node);
                if( ee.name() != L"methodResponse" )
                    throw SerializationError("invalid XML-RPC methodCall");

                _state = OnMethodResponseEnd;
            }
            break;
        }

        case OnMethodResponseEnd:
        {
            _state = OnMethodResponseEnd;
            break;
        }
    }
}

}

}
