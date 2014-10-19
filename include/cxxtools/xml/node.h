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
#ifndef cxxtools_Xml_Node_h
#define cxxtools_Xml_Node_h

namespace cxxtools {

    namespace xml {

        /**
         * @brief The super-class for all specific Node type of an XML document.
         *
         * A Node may for example be a opening tag, a closing tag, a comment or a doctype declaration.
         * The supported node types are contained in the enum Type. To determine the type of a Node the
         * method type() can be used.
         *
         * For every supported node type (except "Unknown") a specialized class exists that is derived
         * from this Node class. Those classes contain more data and access methods to allow the user
         * to determine the information specific to the node, for example the tag name for a StartElement.
         *
         * This class mainly provides the method type() to determine the type of the Node. The user
         * may use this information to determine to which specialized class that is associated
         * with the type this object can be cast; for the Node::StartElement type the Node object can be
         * cast to StartElement, for example.
         *
         * @see Type
         */
        class Node {
            public:
                enum Type {
                    //! Unknown Node type (may not currently be supported)
                    Unknown = 0,
                    //! Xml declaration (see class XmlDeclaration)
                    StartDocument = 1,
                    //! Doctype (see class DocType)
                    DocType = 2,
                    //! End of the document (see EndDocument)
                    EndDocument = 3,
                    //! Start element aka opening tag (see StartElement)
                    StartElement = 4,
                    //! End element aka closing tag (see EndElement)
                    EndElement = 5,
                    //! Parsed content of a tag's body (see Characters)
                    Characters = 6,
                    //! Comment (see Comment)
                    Comment,
                    //! Processing instruction (see ProcessingInstruction)
                    ProcessingInstruction
                };

            public:
                /**
                 * @brief Constructs a new Node object with the specified node type
                 * @see Type
                 */
                Node(Type type)
                : _type(type)
                { }

                //! Empty destructor
                virtual ~Node()
                { }

                /**
                 * @brief Returns the type of this Node that can be used to determine what specific
                 * Node this object is.
                 *
                 * This information may be used to determine to which specialized Node class that is associated
                 * with the type, this Node object can be cast; for the Node::StartElement type the Node object
                 * can be cast to StartElement, for example.
                 *
                 * @return The type of this node.
                 */
                Type type() const
                {return _type;}

                /**
                 * @brief Compares this Node object with the given node.
                 *
                 * The return value of the generic operator== method is always false. Class which derive
                 * from this class should always override this method and provide a useful comparison, for
                 * example by comparing the node type and contents of the current and given Node object
                 *
                 * @param node In subclasses this Node object is compared to the current Node object.
                 * @return In sub-classes $true is returned if this Node object is the same as the given
                 * Node object. In this generic class $false$ is always returned.
                 */
                virtual bool operator==(const Node& node) const
                { return false; }

                virtual Node* clone() const = 0;

            private:
                //! The type of this Node.
                Type _type;
        };

    }

}

#endif
