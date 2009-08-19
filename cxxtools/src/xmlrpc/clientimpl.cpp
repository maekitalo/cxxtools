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

    cxxtools::connect(_client.headerReceived, *this, &ClientImpl::onReplyHeader);
    cxxtools::connect(_client.bodyAvailable, *this, &ClientImpl::onReplyBody);
    cxxtools::connect(_client.replyFinished, *this, &ClientImpl::onReplyFinished);
    cxxtools::connect(_client.errorOccured, *this, &ClientImpl::onErrorOccured);

    _formatter.addAlias("bool", "boolean");
}


ClientImpl::ClientImpl(SelectorBase& selector, const std::string& server,
                             unsigned short port, const std::string& url)
: _state(OnBegin)
, _client(selector, server, port)
, _request(url)
, _ts( new Utf8Codec )
, _reader(_ts)
, _formatter(_writer)
, _method(0)
, _timeout(Selectable::WaitInfinite)
{
    _writer.useIndent(false);
    _writer.useEndl(false);

    cxxtools::connect(_client.headerReceived, *this, &ClientImpl::onReplyHeader);
    cxxtools::connect(_client.bodyAvailable, *this, &ClientImpl::onReplyBody);
    cxxtools::connect(_client.replyFinished, *this, &ClientImpl::onReplyFinished);
    cxxtools::connect(_client.errorOccured, *this, &ClientImpl::onErrorOccured);

    _formatter.addAlias("bool", "boolean");
}


ClientImpl::ClientImpl(const std::string& server, unsigned short port, const std::string& url)
: _state(OnBegin)
, _client(server, port)
, _request(url)
, _ts( new Utf8Codec )
, _reader(_ts)
, _formatter(_writer)
, _method(0)
, _timeout(Selectable::WaitInfinite)
{
    _writer.useIndent(false);
    _writer.useEndl(false);

    cxxtools::connect(_client.headerReceived, *this, &ClientImpl::onReplyHeader);
    cxxtools::connect(_client.bodyAvailable, *this, &ClientImpl::onReplyBody);
    cxxtools::connect(_client.replyFinished, *this, &ClientImpl::onReplyFinished);
    cxxtools::connect(_client.errorOccured, *this, &ClientImpl::onErrorOccured);

    _formatter.addAlias("bool", "boolean");
}


ClientImpl::~ClientImpl()
{
}


void ClientImpl::beginCall(IDeserializer& r, IRemoteProcedure& method, ISerializer** argv, unsigned argc)
{
    _method = &method;
    _state = OnBegin;

    this->prepareRequest(method.name(), argv, argc);
    _client.beginExecute(_request);
    _reader.reset(_ts);
    _scanner.begin(r,_context);
}


void ClientImpl::call(IDeserializer& r, IRemoteProcedure& method, ISerializer** argv, unsigned argc)
{
    _method = &method;
    _state = OnBegin;

    this->prepareRequest(method.name(), argv, argc);
    http::ReplyHeader header = _client.execute(_request, _timeout);

    std::string body;
    _client.readBody(body);
    std::istringstream is(body);
    _ts.attach(is);
    _reader.reset(_ts);
    _scanner.begin(r, _context);

    while( _reader.get().type() !=  cxxtools::xml::Node::EndDocument )
    {
        const cxxtools::xml::Node& node = _reader.get();
        this->advance(node);
        _reader.next();
    }

    _ts.detach();

    // let xml::ParseError SerializationError, ConversionError propagate

    if (_method->failed() )
    {
        _state = OnBegin;
        throw _fault;
    }

    _state = OnBegin;

    // _method contains a valid return value now
}


void ClientImpl::onReplyHeader(http::Client& client)
{
    _ts.attach( client.in() );
}


std::size_t ClientImpl::onReplyBody(http::Client& client)
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
                this->advance(node); // SerializationError, ConversionError
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


void ClientImpl::onErrorOccured(http::Client& client, const std::exception& e)
{
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


void ClientImpl::onReplyFinished(http::Client& client)
{
    IRemoteProcedure* method = _method;
    _method = 0;
    method->onFinished();
}


void ClientImpl::prepareRequest(const std::string& name, ISerializer** argv, unsigned argc)
{
    _request.clear();
    _request.setHeader("Content-Type", "text/xml");

    _writer.begin( _request.body() );
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
