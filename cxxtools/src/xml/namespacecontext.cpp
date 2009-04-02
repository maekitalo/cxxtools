/*
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
#include "cxxtools/xml/namespacecontext.h"
#include "cxxtools/string.h"

namespace cxxtools {

namespace xml {

NamespaceContext::NamespaceContext()
{
}


NamespaceContext::~NamespaceContext()
{
}


const String& NamespaceContext::namespaceUri(const String& prefix) const
{
    static const String null;

    std::multimap<String, Namespace>::const_iterator it;
    for( it = _namespaceScopes.begin(); it != _namespaceScopes.end(); ++it) {
        if(it->second.prefix() == prefix) {
            return it->second.namespaceUri();
        }
    }

    return null;
}


const String& NamespaceContext::prefix(const String& namespaceUri) const
{
    static const String null;

    std::multimap<String, Namespace>::const_iterator it;
    for( it = _namespaceScopes.begin(); it != _namespaceScopes.end(); ++it) {
        if(it->second.namespaceUri() == namespaceUri) {
            return it->second.prefix();
        }
    }

    return null;
}


void NamespaceContext::addNamespace(const String& elementName, const Namespace& ns)
{
    ScopeMap::value_type elem(elementName, ns);
    _namespaceScopes.insert(elem);
}


void NamespaceContext::removeNamespace(const String& elementName)
{
    _namespaceScopes.erase(elementName);
}

} // namespace xml

} // namespace cxxtools

