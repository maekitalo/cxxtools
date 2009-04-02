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
#ifndef CXXTOOLS_Xml_Comment_h
#define CXXTOOLS_Xml_Comment_h

#include <cxxtools/xml/api.h>
#include <cxxtools/xml/node.h>
#include <cxxtools/string.h>


namespace cxxtools {

    namespace xml {

        /**
         * @brief A Comment element (Node) of an XML document, containing the comment's content.
         *
         * A Comment element stores the content of a comment. There is no interpretation of the
         * comment's Text before it is stored.
         *
         * Use Text() to get the content/Text of the comment element without the &lt;!-- and -->.
         *
         * When parsing a comment %&lt;!-- This is a comment -->$ the following Text will be
         * returned by Text(): $This is a comment$
         *
         * @see Node
         */
        class CXXTOOLS_XML_API Comment : public Node {
            public:
                /**
                 * @brief Constructs a new Comment object with the given string as content/Text.
                 *
                 * @param Text The content/Text of the Comment object.
                 */
                Comment(const String& Text);

                //! Empty destructor
                ~Comment();

                /**
                 * @brief Clones this Comment object by creating a duplicate on the heap and returning it.
                 * @return A cloned version of this Comment object.
                 */
                Comment* clone() const
                {return new Comment(*this);}

                /**
                 * @brief Returns the content/Text of this Comment object.
                 *
                 * The content includes everything that is between the start and end "tags" of the comment
                 * without being parsed. When parsing a comment %&lt;!-- This is a comment -->$ the following
                 * Text will be returned: $This is a comment$
                 *
                 * @return The Text of this Comment object.
                 */
                String& text();

                /**
                 * @brief Returns the content/Text of this Comment object.
                 *
                 * The content includes everything that is between the start and end "tags" of the comment
                 * without being parsed. When parsing a comment %&lt;!-- This is a comment -->$ the following
                 * Text will be returned: $This is a comment$
                 *
                 * @return The Text of this Comment object.
                 */
                const String& text() const;

                /**
                 * @brief Sets the Text of this Comment object.
                 * @param text The new Text for this Comment object.
                 */
                void setText(const String text);

            private:
                //! The Text of this Comment object.
                String _text;
        };

    }

}

#endif
