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
#ifndef cxxtools_xml_NamespaceContext_h
#define cxxtools_xml_NamespaceContext_h

#include <cxxtools/xml/namespace.h>
#include <cxxtools/string.h>
#include <map>

namespace cxxtools {

namespace xml {

    /**
     * @brief Manages all namespaces which are valid for a specific XML document.
     *
     * Namespaces can be added using the method addNamespace() and can be removed using
     * the method removeNamespace().
     *
     * To get the namespace URI for a prefix the method namespaceUri() can be used. To
     * determine the prefix for a namespace the method prefix() can be used.
     *
     * @see Namespace
     */
    class NamespaceContext {
        public:
            //! Creates a new NamespaceContext object which manages the namespaces of an XML document.
            NamespaceContext()
            { }

            /**
             * @brief Returns the namespace URI of the namespace which has the prefix that is passed to
             * this method.
             *
             * If no namespace with the given prefix exists, an empty String is returned.
             *
             * @param prefix The namespace URI for the namespace with this prefix is returned.
             * @return The namespace URI for the prefix or an empty String if the prefix was not found.
             */
            const String& namespaceUri(const String& prefix) const;

            /**
             * @brief Returns the prefix for the namespace which has the URI that is passed to this method.
             *
             * If no namespace with this URI exists, an empty String is returned.
             *
             * @param namespaceUri The prefix of the namespace with this namespace URI is returned.
             * @return The namespace URI for the prefix or an empty String if the prefix was not found.
             */
            const String& prefix(const String& namespaceUri) const ;

            /**
             * @brief Associates the element name (elementName) with the given namespace (ns).
             *
             * The stored namespace can be retrieved by calling namespaceUri() or prefix(). To
             * remove the association between the element name and namespace again, the method
             * removeNamespace() can be used.
             *
             * @param elementName Associates this element name with the also given namespace (ns).
             * @param ns Associates this namespace with the also given element name (elementName).
             */
            void addNamespace(const String& elementName, const Namespace& ns);

            /**
             * @brief Removes the associates of the given element name (elementName) to the namespace.
             *
             * @param elementName The associates for this element name is removed.
             */
            void removeNamespace(const String& elementName)
            { _namespaceScopes.erase(elementName); }


        private:
            //! Multimap that stores the assocations between an element name and its namespace.
            typedef std::multimap<String, Namespace> ScopeMap;
            std::multimap<String, Namespace> _namespaceScopes;
    };

}

}



#endif
