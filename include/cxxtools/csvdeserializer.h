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

#ifndef CXXTOOLS_CSVDESERIALIZER_H
#define CXXTOOLS_CSVDESERIALIZER_H

#include <cxxtools/char.h>
#include <cxxtools/deserializer.h>
#include <cxxtools/textstream.h>
#include <cxxtools/utf8codec.h>
#include <cxxtools/csvparser.h>

namespace cxxtools
{
    class CsvDeserializer : public Deserializer
    {
        public:
            CsvDeserializer()
            { }

            Char delimiter() const
            { return _parser.delimiter(); }

            void delimiter(Char ch)
            { _parser.delimiter(ch); }

            bool readTitle() const
            { return _parser.readTitle(); }

            void readTitle(bool sw)
            { _parser.readTitle(sw); }

            static const Char autoDelimiter;

            void begin();

            void advance(Char ch)
            { _parser.advance(ch); }

            void finish()
            { _parser.finish(); }

            void read(std::istream& in, TextCodec<Char, char>* codec = new Utf8Codec());
            void read(std::basic_istream<Char>& in);

            template <typename T>
            static void toObject(std::istream& in, T& type)
            {
                CsvDeserializer d;
                d.read(in);
                d.deserialize(type);
            }

        private:
            void doDeserialize(std::basic_istream<Char>& in);

            CsvParser _parser;
    };
}

#endif // CXXTOOLS_CSVDESERIALIZER_H
