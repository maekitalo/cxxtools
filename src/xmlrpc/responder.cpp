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
#include "cxxtools/xmlrpc/responder.h"
#include "cxxtools/xmlrpc/service.h"
#include "cxxtools/remoteexception.h"
#include "cxxtools/xml/xmlerror.h"
#include "cxxtools/xml/startelement.h"
#include "cxxtools/xml/characters.h"
#include "cxxtools/xml/endelement.h"
#include "cxxtools/http/reply.h"
#include "cxxtools/utf8codec.h"
#include "cxxtools/convert.h"
#include "cxxtools/log.h"

log_define("cxxtools.xmlrpc.responder")

namespace cxxtools
{

namespace xmlrpc
{

XmlRpcResponder::XmlRpcResponder(Service& service)
: http::Responder(service)
, _state(OnBegin)
, _ts(new Utf8Codec)
, _reader(_ts)
, _formatter(_writer)
, _service(&service)
, _args(0)
{
    _writer.useIndent(false);
    _writer.useEndl(false);

    _formatter.addAlias("bool", "boolean");
}


void XmlRpcResponder::beginRequest(net::TcpSocket& /*socket*/, std::istream& is, http::Request& /*request*/)
{
    _fault.clear();
    _state = OnBegin;
    _ts.attach( is );
    _args = 0;
}


std::size_t XmlRpcResponder::readBody(std::istream& /*is*/)
{
    std::size_t n = 0;

    try
    {
        while(true)
        {
            std::streamsize m = _ts.buffer().import();
            if( ! m)
                break;

            n += m;

            while( _reader.advance() )
            {
                this->advance(_reader.get());
            }
        }
    }
    catch(const xml::XmlError& error)
    {
        _fault.rc(1);
        _fault.text( error.what() );
        throw _fault;
    }
    catch(const SerializationError& error)
    {
        _fault.rc(2);
        _fault.text( error.what() );
        throw _fault;
    }
    catch(const ConversionError& error)
    {
        _fault.rc(3);
        _fault.text( error.what() );
        throw _fault;
    }

    return n;
}


void XmlRpcResponder::replyError(std::ostream& os, http::Request& /*request*/,
                                     http::Reply& reply, const std::exception& ex)
{
    reply.setHeader("Content-Type", "text/xml");

    _writer.begin(os);
    _writer.writeStartElement( L"methodResponse" );
    _writer.writeStartElement( L"fault" );
    _writer.writeStartElement( L"value" );
    _writer.writeStartElement( L"struct" );

    _writer.writeStartElement( L"member" );
    _writer.writeElement( L"name", L"faultCode" );
    _writer.writeStartElement( L"value" );
    _writer.writeElement( L"int", cxxtools::convert<cxxtools::String>(_fault.rc()) );
    _writer.writeEndElement(); // value
    _writer.writeEndElement(); // member

    _writer.writeStartElement( L"member" );
    _writer.writeElement( L"name", L"faultString" );
    _writer.writeStartElement( L"value" );

    const char* msg = (_fault.rc() ? _fault.what() : ex.what());
    _writer.writeElement( L"string", cxxtools::String::widen(msg));

    _writer.writeEndElement(); // value
    _writer.writeEndElement(); // member

    _writer.writeEndElement(); // struct
    _writer.writeEndElement(); // value
    _writer.writeEndElement(); // fault
    _writer.writeEndElement(); // methodResponse
    _writer.flush();
}


void XmlRpcResponder::reply(std::ostream& os, http::Request& request, http::Reply& reply)
{
    try
    {
        if( ! _proc )
        {
            _fault.rc(4);
            _fault.text("invalid XML-RPC");
            throw _fault;
        }

        if (_args && _args->needMore())
        {
            _fault.rc(5);
            _fault.text("invalid XML-RPC, missing arguments");
            throw _fault;
        }

        IDecomposer* rh = _proc->endCall();

        reply.setHeader("Content-Type", "text/xml");

        _writer.begin(os);
        _writer.writeStartElement( L"methodResponse" );
        _writer.writeStartElement( L"params" );
        _writer.writeStartElement( L"param" );
        rh->format(_formatter);
        _writer.writeEndElement(); // param
        _writer.writeEndElement(); // params
        _writer.writeEndElement(); // methodResponse
        _writer.flush();
    }
    catch (const RemoteException& fault)
    {
        _fault.rc(fault.rc());
        _fault.text(fault.text());
        replyError(reply.bodyStream(), request, reply, fault);
    }
    catch (...)
    {
        _writer.flush();
        throw;
    }
}


void XmlRpcResponder::advance(cxxtools::xml::Node& node)
{
    switch(_state)
    {
        case OnBegin:
        { //std::cerr << "OnBegin" << std::endl;
            if(node.type() == xml::Node::StartElement)
            {
                const xml::StartElement& se = static_cast<const xml::StartElement&>(node);
                if( se.name() != L"methodCall" )
                    throw xml::XmlError( "invalid XML-RPC methodCall", _reader.line() );

                _state = OnMethodCallBegin;
            }

            break;
        }

        case OnMethodCallBegin:
        { //std::cerr << "OnMethodCallBegin" << std::endl;
            if(node.type() == xml::Node::StartElement)
            {
                _state = OnMethodNameBegin;
            }
            break;
        }

        case OnMethodNameBegin:
        { //std::cerr << "OnMethodNameBegin" << std::endl;
            if(node.type() == xml::Node::Characters)
            {
                const xml::Characters& chars = static_cast<const xml::Characters&>(node);

                log_info("xmlrpc method <" << chars.content().narrow() << '>');
                _procName = chars.content().narrow();
                _proc = _service->getProcedure(_procName);
                if( ! _proc )
                    throw std::runtime_error("no such procedure \"" + _procName + '"');

                //std::cerr << "-> Found Procedure: " << chars.content().narrow() << std::endl;

                _state = OnMethodName;
            }
            break;
        }

        case OnMethodName:
        { //std::cerr << "OnMethodName" << std::endl;
            if(node.type() == xml::Node::EndElement)
            {
                const xml::EndElement& ee = static_cast<const xml::EndElement&>(node);
                if( ee.name() != L"methodName" )
                    throw std::runtime_error("invalid XML-RPC methodCall");

                _state = OnMethodNameEnd;
            }
            break;
        }

        case OnMethodNameEnd:
        { //std::cerr << "OnMethodNameEnd" << std::endl;
            if(node.type() == xml::Node::StartElement)
            {
                const xml::StartElement& se = static_cast<const xml::StartElement&>(node);
                if( se.name() != L"params" )
                    throw std::runtime_error("invalid XML-RPC methodCall");

                _state = OnParams;
            }
            break;
        }

        case OnParams:
        { //std::cerr << "OnParams" << std::endl;
            if(node.type() == xml::Node::EndElement) // </params>
            {
                const xml::EndElement& ee = static_cast<const xml::EndElement&>(node);
                if( ee.name() != L"params" )
                    throw std::runtime_error("invalid XML-RPC methodCall");

                _state = OnParamsEnd;
                break;
            }

            if(node.type() == xml::Node::StartElement)
            {
                const xml::StartElement& se = static_cast<const xml::StartElement&>(node);
                if( se.name() != L"param" )
                    throw std::runtime_error("invalid XML-RPC methodCall");

                //std::cerr << "-> Found param" << std::endl;
                if( ! _args )
                {
                    //std::cerr << "-> begin call" << std::endl;
                    _args = _proc->beginCall(_procName);
                    if ( ! _args->needMore())
                        throw std::runtime_error("too many arguments");
                }

                IComposer* arg = nullptr;

                if(_args && !_args->needMore())
                    throw std::runtime_error("too many arguments");

                if (_args)
                    arg = _args->get();

                if (!arg)
                    throw std::runtime_error("invalid XML-RPC, too many arguments");

                _scanner.begin(_deserializer, *arg);
                _state = OnParam;
                break;
            }

            break;
        }

        case OnParam:
        { //std::cerr << "S: OnParam" << std::endl;
            bool finished = _scanner.advance(node);
            if(finished)
            {
                //std::cerr << "-> param finished" << std::endl; // node is </param>
                _state = OnParams;
            }

            break;
        }

        case OnParamsEnd:
        { //std::cerr << "OnParamsEnd" << std::endl;
            if(node.type() == xml::Node::EndElement) // </methodCall>
            {
                const xml::EndElement& ee = static_cast<const xml::EndElement&>(node);
                if( ee.name() != L"methodCall" )
                    throw std::runtime_error("invalid XML-RPC methodCall");

                _state = OnMethodCallEnd;
            }
            break;
        }

        case OnMethodCallEnd:
        { //std::cerr << "OnMethodCallEnd" << std::endl;
            if(node.type() == xml::Node::EndDocument)
            {
                _state = OnMethodCallEnd;
            }
            break;
        }
    }
}
}

}
