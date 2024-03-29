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

#ifndef CXXTOOLS_BIN_DESERIALIZER_H
#define CXXTOOLS_BIN_DESERIALIZER_H

#include <cxxtools/deserializer.h>
#include <cxxtools/bin/serializer.h>
#include <cxxtools/bin/parser.h>

#include <iosfwd>
#include <cstddef>

namespace cxxtools
{
namespace bin
{
    class Deserializer : public cxxtools::Deserializer
    {
        public:
            typedef bin::Serializer::Type TypeCode;

            /// Default construct binary deserializer.
            Deserializer()
            { }

            /// Creates a binary deserializer and processes all input from passed stream.
            explicit Deserializer(std::istream& in)
            { read(in); }
            explicit Deserializer(std::streambuf& in)
            { read(in); }

            Deserializer(const char* data, size_t size);

            /// Processes all input from passed stream.
            void read(std::istream& in);
            void read(std::streambuf& in);

            /// Initialize the binary deserializer to receive data.
            void begin(bool resetDictionary = true);

            /// Process available characters in input stream buf.
            /// Only characters available in input buffer are processed. No
            /// underflow is triggered.
            /// Returns true, if the end of data is reached.
            bool advance(std::streambuf& in)
            { return _parser.advance(in); }

            /// Rest of input is parsed but do not process any data.
            void skip()
            { _parser.skip(); }

        private:
            void doDeserialize(std::istream& in);
            Parser _parser;
    };
}
}

#endif // CXXTOOLS_BIN_DESERIALIZER_H
