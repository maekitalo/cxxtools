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
#ifndef cxxtools_xmlrpc_Scanner_h
#define cxxtools_xmlrpc_Scanner_h

#include <cxxtools/string.h>

namespace cxxtools
{

class Deserializer;
class IComposer;

namespace xml
{

class Node;

}

namespace xmlrpc
{

class Scanner
{
    enum State
    {
        OnParam,
        OnValueBegin,
        OnValueEnd,
        //OnParamEnd,

        OnScalarBegin,
        OnScalar,
        OnScalarEnd,

        OnStructBegin,
        OnMemberBegin,
        OnNameBegin,
        OnName,
        OnNameEnd,
        //OnMemberEnd,
        OnStructEnd,

        OnArrayBegin,
        OnDataBegin,
        OnDataEnd,
        OnArrayEnd
    };

    public:
        Scanner()
        : _state(OnParam)
        , _deserializer(0)
        , _composer(0)
        {}

        ~Scanner()
        {}

        void begin(Deserializer& handler, IComposer& composer);

        bool advance(xml::Node& node);

    private:
        State _state;
        Deserializer* _deserializer;
        IComposer* _composer;
        String _value;
        String _type;
};

}

}

#endif
