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
#ifndef cxxtools_xml_StartElement_h
#define cxxtools_xml_StartElement_h

#include <cxxtools/xml/node.h>
#include <cxxtools/xml/namespace.h>
#include <cxxtools/xml/namespacecontext.h>
#include <cxxtools/xml/attribute.h>
#include <cxxtools/string.h>

namespace cxxtools {

namespace xml {

    /**
     * @brief A start element (Node) which represents an opening tag of an XML document.
     *
     * A start element is created when the parser reaches a start tag, for example $&lt;a>$.
     * A StartElement object not only stores the name of the tag and its namespace information,
     * but also stores the attributes of the tag. These attributes can be accessed by calling
     * attributes(), attribute() and hasAttribute().
     *
     * Use name() to get the name of the tag which was closed.
     *
     * When parsing $<a>test</a>$ a StartElement, a Character and finally an EndElement node is
     * created. If an empty tag is parsed, like for example $</a>$, a StartElement and an EndElement
     * is created.
     *
     * @see EndElement
     * @see Node
     * @see Attribute
     */
    class StartElement : public Node
    {
        public:
            //! Constructs a new StartElement object with no name and an empty attribute list.
            StartElement()
            : Node(Node::StartElement)
            { }

            /**
             * @brief Constructs a new StartElement object with the given string as tag name.
             *
             * @param name The name of the EndElement object. This is an optional parameter.
             * Default is an empty string.
             */
            StartElement(const String& name)
            : Node(Node::StartElement),
              _name(name)
            { }

            /**
             * @brief Clones this StartElement object by creating a duplicate on the heap and returning it.
             * @return A cloned version of this StartElement object.
             */
            StartElement* clone() const
            {return new StartElement(*this);}


            void clear()
            {
                _name.clear();
                _attributes.clear();
            }

            /**
             * @brief Returns the tag name of the opening tag for which this StartElement object was created.
             *
             * When parsing <a>test</a> a StartElement, a Character and finally an EndElement node is
             * created. The StartElement has the name "a". If an empty tag is parsed, like for example </a>,
             * only a StartElement and an EndElement ("a") is created.
             *
             * @return The tag name of the opening tag for which this StartElement object was created.
             */
            String& name() {return _name;}

            /**
             * @brief Returns the tag name of the opening tag for which this StartElement object was created.
             *
             * When parsing <a>test</a> a StartElement, a Character and finally an EndElement node is
             * created. The StartElement has the name "a". If an empty tag is parsed, like for example </a>,
             * only a StartElement and an EndElement ("a") is created.
             *
             * @return The tag name of the opening tag for which this StartElement object was created.
             */
            const String& name() const
            {return _name;}

            /**
             * @brief Sets the tag name of the end start for which this StartElement object was created.
             * @param name The new name for this StartElement object.
             */
            void setName(const String& name)
            {_name = name;}

            /**
             * @brief Add the given attribute to the attribute list of this start tag.
             *
             * This StartElement object holds a list of attributes, which consist of the attribute name
             * and the attribute value. The attributes can be read using attributes() or attribute().
             *
             * @param attribute The attribute which is added to this object's attribute list.
             */
            void addAttribute(const Attribute& attribute)
            {_attributes.push_back(attribute);}

            /**
             * @brief Returns the attribute list of this StartElement which contains all attributes of the tag.
             *
             * This StartElement object holds a list of attributes, which consist of the attribute name
             * and the attribute value. This method returns all attributes of the represented tag. The list
             * can be iterated using a iterator. To access a specific attribute the method attribute() can be
             * used.
             *
             * @return A list containing all attributes of the tag this StartElement represents.
             */
            const Attributes& attributes() const
            { return _attributes; }

            /**
             * @brief Returns the value of the attribute with the given name.
             *
             * This StartElement object holds a list of attributes, which consist of the attribute name
             * and the attribute value. This methods returns the value of a single attribute. To access
             * all attributes of this StartElement the method attributes() can be used.
             *
             * If no attribute with the given name exists, an empty string is returned.
             *
             * @param attributeName The value of the attribute with this name is returned.
             * @return The value of the request attribute; or an empty string if there is no attribute
             * with this name.
             */
            const String& attribute(const String& attributeName) const;

            /**
             * @return Checks if the StartElement has an attribute with the given name.
             *
             * This method returns $true$ if an attribute with the given name exists in this
             * StartElement. If no attribute with this name exist $false$ is returned.
             *
             * @param attributeName It is checked if an attribute with this attribute name exists.
             * @return $true$ if an attribute with this name exists; $false$ otherwise.
             */
            bool hasAttribute(const String& attributeName) const;

            /**
             * @brief Returns the namespace conText of this StartElement.
             *
             * @return NamespaceContext The namespace conText of this StartElment.
             * @see NamespaceContext
             */
            const NamespaceContext& namespaceContext() const
            {return _namespaceContext;}

            /**
             * @brief Sets the namespace conText for this StartElement.
             *
             * @param conText The new namespace conText for this StartElment.
             * @see NamespaceContext
             */
            void setNamespaceContext(const NamespaceContext& conText)
            {_namespaceContext = conText;}

            /**
             * @brief Returns the namespace uri for the given tag prefix in this StartElments namespace conText.
             *
             * The namespace uri is determined using the method NamespaceContext::namespaceUri().
             * If no namespace uri exists for this prefix an empty string is returned.
             *
             * @param prefix The prefix for which the namespace uri is returned.
             * @return The namespace uri for the given prefix; or an empty string if no namespace uri exists
             * for this prefix.
             */
            const String& namespaceUri(const String& prefix) const
            {return _namespaceContext.namespaceUri(prefix);}

            /**
             * @brief Compares this StartElement object with the given node.
             *
             * This method returns $true$ if the given node also is a StartElement object and
             * the content of both StartElement objects is the same. Otherwise it returns $false$.
             *
             * @param node This Node object is compared to the current StartElement node object.
             * @return $true if this StartElement object is the same as the given node.
             */
            virtual bool operator==(const Node& node) const;

        private:
            //! The name of the underlying tag.
            String _name;

            //! The list which contains all attributes of the underlying tag.
            Attributes _attributes;

            //! The namespace conText of this StartElement.
            NamespaceContext _namespaceContext;
    };

}

}

#endif
