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
#ifndef CXXTOOLS_xml_ProcessingInstruction_h
#define CXXTOOLS_xml_ProcessingInstruction_h

#include <cxxtools/xml/node.h>
#include <cxxtools/string.h>

namespace cxxtools {

    namespace xml {

        /**
         * @brief A ProcessingInstruction (PI) element (Node) of an XML document.
         *
         * A processing instruction can be used to add instructions to an XML document which is needed
         * and can be used by specific XML processing software. The data of a processing instruction
         * has no particular format and can contain plain Text or XML-like attribute/value-associations.
         *
         * To access the target, which may for example be a identifier for a specific XML processor,
         * the method target() can be used. To access the data for this processor the method data()
         * can be used.
         *
         * @see Node
         */
        class ProcessingInstruction : public Node {
            public:
                //! Constructs a new ProcessingInstruction.
                ProcessingInstruction()
                : Node(Node::ProcessingInstruction)
                { }

                void clear()
                { _target.clear(); _data.clear(); }

                /**
                 * @brief Clones this CData object by creating a duplicate on the heap and returning it.
                 * @return A cloned version of this CData object.
                 */
                ProcessingInstruction* clone() const
                {return new ProcessingInstruction(*this);}

                /**
                 * @brief Returns the processor instruction's target.
                 *
                 * The target may be the XML processor for which this PI was added to the XML document.
                 *
                 * @return The target of this processing instruction.
                 */
                const String& target() const
                { return _target; }

                String& target()
                { return _target; }

                /**
                 * @brief Sets the processor instruction's target.
                 *
                 * @param target The target for this processing instruction.
                 */
                void setTarget(const String& target)
                { _target = target; }


                /**
                 * @brief Returns the processor instruction's data.
                 *
                 * The precise nature of the PI data depends on the XML processor for which this PI
                 * was added to the XML document. It usually contains special instructions for this processor.
                 *
                 * @return The data of this processing instruction.
                 */
                const String& data() const
                { return _data; }


                String& data()
                { return _data; }

                /**
                 * @brief Sets the processor instruction's data.
                 *
                 * @param data The data for this processing instruction.
                 */
                void setData(const String& data)
                { _data = data; }

            private:
                //! The target of this processing instruction.
                String _target;

                //! The data of this processing instruction.
                String _data;
        };

    }

}

#endif
