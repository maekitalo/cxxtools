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
#ifndef cxxtools_xml_Characters_h
#define cxxtools_xml_Characters_h

#include <cxxtools/xml/api.h>
#include <cxxtools/xml/node.h>
#include <cxxtools/string.h>

namespace cxxtools {

    namespace xml {

        /**
         * @brief A Character element (Node) of an XML document, containing the body's Text of a tag.
         *
         * A Character element stores the data of a tag's body. The data is interpreted before it
         * is set as content of a Character element. This means that entities were translated into
         * their corresponding character sequence, ...
         *
         * Use content() to get the content of the CDATA element.
         *
         * When parsing a tag $&lt;a>This is the body's Text&lt;/a>$ the following content will be
         * returned by content(): $This is the body's Text$
         *
         * @see Node
         */
        class CXXTOOLS_XML_API Characters : public Node
        {
            public:
                /**
                 * @brief Constructs a new Character object with the given (optional) string as content.
                 *
                 * @param content The content of the Character object. This is an optional parameter.
                 * Default is an empty string.
                 */
                Characters( const String& content = String() );

                //! Empty destructor
                ~Characters();

                /**
                 * @brief Clones this Character object by creating a duplicate on the heap and returning it.
                 * @return A cloned version of this Character object.
                 */
                Characters* clone() const
                { return new Characters(*this); }

                /**
                 * @brief Returns $true$ if the content of this Character object is empty; $false$ otherwise.
                 * @return $true$ if the content of this Character object is empty; $false$ otherwise.
                 */
                bool empty() const;

                void clear()
                { _content.clear(); }

                /**
                 * @brief Returns the content of this Character object.
                 *
                 * The content includes the Text inside a tag's body. The Text is interpreted before it
                 * is returned, this means that for example entities are translated into their corresponding
                 * character sequence. When parsing a tag $<a>This is the body's Text</a>$ the followin
                 * content will be returned: $This is the body's Text$
                 *
                 * @return The content of this Character object.
                 */
                String& content();

                /**
                 * @brief Returns the content of this Character object.
                 *
                 * The content includes the Text inside a tag's body. The Text is interpreted before it
                 * is returned, this means that for example entities are translated into their corresponding
                 * character sequence. When parsing a tag $<a>This is the body's Text</a>$ the followin
                 * content will be returned: $This is the body's Text$
                 *
                 * @return The content of this Character object.
                 */
                const String& content() const;

                /**
                 * @brief Sets the content of this Character object.
                 * @param content The new content for this Character object.
                 */
                void setContent(const String& content);

                /**
                 * @brief Compares this Character object with the given node.
                 *
                 * This method returns $true$ if the given node also is a Character object and
                 * the content of both Character objects is the same. Otherwise it returns $false$.
                 *
                 * @param node This Node object is compared to the current Character node object.
                 * @return $true if this Character object is the same as the given node.
                 */
                virtual bool operator==(const Node& node) const;

            private:
                //! The content of this Character object.
                String _content;
        };

    }

}

#endif
