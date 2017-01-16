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
#ifndef cxxtools_xml_Attribute_h
#define cxxtools_xml_Attribute_h

#include <cxxtools/string.h>
#include <cxxtools/date.h>
#include <cxxtools/time.h>
#include <cxxtools/datetime.h>
#include <vector>

namespace cxxtools {

namespace xml {

    /**
     * @brief A class representing a single attribute of a tag from an XML document.
     *
     * An XML attribute consists of the attribute's name and the attribute's value.
     * The name can be retrieved using the method name(). The value can be retrieved
     * using the method value().
     *
     * The attributes of a tag are retrieved from the document when the opening tag
     * is parsed. The attributes are stored in a StartElement object from where they
     * can be retrieved.
     */
    class Attribute
    {
        public:
            //! Constructs a new Attribute object with an empty name and value.
            Attribute()
            { }

            /**
             * @brief Constructs a new Attribute using the given name and value.
             *
             * @param name The name of the XML attribute.
             * @param value The value of the XML attribute.
             */
            Attribute(const String& name, const String& value)
            : _name(name), _value(value)
            { }

            Attribute(const String& name, const Date& value)
            : _name(name), _value(value.toString("%Y-%m-%d"))
            { }

            Attribute(const String& name, const Time& value)
            : _name(name), _value(value.toString("%H:%M:%S"))
            { }

            Attribute(const String& name, const DateTime& value)
            : _name(name), _value(value.toString("%Y-%m-%dT%H:%M:%S"))
            { }

            /**
             * @brief Returns the name of this attribute.
             * @return The attribute's name.
             */
            const String& name() const
            { return _name; }

            String& name()
            { return _name; }

            /**
             * @brief Sets the name of this attribute.
             * @param name The new name of this attribute.
             */
            void setName(const String& name)
            { _name = name; }

            /**
             * @brief Returns the value of this attribute.
             * @return The attribute's value.
             */
            const String& value() const
            { return _value; }

            String& value()
            { return _value; }

            /**
             * @brief Sets the value of this attribute.
             * @param value The new value of this attribute.
             */
            void setValue(const String& value)
            { _value = value; }

            void clear()
            { _name.clear(); _value.clear(); }

        private:
            //! The name of this attribute.
            String _name;

            //! The value of this attribute.
            String _value;
    };

    typedef std::vector<Attribute> Attributes;
}

}

#endif
