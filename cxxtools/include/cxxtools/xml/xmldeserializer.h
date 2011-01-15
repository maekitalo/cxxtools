/*
 * Copyright (C) 2008 by Marc Boris Duerner
 * 
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
#ifndef xis_cxxtools_XmlDeserializer_h
#define xis_cxxtools_XmlDeserializer_h

#include <cxxtools/string.h>
#include <cxxtools/deserializer.h>
#include "cxxtools/xml/xmlreader.h"
#include <memory>

namespace cxxtools {

namespace xml {

    class XmlReader;
    class Node;

    /** @brief Deserialize objects or object data to XML

        Thic class performs XML deserialization of a single object or
        object data.
    */
    class XmlDeserializer
    {
        public:
            XmlDeserializer(cxxtools::xml::XmlReader& reader);

            XmlDeserializer(std::istream& is);

            //! @brief Destructor
            ~XmlDeserializer();

            cxxtools::xml::XmlReader& reader()
            { return *_reader; }

            /** @brief Deserialize an object

                This method will deserialize the object \a type from an
                XML format. The type \a type must be serializable.
            */
            template <typename T>
            void deserialize(T& type)
            {
                cxxtools::Deserializer<T> deser;
                deser.begin(type);
                this->get(&deser);
                deser.fixup(_context);
            }

            void finish()
            {
                _context.fixup();
                _context.clear();
            }

            template <typename T>
            static void toObject(const std::string& str, T& type)
            {
               std::istringstream in(str);
               XmlDeserializer d(in);
               d.deserialize(type);
            }

            template <typename T>
            static void toObject(XmlReader& in, T& type)
            {
               XmlDeserializer d(in);
               d.deserialize(type);
            }

            template <typename T>
            static void toObject(std::istream& in, T& type)
            {
               XmlDeserializer d(in);
               d.deserialize(type);
            }

        protected:
            void get(cxxtools::IDeserializer* deser);

            //! @internal
            void beginDocument(const cxxtools::xml::Node& node);

            //! @internal
            void onRootElement(const cxxtools::xml::Node& node);

            //! @internal
            void onStartElement(const cxxtools::xml::Node& node);

            //! @internal
            void onWhitespace(const cxxtools::xml::Node& node);

            //! @internal
            void onContent(const cxxtools::xml::Node& node);

            //! @internal
            void onEndElement(const cxxtools::xml::Node& node);

        private:
            //! @internal
            cxxtools::xml::XmlReader* _reader;

            //! @internal
            std::auto_ptr<cxxtools::xml::XmlReader> _deleter;

            //! @internal
            typedef void (XmlDeserializer::*ProcessNode)(const cxxtools::xml::Node&);

            //! @internal
            ProcessNode _processNode;

            size_t _startDepth;

            cxxtools::DeserializationContext _context;

            //! @internal
            cxxtools::IDeserializer* _deser;

            //! @internal
            cxxtools::String _nodeName;

            cxxtools::String _nodeId;

            cxxtools::String _nodeType;

            cxxtools::String _nodeCategory;

            SerializationInfo::Category nodeCategory() const;

    };

} // namespace xml

} // namespace cxxtools

#endif
