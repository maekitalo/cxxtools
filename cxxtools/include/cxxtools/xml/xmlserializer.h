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

#include <cxxtools/string.h>
#include <cxxtools/formatter.h>
#include <cxxtools/serializer.h>
#include <memory>

namespace cxxtools {

namespace xml {

class XmlWriter;

/** @brief Serialize objects or object data to XML

    Thic class performs XML serialization of a single object or
    object data.
*/
class XmlFormatter : public cxxtools::Formatter
{
    public:
        /** @brief Construct a serializer without initializing the
                    serializer for writing.

            The serializer can be "opened" for writing by calling
            method attach().
        */
        XmlFormatter();

        /** @brief Construct a serializer writing to a byte stream

            The serializer will write the objects as XML with
            UTF-8 encoding to the output stream.
        */
        XmlFormatter(std::ostream& os);


        /** @brief Construct a serializer writing to the given XmlWriter object

            The serializer will write the objects to the given XmlWriter object.
            This class will not free the given XmlWriter object. The caller is
            responsible to free it if needed.
        */
        XmlFormatter(cxxtools::xml::XmlWriter* writer);

        //! @brief Destructor
        ~XmlFormatter();

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

        //! @internal
        void flush();

        void addValue(const std::string& name, const std::string& type,
                      const cxxtools::String& value, const std::string& id);

        void addReference(const std::string& name, const cxxtools::String& value);

        void beginArray(const std::string& name, const std::string& type,
                        const std::string& id);

        void finishArray();

        void beginObject(const std::string& name, const std::string& type,
                         const std::string& id);

        void beginMember(const std::string& name);

        void finishMember();

        void finishObject();

        void finish();

    private:
        void beginComplexElement(const std::string& name, const std::string& type,
                        const std::string& id, const String& category);

        //! @internal
        cxxtools::xml::XmlWriter* _writer;

        //! @internal
        std::auto_ptr<cxxtools::xml::XmlWriter> _deleter;
};


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

        /** @brief Serialize an object to XML

            The serializer will serialize the object \a type as
            XML to the assigned stream. The string \a name will be used
            as the instance name of \a type and appear as the name of the
            XML element. The type must be serializable.
        */
        template <typename T>
        void serialize(const T& type, const std::string& name)
        {
            cxxtools::ISerializer* serializer = _context.begin(type);
            serializer->setName(name);
        }

        void finish()
        {
            _context.fixdown(_formatter);
            _context.clear();
            flush();
        }

        //! @internal
        void flush();

    private:
        XmlFormatter _formatter;

        //! @internal
        cxxtools::SerializationContext _context;
};

} // namespace xml

} // namespace cxxtools

#endif
