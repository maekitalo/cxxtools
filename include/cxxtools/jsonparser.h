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

#ifndef CXXTOOLS_JSONPARSER_H
#define CXXTOOLS_JSONPARSER_H

#include <cxxtools/string.h>
#include <cxxtools/serializationerror.h>

namespace cxxtools
{
    class JsonDeserializer;

    class JsonParserError : public SerializationError
    {
            friend class JsonParser;
            unsigned _lineNo;
            mutable std::string _msg;

        public:
            JsonParserError(const std::string& msg, unsigned lineNo)
                : SerializationError(msg),
                  _lineNo(lineNo)
            { }

            ~JsonParserError() throw()
            { }

            const char* what() const throw();
    };

    class JsonNoData : public JsonParserError
    {
        public:
            explicit JsonNoData(unsigned lineNo)
                : JsonParserError("no json data found", lineNo)
            { }
    };

    class JsonParser
    {
            class JsonStringParser
            {
                    JsonParser* _jsonParser;
                    String _str;
                    unsigned _count;
                    unsigned short _value;
                    unsigned short _surrogateValue;

                    enum
                    {
                        state_0,
                        state_esc,
                        state_hex,
                        state_highsurrogate0,
                        state_lowsurrogate0,
                        state_highsurrogate_esc,
                        state_lowsurrogate_esc,
                        state_highsurrogate_hex,
                        state_lowsurrogate_hex
                    } _state;

                public:
                    explicit JsonStringParser(JsonParser* jsonParser)
                        : _jsonParser(jsonParser),
                          _state(state_0)
                        { }

                    bool advance(Char ch);

                    void clear()
                    { _state = state_0; _str.clear(); }

                    const String& str() const
                    { return _str; }

                    String&& str()
                    { return std::move(_str); }

                    void str(String&& s)
                    { _str = std::move(s); }
            };

            JsonParser(const JsonParser&) = delete;
            JsonParser& operator=(const JsonParser&) = delete;

        public:
            JsonParser();
            ~JsonParser();

            void begin(JsonDeserializer& handler)
            {
                _state = state_beforestart;
                _token.clear();
                _deserializer = &handler;
            }

            int advance(Char ch); // 1: end character detected; -1: end but char not consumed; 0: no end
            void finish();

        private:
            enum
            {
                state_beforestart,
                state_0,
                state_object,
                state_object_plainname,
                state_object_name,
                state_object_after_name,
                state_object_value,
                state_object_e,
                state_object_next_member0,
                state_object_next_member,
                state_array,
                state_array_value0,
                state_array_value,
                state_array_e,
                state_string,
                state_number,
                state_float,
                state_token,
                state_comment0,
                state_commentline,
                state_comment,
                state_comment_e,
                state_end
            } _state, _nextState;

            String _token;

            JsonDeserializer* _deserializer;
            JsonStringParser _stringParser;
            JsonParser* _next;
            unsigned _lineNo;

            void doThrow(const std::string& msg);
            void throwInvalidCharacter(Char ch);
    };
}

#endif // CXXTOOLS_JSONPARSER_H
