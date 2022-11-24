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
#ifndef cxxtools_xmlrpc_Formatter_h
#define cxxtools_xmlrpc_Formatter_h

#include <cxxtools/xml/xmlwriter.h>
#include <cxxtools/formatter.h>
#include <cxxtools/string.h>
#include <string>
#include <map>

namespace cxxtools {

namespace xmlrpc {

class Formatter : public cxxtools::Formatter
{
    public:
        Formatter(xml::XmlWriter& writer)
        : _writer(&writer)
        { }

        void addAlias(const std::string& type, const std::string& alias)
        { _typemap[type] = alias; }

        void attach(xml::XmlWriter& writer)
        { _writer = &writer; }

        void addValueString(const std::string& name, const std::string& type,
                      cxxtools::String&& value);

        void beginArray(const std::string& name, const std::string& type);

        void finishArray();

        void beginObject(const std::string& name, const std::string& type);

        void beginMember(const std::string& name);

        void finishMember();

        void finishObject();

        void finish();

    private:
        xml::XmlWriter* _writer;
        std::map<std::string, std::string> _typemap;
};

}

}

#endif
