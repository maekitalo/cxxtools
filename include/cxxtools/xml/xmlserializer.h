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
#ifndef cxxtools_Xml_XmlSerializer_h
#define cxxtools_Xml_XmlSerializer_h

#include <cxxtools/xml/xmlformatter.h>
#include <cxxtools/decomposer.h>
#include <sstream>

namespace cxxtools
{

namespace xml
{

/** @brief Serialize objects or object data to XML

    Thic class performs XML serialization of a single object or
    object data.
*/
class XmlSerializer
{
    public:
        /** @brief Construct a serializer without initializing the
                    serializer for writing.

            The serializer can be "opened" for writing by calling
            method attach().
        */
        XmlSerializer();

        /** @brief Construct a serializer writing to a byte stream

            The serializer will write the objects as XML with
            UTF-8 encoding to the output stream.
        */
        XmlSerializer(std::ostream& os);

        /** @brief Construct a serializer writing to the given XmlWriter object

            The serializer will write the objects to the given XmlWriter object.
            This class will not free the given XmlWriter object. The caller is
            responsible to free it if needed.
        */
        XmlSerializer(cxxtools::xml::XmlWriter* writer);

        //! @brief Destructor
        ~XmlSerializer();

        /** @brief Opens this serializer for writing into the given stream.

            The serializer will write the objects as XML with
            UTF-8 encoding to the output stream.

            This method does not have to be called if this XmlSerializer object
            was constructed using the constructor that takes an ostream or
            XmlWriter object. If this method is called anyway or called twice an
            std::logic_error is thrown.
        */
        void attach(std::ostream& os);

        /** @brief Opens this serializer for writing into the given XmlWriter object.

            The serializer will write the objects to the given XmlWriter object.

            This method does not have to be called if this XmlSerializer object
            was constructed using the constructor that takes an ostream or
            XmlWriter object. If this method is called anyway or called twice an
            std::logic_error is thrown.

            This class will not free the given XmlWriter object. The caller is
            responsible to free it if needed.
        */
        void attach(cxxtools::xml::XmlWriter& writer);

        /** @brief Detaches the currently set writer from this object.

            Before detaching the writer, the underlaying stream is flushed.
            If there is no currently set writer, nothing happens.
        */
        void detach();

        void useXmlDeclaration(bool sw)
        { _formatter.useXmlDeclaration(sw); }

        bool useXmlDeclaration() const
        { return _formatter.useXmlDeclaration(); }

        void useIndent(bool sw)
        { _formatter.useIndent(sw); }

        bool useIndent() const
        { return _formatter.useIndent(); }

        void useEndl(bool sw)
        { _formatter.useEndl(sw); }

        bool useEndl() const
        { return _formatter.useEndl(); }

        void useAttributes(bool sw)
        { _formatter.useAttributes(sw); }

        bool useAttributes() const
        { return _formatter.useAttributes(); }

        /** @brief Serialize an object to XML

            The serializer will serialize the object \a type as
            XML to the assigned stream. The string \a name will be used
            as the instance name of \a type and appear as the name of the
            XML element. The type must be serializable.
        */
        template <typename T>
        void serialize(const T& type, const std::string& name)
        {
            Decomposer<T> decomposer;
            decomposer.begin(type);
            decomposer.setName(name);
            decomposer.format(_formatter);
            _formatter.finish();
            _formatter.flush();
        }

        void finish()
        {
        }

        template <typename T>
        static std::string toString(const T& type, const std::string& name, bool beautify = false)
        {
          std::ostringstream os;
          XmlWriter writer(os, beautify ? XmlWriter::UseXmlDeclaration | XmlWriter::UseIndent | XmlWriter::UseEndl
                                        : XmlWriter::UseXmlDeclaration);
          XmlSerializer s(&writer);
          s.serialize(type, name);
          s.finish();
          return os.str();
        }

    private:
        XmlFormatter _formatter;
};

} // namespace xml

} // namespace cxxtools

#endif
