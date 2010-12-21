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
#ifndef cxxtools_Xml_Namespace_h
#define cxxtools_Xml_Namespace_h

#include <cxxtools/xml/api.h>
#include <cxxtools/string.h>

#include <iosfwd>

namespace cxxtools {

namespace xml {

    /**
     * @brief A Namespace element (Node) of an XML document.
     *
     * A namespace element stores a namespace uri which describes the namespace URI and a locally
     * usable prefix which can be added before a tag name to specify that this particular tag
     * is part of that namespace.
     *
     * Use namespaceUri() to get the namespace URI. Use prefix() to get the prefix.
     *
     * @see Node
     * @see NamespaceContext
     */
    class CXXTOOLS_XML_API Namespace {
        public:
            /**
             * @brief Constructs a new Namespace object with the given namespace URI and prefix.
             *
             * @param namespaceURI The unique URI of this namespace.
             * @param prefix The namespace prefix which can be added to a tag name to specify that
             * this tag belongs to that namespace.
             */
            Namespace(const String& namespaceUri, const String& prefix)
            : _prefix(prefix), _namespaceUri(namespaceUri)
            { }

            /**
             * @brief Returns the prefix of this namespace.
             *
             * The namespace prefix can be added to a tag name to specify that this tag belongs
             * to that namespace.
             *
             * @return The namespace prefix of this Namespace object.
             */
            const String& prefix() const
            { return _prefix; }

            /**
             * @brief Sets the prefix of this namespace.
             *
             * The namespace prefix can be added to a tag name to specify that this tag belongs
             * to that namespace.
             *
             * @param prefix The namespace prefix for this Namespace object.
             */
            void setPrefix(const String& prefix)
            { _prefix = prefix; }

            /**
             * @brief Returns the URI of this namespace.
             *
             * The URI is unique and identifies the namespace.
             *
             * @return The namespace URI of this Namespace object.
             */
            const String& namespaceUri() const
            { return _namespaceUri; }

            /**
             * @brief Sets the URI of this namespace.
             *
             * The URI is unique and identifies the namespace.
             *
             * @param namespaceUri The namespace URI for this Namespace object.
             */
            void setNamespaceUri(const String& namespaceUri)
            { _namespaceUri = namespaceUri; }

            /**
             * @brief Returns $true$ if this is the default namespace in the current XML document. Otherwise
             * $false$ is returned.
             *
             * @return $true$ if this is the default namespace; $false$ otherwise.
             */
            bool isDefaultNamespaceDeclaration();

        private:
            //! The prefix of this namespace.
            String _prefix;

            //! The namespace URI of this namespace.
            String _namespaceUri;
    };

}

}

#endif
