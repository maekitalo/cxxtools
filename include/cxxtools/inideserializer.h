/*
 * Copyright (C) 2019 Tommi Maekitalo
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

#ifndef CXXTOOLS_INIDESERIALIZER_H
#define CXXTOOLS_INIDESERIALIZER_H

#include <cxxtools/deserializer.h>
#include <cxxtools/iniparser.h>
#include <cxxtools/textstream.h>
#include <cxxtools/utf8codec.h>

namespace cxxtools
{
    /**
     * This class converts ini format to cxxtools::SerializationInfo.
     *
     * Here is a usage example:
     *
     * @code
     *
     * #include <iostream>
     * #include <cxxtools/inideserializer.h>
     * #include <cxxtools/serializationinfo.h>
     *
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
     * void operator>>= (const cxxtools::SerializationInfo& si, Configuration::Indent& indent)
     * {
     *   si.getMember("length") >>= indent.length;
     *   si.getMember("use_space") >>= indent.useSpace;
     * }
     *
     * void operator>>= (const cxxtools::SerializationInfo& si, Configuration& config)
     * {
     *   si.getMember("encoding") >>= config.encoding;
     *   si.getMember("plug-ins") >>= config.plugIns;
     *   si.getMember("indent") >>= config.indent;
     * }
     *
     * int main(int argc, char* argv[])
     * {
     *   try
     *   {
     *     // define a empty config object
     *     Configuration config;
     *
     *     // read ini configuration struct from stdin:
     *     cxxtools::IniDeserializer deserializer(std::cin);
     *     deserializer.deserialize(config);
     *
     *     // print configuration
     *     std::cout << "encoding=" << config.encoding << '\n'
     *               << "plugIns=";
     *     for (unsigned n = 0; n < config.plugIns.size(); ++n)
     *       std::cout << config.plugIns[n] << ' ';
     *     std::cout << '\n'
     *               << "indent.length=" << config.indent.length << '\n'
     *               << "indent.useSpace=" << config.indent.useSpace << '\n';
     *   }
     *   catch (const std::exception& e)
     *  {
     *     std::cerr << e.what() << std::endl;
     *   }
     * }
     * @endcode
     */
    class IniDeserializer : public Deserializer
    {
        public:
            explicit IniDeserializer(std::istream& in, TextCodec<Char, char>* codec = new Utf8Codec());

            explicit IniDeserializer(std::basic_istream<Char>& in);
    };
}

#endif // CXXTOOLS_INIDESERIALIZER_H
