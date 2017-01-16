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
#ifndef cxxtools_Xml_XmlWriter_h
#define cxxtools_Xml_XmlWriter_h

#include <cxxtools/string.h>
#include <cxxtools/textstream.h>
#include <cxxtools/xml/attribute.h>
#include <stack>

namespace cxxtools {

namespace xml {

    class XmlWriter
    {
        public:
            XmlWriter();

            XmlWriter(std::ostream& os, int format =  UseXmlDeclaration | UseIndent | UseEndl);

            void begin(std::ostream& os);

            void writeStartElement(const cxxtools::String& localName);

            void writeStartElement(const cxxtools::String& localName, const Attribute* attr, size_t attrCount);

            void writeStartElement(const cxxtools::String& localName, const Attributes& attr)
                { writeStartElement(localName, &attr[0], attr.size()); }

            void writeEndElement();

            void writeElement(const cxxtools::String& localName, const cxxtools::String& content);

            void writeElement(const cxxtools::String& localName, const Attribute* attr, size_t attrCount, const cxxtools::String& content);

            void writeElement(const cxxtools::String& localName, const Attributes& attr, const cxxtools::String& content)
                { writeElement(localName, &attr[0], attr.size(), content); }

            void writeContent(const cxxtools::String& text);

            void writeCharacters(const cxxtools::String& text);

            void flush();

            void endl();

            enum FormatFlags {
              UseXmlDeclaration = 1,
              UseIndent = 2,
              UseEndl = 4
            };

            void setFormat(int f)  { _flags = f; }

            void setFormatFlags(int f, bool sw = true)  { if (sw) _flags |= f; else _flags &= ~f; }

            int format() const               { return _flags; }

            bool useXmlDeclaration() const   { return _flags & UseXmlDeclaration; }

            void useXmlDeclaration(bool sw)  { setFormatFlags(UseXmlDeclaration, sw); }

            bool useIndent() const           { return _flags & UseIndent; }

            void useIndent(bool sw)          { setFormatFlags(UseIndent, sw); }

            bool useEndl() const             { return _flags & UseEndl; }

            void useEndl(bool sw)            { setFormatFlags(UseEndl, sw); }

            void beautify(bool sw)           { _flags = sw ? (UseXmlDeclaration | UseIndent | UseEndl) : 0; }

            class Element
            {
            public:
                Element(XmlWriter& writer, const cxxtools::String& localName)
                    : _writer(writer)
                    { _writer.writeStartElement(localName); }

                Element(XmlWriter& writer, const cxxtools::String& localName, const Attribute* attr, size_t attrCount)
                    : _writer(writer)
                    { _writer.writeStartElement(localName, attr, attrCount); }

                Element(XmlWriter& writer, const cxxtools::String& localName, const Attributes& attr)
                    : _writer(writer)
                    { _writer.writeStartElement(localName, attr); }

                void writeContent(const String& text)
                {
                    _writer.writeCharacters(text);
                    if (_writer.useEndl())
                        _writer.endl();
                }


                ~Element()
                    { _writer.writeEndElement(); }

            private:
                XmlWriter& _writer;
            };

        private:
            TextOStream _tos;
            std::stack<cxxtools::String> _elements;
            int _flags;
    };

}

}

#endif
