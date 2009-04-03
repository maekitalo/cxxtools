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
#include "cxxtools/xmlrpc/remoteprocedure.h"
#include "cxxtools/xml/xmlerror.h"
#include "cxxtools/xml/startelement.h"
#include "cxxtools/xml/characters.h"
#include "cxxtools/xml/endelement.h"
#include "cxxtools/selector.h"
#include "cxxtools/utf8codec.h"

namespace cxxtools {

namespace xmlrpc {

Client::Client(SelectorBase& selector, const std::string& server,
                             unsigned short port, const std::string& url)
: _state(OnBegin)
, _url(url)
, _client(server, port)
, _request(url)
, _ts( new Utf8Codec )
, _reader(_ts)
, _formatter(_writer)
, _method(0)
{
    _client.setSelector(selector);
    connect(_client.headerReceived, *this, &Client::onReplyHeader);
    connect(_client.bodyAvailable, *this, &Client::onReplyBody);
    connect(_client.replyFinished, *this, &Client::onReplyFinished);

    _formatter.addAlias("bool", "boolean");
}


Client::Client(const std::string& server, unsigned short port, const std::string& url)
: _state(OnBegin)
, _url(url)
, _client(server, port)
, _request(url)
, _ts( new Utf8Codec )
, _reader(_ts)
, _formatter(_writer)
, _method(0)
{
    connect(_client.headerReceived, *this, &Client::onReplyHeader);
    connect(_client.bodyAvailable, *this, &Client::onReplyBody);

    _formatter.addAlias("bool", "boolean");
}


Client::~Client()
{
}


void Client::beginCall(IDeserializer& r, IRemoteProcedure& method, ISerializer** argv, unsigned argc)
{
    _method = &method;
    _state = OnBegin;

    this->prepareRequest(method.name(), argv, argc);
    _client.beginExecute(_request);
    _scanner.begin(r,_context);
}


void Client::call(IDeserializer& r, IRemoteProcedure& method, ISerializer** argv, unsigned argc)
{
    _method = &method;
    _state = OnBegin;

    this->prepareRequest(method.name(), argv, argc);
    net::HttpReplyHeader header = _client.execute(_request);

    std::string body = _client.readBody();
    std::istringstream is(body);
    _ts.attach(is);
    _scanner.begin(r, _context);

    while( _reader.get().type() !=  cxxtools::xml::Node::EndDocument )
    {
        const cxxtools::xml::Node& node = _reader.get();
        this->advance(node);
        _reader.next();
    }

    _ts.detach();
    _state = OnBegin;

    // let xml::ParseError SerializationError, ConversionError propagate

    if( _state == OnFaultResponseEnd )
        throw _fault;

    // _method contains a valid return value now
}


void Client::onReplyHeader(net::HttpClient& client)
{
    _fault.clear();
    _ts.attach( client.in() );
}


std::size_t Client::onReplyBody(net::HttpClient& client)
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
        _fault.setRc(1);
        _fault.setText( error.what() );
        throw;
    }
    catch(const SerializationError& error)
    {
        _fault.setRc(2);
        _fault.setText( error.what() );
        throw;
    }
    catch(const ConversionError& error)
    {
        _fault.setRc(3);
        _fault.setText( error.what() );
        throw;
    }

    return n;
}


void Client::onReplyFinished(net::HttpClient& client)
{
    if(_state == OnMethodResponseEnd)
    {
        _method->onFinished();
    }
    else if(_state == OnFaultResponseEnd || _fault.rc() != 0)
    {
        _method->fault(_fault);
    }
}


void Client::prepareRequest(const std::string& name, ISerializer** argv, unsigned argc)
{
    _request.clear();
    _request.url(_url);
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


void Client::advance(const cxxtools::xml::Node& node)
{
    switch(_state)
    {
        case OnBegin:
        { //std::cerr << "Client:: OnBegin" << std::endl;
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
        { //std::cerr << "Client:: OnMethodResponseBegin" << std::endl;
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
        { //std::cerr << "Client:: OnFaultBegin" << std::endl;
            bool finished = _scanner.advance(node); // start with <value>
            if(finished)
            {
                // </fault>
                _state = OnFaultEnd;
            }

            break;
        }

        case OnFaultEnd:
        { //std::cerr << "Client:: OnFaultEnd" << std::endl;
            if(node.type() == xml::Node::EndElement) // </methodResponse>
            {
                const xml::EndElement& ee = static_cast<const xml::EndElement&>(node);
                if( ee.name() != L"methodResponse" )
                    throw SerializationError("invalid XML-RPC methodCall");

                _state = OnFaultResponseEnd;
            }
            break;
        }

        case OnFaultResponseEnd:
        { //std::cerr << "Client:: OnFaultEnd" << std::endl;
            _state = OnFaultResponseEnd;
            break;
        }

        case OnParamsBegin:
        { //std::cerr << "Client:: OnParams" << std::endl;
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
        { //std::cerr << "Client:: OnParam" << std::endl;
            bool finished = _scanner.advance(node); // start with <value>
            if(finished)
            {
                // </param>
                _state = OnParamEnd;
            }

            break;
        }

        case OnParamEnd:
        { //std::cerr << "Client:: OnParamsEnd" << std::endl;
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
        { //std::cerr << "Client:: OnParamsEnd" << std::endl;
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
        { //std::cerr << "Client:: OnMethodResponseEnd" << std::endl;
            _state = OnMethodResponseEnd;
            break;
        }
    }
}

}

}
