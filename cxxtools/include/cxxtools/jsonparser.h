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

#ifndef CXXTOOLS_JSONPARSER_H
#define CXXTOOLS_JSONPARSER_H

#include <cxxtools/api.h>
#include <cxxtools/string.h>

namespace cxxtools
{
    class DeserializationContext;
    class IDeserializer;

    class CXXTOOLS_API JsonParser
    {
            class JsonStringParser
            {
                    String _str;
                    unsigned _count;
                    unsigned short _value;

                    enum
                    {
                        state_0,
                        state_esc,
                        state_hex,
                    } _state;

                public:
                    JsonStringParser()
                        : _state(state_0)
                        { }

                    bool advance(Char ch);

                    void clear()
                    { _state = state_0; _str.clear(); }

                    const String& str() const
                    { return _str; }
            };

        public:
            JsonParser();

            void begin(IDeserializer& handler, DeserializationContext& context)
            {
                _state = state_0;
                _token.clear();
                _deserializer = &handler;
                _context = &context;
            }

            int advance(Char ch); // 1: end character detected; -1: end but char not consumed; 0: no end
            bool finish();

        private:
            enum
            {
                state_0,
                state_object,
                state_object_name,
                state_object_after_name,
                state_object_value,
                state_object_e,
                state_object_next_member,
                state_array,
                state_array_value,
                state_array_e,
                state_string,
                state_number,
                state_float,
                state_token,
                state_end
            } _state;

            String _token;

            DeserializationContext* _context;
            IDeserializer* _deserializer;
            JsonStringParser _stringParser;
            JsonParser* _next;
    };
}

#endif // CXXTOOLS_JSONPARSER_H
