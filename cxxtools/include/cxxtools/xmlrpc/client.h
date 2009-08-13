/*
 * Copyright (C) 2009 by Dr. Marc Boris Duerner
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
#ifndef cxxtools_xmlrpc_Client_h
#define cxxtools_xmlrpc_Client_h

#include <cxxtools/xmlrpc/api.h>
#include <cxxtools/xmlrpc/fault.h>
#include <cxxtools/xmlrpc/formatter.h>
#include <cxxtools/xmlrpc/scanner.h>
#include <cxxtools/xml/xmlreader.h>
#include <cxxtools/xml/xmlwriter.h>
#include <cxxtools/http/client.h>
#include <cxxtools/http/request.h>
#include <cxxtools/deserializer.h>
#include <cxxtools/serializer.h>
#include <cxxtools/connectable.h>
#include <cxxtools/textstream.h>
#include <string>
#include <sstream>
#include <cstddef>

namespace cxxtools {

class SelectorBase;

namespace xmlrpc {

class IRemoteProcedure;


class CXXTOOLS_XMLRPC_API Client : public cxxtools::Connectable
{
    enum State
    {
        OnBegin,
        OnMethodResponseBegin,
        OnFaultBegin,
        OnFaultEnd,
        OnFaultResponseEnd,
        OnParamsBegin,
        OnParam,
        OnParamEnd,
        OnParamsEnd,
        OnMethodResponseEnd
    };

    public:
        Client();

        Client(SelectorBase& selector, const std::string& addr,
               unsigned short port, const std::string& url);

        Client(const std::string& addr, unsigned short port, const std::string& url);

        virtual ~Client();

        void connect(const std::string& addr, unsigned short port,
                     const std::string& url)
        {
            _client.connect(addr, port);
            _request.url(url);
        }

        void auth(const std::string& username, const std::string& password)
        {
            _client.auth(username, password);
        }

        void clearAuth()
        {
            _client.clearAuth();
        }

        void beginCall(IDeserializer& r, IRemoteProcedure& method, ISerializer** argv, unsigned argc);

        void call(IDeserializer& r, IRemoteProcedure& method, ISerializer** argv, unsigned argc);

        std::size_t timeout() const  { return _timeout; }

        void timeout(std::size_t t)  { _timeout = t; }

        std::string url() const
        {
            std::ostringstream s;
            s << "http://"
              << _client.server()
              << ':'
              << _client.port()
              << _request.url();

            return s.str();
        }

    protected:
        void onReplyHeader(http::Client& client);

        std::size_t onReplyBody(http::Client& client);

        void onReplyFinished(http::Client& client);

        void onErrorOccured(http::Client& client, const std::exception& e);

        void prepareRequest(const std::string& name, ISerializer** argv, unsigned argc);

        void advance(const xml::Node& node);

    private:
        State _state;
        http::Client _client;
        http::Request _request;
        TextIStream _ts;
        xml::XmlReader _reader;
        xml::XmlWriter _writer;
        Formatter _formatter;
        Scanner _scanner;
        IRemoteProcedure* _method;
        DeserializationContext _context;
        Fault _fault;
        Deserializer<Fault> _fh;
        std::size_t _timeout;
};

}

}

#endif
