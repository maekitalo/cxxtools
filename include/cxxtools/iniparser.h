/*
 * Copyright (C) 2007 Tommi Maekitalo
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.    See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA    02110-1301    USA
 */

#ifndef CXXTOOLS_INIPARSER_H
#define CXXTOOLS_INIPARSER_H

#include <cxxtools/string.h>
#include <cxxtools/textstream.h>
#include <cxxtools/utf8codec.h>

namespace cxxtools
{
    /**
     * Parser for files in ini-format
     */
    class IniParser
    {
        public:
            class Event
            {
                protected:
                    virtual ~Event()    { }

                public:
                    // events return true, if parsing should be stopped
                    virtual bool onSection(const String& section);
                    virtual bool onKey(const String& key);
                    virtual bool onValue(const String& value);
                    virtual bool onComment(const String& comment);
                    virtual bool onError();
            };

        private:
            Event& event;
            String data;
            enum
            {
                state_0,
                state_section,
                state_key,
                state_key_sp,
                state_value0,
                state_value,
                state_valueesc,
                state_valuesq,     // single quote
                state_valuesqesc,
                state_valuedq,    // double quote
                state_valuedqesc,
                state_valueqend,
                state_comment

            } state;

        public:
            IniParser(Event& event_)
                : event(event_),
                    state(state_0)
                { }

            bool parse(Char ch);
            void end();
            void parse(std::istream& in, TextCodec<Char, char>* codec = new Utf8Codec());
            void parse(std::basic_istream<Char>& in);
    };
}

#endif
