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
#include "cxxtools/xmlrpc/fault.h"
#include "cxxtools/xml/xmlerror.h"
#include "cxxtools/xml/startelement.h"
#include "cxxtools/xml/characters.h"
#include "cxxtools/xml/endelement.h"
#include "cxxtools/http/reply.h"
#include "cxxtools/utf8codec.h"

namespace cxxtools {

namespace xmlrpc {

XmlRpcResponder::XmlRpcResponder(Service& service)
: http::Responder(service)
, _state(OnBegin)
, _ts(new Utf8Codec)
, _reader(_ts)
, _formatter(_writer)
, _service(&service)
, _proc(0)
, _args(0)
{
    _formatter.addAlias("bool", "boolean");
}


XmlRpcResponder::~XmlRpcResponder()
{
    if(_proc)
        _service->releaseProcedure(_proc);
}


void XmlRpcResponder::beginRequest(std::istream& is, http::Request& request)
{
    _fault.clear();
    _state = OnBegin;
    _ts.attach( is );
    _args = 0;
}


std::size_t XmlRpcResponder::readBody(std::istream& is)
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
                const xml::Node& node = _reader.get();
                this->advance(node);
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


void XmlRpcResponder::replyError(std::ostream& os, http::Request& request,
                                     http::Reply& reply, const std::exception& ex)
{
    reply.setHeader("Content-Type", "text/xml");

    os << "<?xml version=\"1.0\"?>\n"
          "<methodResponse>\n"
          "<fault>\n"
          "<value>\n"
          "<struct>\n"
          "<member>\n"
          "<name>faultCode</name>\n"
          "<value><int>" << _fault.rc() << "</int></value>\n"
          "</member>\n"
          "<member>\n"
          "<name>faultString</name>\n"
          "<value><string>" << _fault.what() << "</string></value>\n"
          "</member>\n"
          "</struct>\n"
          "</value>\n"
          "</fault>\n"
          "</methodResponse>\n";
}


void XmlRpcResponder::reply(std::ostream& os, http::Request& request, http::Reply& reply)
{
    if( ! _proc )
        throw std::runtime_error("invalid XML-RPC, no method found");

    if( _args )
    {
        ++_args;
        if( * _args )
            throw std::runtime_error("invalid XML-RPC, missing arguments");
    }

    reply.setHeader("Content-Type", "text/xml");

    _writer.begin(os);
    _writer.writeStartElement( L"methodResponse" );

    try
    {
        ISerializer* rh = _proc->endCall();

        _writer.writeStartElement( L"params" );
        _writer.writeStartElement( L"param" );
        rh->format(_formatter);
        _writer.writeEndElement(); // param
        _writer.writeEndElement(); // params
        _writer.writeEndElement(); // methodResponse
        _writer.flush();
    }
    catch(const Fault& fault)
    {
        _writer.writeStartElement( L"fault" );
        _writer.writeStartElement( L"value" );
        _writer.writeStartElement( L"struct" );

        _writer.writeStartElement( L"member" );
        _writer.writeElement( L"name", L"faultCode" );
        _writer.writeStartElement( L"value" );
        _writer.writeElement( L"int", cxxtools::convert<cxxtools::String>(fault.rc()) );
        _writer.writeEndElement(); // value
        _writer.writeEndElement(); // member

        _writer.writeStartElement( L"member" );
        _writer.writeElement( L"name", L"faultString" );
        _writer.writeStartElement( L"value" );
        _writer.writeElement( L"string", cxxtools::String::widen(fault.what()) );
        _writer.writeEndElement(); // value
        _writer.writeEndElement(); // member

        _writer.writeEndElement(); // struct
        _writer.writeEndElement(); // value
        _writer.writeEndElement(); // fault
        _writer.writeEndElement(); // methodResponse
        _writer.flush();
    }
}


void XmlRpcResponder::advance(const cxxtools::xml::Node& node)
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

                _proc = _service->getProcedure( chars.content().narrow() );
                if( ! _proc )
                    throw std::runtime_error("no such procedure");

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
                    _args = _proc->beginCall();
                    if( ! *_args)
                        std::runtime_error("too many arguments");
                }
                else
                {
                    //std::cerr << "-> next argument" << std::endl;
                    ++_args;
                    if( ! *_args)
                        std::runtime_error("too many arguments");
                }

                _scanner.begin(**_args, _context);
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
