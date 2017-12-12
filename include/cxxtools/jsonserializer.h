/*
 * Copyright (C) 2011 Tommi Maekitalo
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

#ifndef CXXTOOLS_JSONSERIALIZER_H
#define CXXTOOLS_JSONSERIALIZER_H

#include <cxxtools/decomposer.h>
#include <cxxtools/jsonformatter.h>
#include <iostream>
#include <sstream>
#include <stdexcept>

namespace cxxtools
{
    /**
     * Here is a small example to use this class.
     * @code{.cpp}
     * #include <iostream>
     * #include <cxxtools/jsonserializer.h>
     * #include <cxxtools/serializationinfo.h>
     *
     * // define a configuration object
     * struct Configuration
     * {
     *   struct Indent
     *   {
     *     int length;
     *     bool useSpace;
     *   };
     *
     *   std::string encoding;
     *   std::vector<std::string> plugIns;
     *   Indent indent;
     * };
     *
     * // define how to serialize the indent struct of configuration
     * void operator<<= (cxxtools::SerializationInfo& si, const Configuration::Indent& indent)
     * {
     *   si.addMember("length") <<= indent.length;
     *   si.addMember("use_space") <<= indent.useSpace;
     * }
     *
     * // define how to serialize the configuration
     * void operator<<= (cxxtools::SerializationInfo& si, const Configuration& config)
     * {
     *   si.addMember("encoding") <<= config.encoding;
     *   si.addMember("plug-ins") <<= config.plugIns;
     *   si.addMember("indent") <<= config.indent;
     * }
     *
     * int main(int argc, char* argv[])
     * {
     *   try
     *   {
     *     // create a configuration object:
     *     Configuration config;
     *     config.encoding = "UTF-8";
     *     config.plugIns.push_back("python");
     *     config.plugIns.push_back("c++");
     *     config.plugIns.push_back("ruby");
     *     config.indent.length = 3;
     *     config.indent.useSpace = true;
     *
     *     // serialize to json
     *     cxxtools::JsonSerializer serializer(std::cout);
     *     serializer.beautify(true);   // this makes it just nice to read
     *     serializer.serialize(config).finish();
     *   }
     *   catch (const std::exception& e)
     *   {
     *     std::cerr << e.what() << std::endl;
     *   }
     * }
     * @endcode
     */
    class JsonSerializer
    {
            // make non copyable
            JsonSerializer(const JsonSerializer&) { }
            JsonSerializer& operator=(const JsonSerializer&) { return *this; }

        public:
            JsonSerializer()
                : _os(0),
                  _inObject(false)
            {
            }

            explicit JsonSerializer(std::ostream& os)
                : _os(0),
                  _inObject(false)
            {
                begin(os);
            }

            JsonSerializer& begin(std::ostream& os)
            {
                _os = &os;
                _formatter.begin(os);
                return *this;
            }

            void finish()
            {
                if (_inObject)
                {
                    _formatter.finishObject();
                    _inObject = false;
                }

                _formatter.finish();

                _os->flush();
            }

            template <typename T>
            JsonSerializer& serialize(const T& v, const std::string& name)
            {
                Decomposer<T> s;
                s.begin(v);
                s.setName(name);

                if (!_inObject)
                {
                    _formatter.beginObject(std::string(), std::string());
                    _inObject = true;
                }

                s.format(_formatter);
                return *this;
            }

            template <typename T>
            JsonSerializer& serialize(const T& v)
            {
                if (_inObject)
                    throw std::logic_error("can't serialize object without name into another object");

                Decomposer<T> s;
                s.begin(v);
                s.format(_formatter);
                _os->flush();
                return *this;
            }

            void setObject()
            {
                _formatter.beginObject(std::string(), std::string());
                _inObject = true;
            }

            bool object() const       { return _inObject; }

            bool beautify() const     { return _formatter.beautify(); }

            void beautify(bool sw)    { _formatter.beautify(sw); }

            bool plainkey() const     { return _formatter.plainkey(); }

            void plainkey(bool sw)    { _formatter.plainkey(sw); }

            bool inputUtf8() const     { return _formatter.inputUtf8(); }

            void inputUtf8(bool sw)    { _formatter.inputUtf8(sw); }

            template <typename T>
            static std::string toString(const T& type, const std::string& name, bool beautify = false)
            {
                std::ostringstream os;
                JsonSerializer s;
                if (beautify)
                  s.beautify(true);
                s.begin(os);
                s.serialize(type, name);
                s.finish();
                return os.str();
            }

        private:
            JsonFormatter _formatter;
            std::ostream* _os;
            bool _inObject;
    };

}

#endif // CXXTOOLS_JSONSERIALIZER_H

