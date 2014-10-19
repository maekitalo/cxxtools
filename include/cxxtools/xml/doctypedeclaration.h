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
#ifndef cxxtools_xml_DocTypeDeclaration_h
#define cxxtools_xml_DocTypeDeclaration_h

#include <cxxtools/xml/node.h>
#include <cxxtools/string.h>


namespace cxxtools {

namespace xml {

    /**
     * @brief A DocType element (Node) of an XML document.
     *
     * A DocType element stores the document type of the document and contains an URI to a
     * file which contains the document type definition.
     *
     * Use content() to get the content of the DocType element.
     *
     * @see Node
     */
    class DocTypeDeclaration : public Node
    {
        public:
            /**
             * @brief Constructs a new DocTypeDeclaration object with the given string as content.
             */
            DocTypeDeclaration()
            : Node(Node::DocType)
            { }

            void clear()
            { _content.clear(); }

            /**
             * @brief Clones this DocTypeDeclaration object by creating a duplicate on the heap and returning it.
             * @return A cloned version of this DocTypeDeclaration object.
             */
            DocTypeDeclaration* clone() const
            { return new DocTypeDeclaration(*this); }

            /**
             * @brief Returns the content of this DocTypeDeclaration object.
             * @return The content of this DocTypeDeclaration object.
             */
            const String& content() const
            { return _content; }

            String& content()
            { return _content; }

            /**
             * @brief Sets the content of this DocTypeDeclaration object.
             * @param content The new content for this DocTypeDeclaration object.
             */
            void setContent(const String& content)
            { _content = content; }

        private:
            //! The content of this DocTypeDeclaration object.
            String _content;
    };

}

}
#endif
