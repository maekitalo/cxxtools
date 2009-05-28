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
#ifndef cxxtools_xmlrpc_Fault_h
#define cxxtools_xmlrpc_Fault_h

#include <cxxtools/xmlrpc/api.h>
#include <cxxtools/serializationinfo.h>
#include <stdexcept>
#include <string>

namespace cxxtools {

namespace xmlrpc {

class CXXTOOLS_XMLRPC_API Fault : public std::exception
{
    public:
        enum ErrorCodes {
            parseError                  = -32700,
            unsupportedEncoding         = -32701,
            invalidCharacterForEncoding = -32702,
            invalidXmlRpc               = -32600,
            methodNotFound              = -32601,
            invalidMethodParameters     = -32602,
            internalXmlRpcError         = -32603,
            applicationError            = -32500,
            systemError                 = -32400,
            transportError              = -32300
        };

        Fault(const std::string& msg, int rc)
        : _msg(msg)
        , _rc(rc)
        { }

        Fault(const char* msg, int rc);

        Fault()
        : _rc(0)
        { }

        ~Fault() throw()
        { }

        void clear()
        {
            _rc = 0;
            _msg.clear();
        }

        int rc() const
        { return _rc; }

        void setRc(int rc)
        { _rc = rc; }

        void setText(const std::string& msg)
        { _msg = msg; }

        const std::string& text() const
        { return _msg; }

        const char* what() const throw()
        { return _msg.c_str(); }

        operator bool() const
        { return _rc != 0; }

    private:
        std::string _msg;
        int _rc;
};


inline void operator >>=(const cxxtools::SerializationInfo& si, Fault& fault)
{
    fault.setRc( si.getValue<int>("faultCode") );
    fault.setText( si.getValue<std::string>("faultString") );
}


inline void operator <<=(cxxtools::SerializationInfo& si, const Fault& fault)
{
    si.addMember("faultCode") <<= fault.rc();
    si.addMember("faultString") <<= fault.text();
}

}

}

#endif
