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
#ifndef cxxtools_xml_EndElement_h
#define cxxtools_xml_EndElement_h

#include <cxxtools/xml/api.h>
#include <cxxtools/xml/node.h>
#include <cxxtools/string.h>


namespace cxxtools {

    namespace xml {

        /**
         * @brief An end element (Node) which represents a closing tag of an XML document.
         *
         * An end element is created when the parser reaches an end tag, for example $&lt;/a>$.
         * An EndElement object only stores the name of the tag. To access the attributes of the tag the
         * start tag has to be read. The body of the tag can be accessed by reading the previous
         * Character node(s).
         *
         * Use name() to get the name of the tag which was closed.
         *
         * When parsing $<a>test</a>$ a StartElement, a Character and finally an EndElement node is
         * created. If an empty tag is parsed, like for example $</a>$, a StartElement and an EndElement
         * is created.
         *
         * @see StartElement
         * @see Node
         */
        class CXXTOOLS_XML_API EndElement : public Node {
            public:
                /**
                 * @brief Constructs a new EndElement object with the given (optional) string as tag name.
                 *
                 * @param name The name of the EndElement object. This is an optional parameter.
                 * Default is an empty string.
                 */
                EndElement(const String& name = String());

                //! Empty destructor
                ~EndElement();

                /**
                 * @brief Clones this EndElement object by creating a duplicate on the heap and returning it.
                 * @return A cloned version of this EndElement object.
                 */
                EndElement* clone() const
                {return new EndElement(*this);}

                void clear()
                { _name.clear(); }

                /**
                 * @brief Returns the tag name of the closing tag for which this EndElement object was created.
                 *
                 * When parsing <a>test</a> a StartElement, a Character and finally an EndElement node is
                 * created. The EndElement has the name "a". If an empty tag is parsed, like for example </a>,
                 * a StartElement and an EndElement ("a") is created.
                 *
                 * @return The tag name of the closing tag for which this EndElement object was created.
                 */
                String& name();

                /**
                 * @brief Returns the tag name of the closing tag for which this EndElement object was created.
                 *
                 * When parsing <a>test</a> a StartElement, a Character and finally an EndElement node is
                 * created. The EndElement has the name "a". If an empty tag is parsed, like for example </a>,
                 * a StartElement and an EndElement ("a") is created.
                 *
                 * @return The tag name of the closing tag for which this EndElement object was created.
                 */
                const String& name() const;

                /**
                 * @brief Sets the tag name of the end tag for which this EndElement object was created.
                 * @param name The new name for this EndElement object.
                 */
                void setName(const String& name);

                /**
                 * @brief Compares this EndElement object with the given node.
                 *
                 * This method returns $true$ if the given node also is a EndElement object and
                 * the name of both EndElement objects is the same. Otherwise it returns $false$.
                 *
                 * @param node This Node object is compared to the current EndElement node object.
                 * @return $true if this EndElement object is the same as the given node.
                 */
                virtual bool operator==(const Node& node) const;

            private:
                //! The tag name of this end tag.
                String _name;
        };

    }

}

#endif
