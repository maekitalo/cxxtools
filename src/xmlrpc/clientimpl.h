/*
 * Copyright (C) 2009 by Marc Boris Duerner, Tommi Maekitalo
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
#ifndef cxxtools_xmlrpc_ClientImpl_h
#define cxxtools_xmlrpc_ClientImpl_h

#include <cxxtools/xmlrpc/api.h>
#include <cxxtools/remoteexception.h>
#include <cxxtools/xmlrpc/formatter.h>
#include <cxxtools/xmlrpc/scanner.h>
#include <cxxtools/xml/xmlreader.h>
#include <cxxtools/xml/xmlwriter.h>
#include <cxxtools/composer.h>
#include <cxxtools/decomposer.h>
#include <cxxtools/deserializer.h>
#include <cxxtools/connectable.h>
#include <cxxtools/textstream.h>
#include <string>

namespace cxxtools
{

class IRemoteProcedure;

namespace xmlrpc
{

class ClientImpl : public cxxtools::Connectable
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
        ClientImpl();

        virtual ~ClientImpl();

        void beginCall(IComposer& r, IRemoteProcedure& method, IDecomposer** argv, unsigned argc);

        void endCall();

        void call(IComposer& r, IRemoteProcedure& method, IDecomposer** argv, unsigned argc);

        Milliseconds timeout() const  { return _timeout; }
        void timeout(Milliseconds t)  { _timeout = t; if (!_connectTimeoutSet) _connectTimeout = t; }

        Milliseconds connectTimeout() const  { return _connectTimeout; }
        void connectTimeout(Milliseconds t)  { _connectTimeout = t; _connectTimeoutSet = true; }

        virtual std::string url() const = 0;

        const IRemoteProcedure* activeProcedure() const;

        virtual void cancel();

    protected:
        void onReadReplyBegin(std::istream& is);

        std::size_t onReadReply();

        void onReplyFinished();

        virtual void beginExecute() = 0;

        virtual void endExecute() = 0;

        virtual std::string execute() = 0;

        virtual std::ostream& prepareRequest() = 0;

    protected:
        void prepareRequest(const String& name, IDecomposer** argv, unsigned argc);

        void advance(const xml::Node& node);

        State _state;
        TextIStream _ts;
        xml::XmlReader _reader;
        xml::XmlWriter _writer;
        Formatter _formatter;
        Deserializer _deserializer;
        Scanner _scanner;
        IRemoteProcedure* _method;
        RemoteException _fault;
        Composer<RemoteException> _fh;
        Milliseconds _timeout;
        bool _connectTimeoutSet;  // indicates if connectTimeout is explicitely set
                                  // when not, it follows the setting of _timeout
        Milliseconds _connectTimeout;
        bool _errorPending;
};

}

}

#endif
