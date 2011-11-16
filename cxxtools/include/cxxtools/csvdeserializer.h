/*
 * Copyright (C) 2011 Tommi Maekitalo
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * is provided AS IS, WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, and
 * NON-INFRINGEMENT.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 *
 */

#ifndef CXXTOOLS_CSVDESERIALIZER_H
#define CXXTOOLS_CSVDESERIALIZER_H

#include <cxxtools/char.h>
#include <cxxtools/deserializer.h>
#include <cxxtools/textstream.h>
#include <cxxtools/utf8codec.h>
#include <cxxtools/noncopyable.h>

namespace cxxtools
{
    class CsvDeserializer : private NonCopyable
    {
        public:
            CsvDeserializer(std::istream& in, TextCodec<Char, char>* codec = new Utf8Codec());

            CsvDeserializer(TextIStream& in);

            ~CsvDeserializer();

            template <typename T>
            void deserialize(T& t)
            {
                Deserializer<T> d;
                d.begin(t);
                get(&d);
                d.fixup(_context);
                _context.fixup();
                _context.clear();
            }

            Char delimiter() const
            { return _delimiter; }

            void delimiter(Char ch)
            { _delimiter = ch; }

            bool readTitle() const
            { return _readTitle; }

            void readTitle(bool sw)
            { _readTitle = sw; }

            static const Char autoDelimiter;

            template <typename T>
            static void toObject(std::istream& in, T& type)
            {
                CsvDeserializer d(in);
                d.deserialize(type);
            }

        private:
            void get(IDeserializer* d);

            DeserializationContext _context;

            TextIStream* _ts;
            TextIStream& _in;

            Char _delimiter;

            bool _readTitle;


    };
}

#endif // CXXTOOLS_CSVDESERIALIZER_H
