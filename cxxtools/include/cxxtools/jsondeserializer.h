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

#ifndef CXXTOOLS_JSONDESERIALIZER_H
#define CXXTOOLS_JSONDESERIALIZER_H

#include <cxxtools/jsonparser.h>
#include <cxxtools/api.h>
#include <cxxtools/char.h>
#include <cxxtools/deserializer.h>
#include <cxxtools/textstream.h>
#include <cxxtools/utf8codec.h>

namespace cxxtools
{
    class CXXTOOLS_API JsonDeserializer
    {
        public:
            JsonDeserializer(std::istream& in, TextCodec<Char, char>* codec = new Utf8Codec());

            JsonDeserializer(TextIStream& in);

            ~JsonDeserializer();

            template <typename T>
            void deserialize(T& type)
            {
                Deserializer<T> deser;
                deser.begin(type);
                this->get(&deser);
                deser.fixup(_context);
            }

            void finish()
            {
                _context.fixup();
                _context.clear();
            }

        protected:
            void get(IDeserializer* deser);

        private:
            JsonParser _parser;
            DeserializationContext _context;

            TextIStream* _ts;
            TextIStream& _in;
    };
}

#endif // CXXTOOLS_JSONDESERIALIZER_H
