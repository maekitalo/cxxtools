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

#ifndef CXXTOOLS_CSVFORMATTER_H
#define CXXTOOLS_CSVFORMATTER_H

#include <cxxtools/formatter.h>
#include <cxxtools/textstream.h>
#include <cxxtools/utf8codec.h>
#include <cxxtools/string.h>
#include <cxxtools/char.h>
#include <vector>

namespace cxxtools
{
    class CsvFormatter : public Formatter
    {
        public:
            explicit CsvFormatter(std::ostream& os, TextCodec<Char, char>* codec = new Utf8Codec());

            explicit CsvFormatter(TextOStream& os);

            ~CsvFormatter();

            void selectColumn(const std::string& title);

            void selectColumn(const std::string& memberName, const std::string& title);

            void delimiter(String delimiter)
            { _delimiter = delimiter; }

            void quote(Char quote)
            { _quote = quote; }

            void lineEnding(const String& le)
            { _lineEnding = le; }

            virtual void addValueString(const std::string& name, const std::string& type,
                                  String&& value);

            virtual void beginArray(const std::string& name, const std::string& type);

            virtual void finishArray();

            virtual void beginObject(const std::string& name, const std::string& type);

            virtual void beginMember(const std::string& name);

            virtual void finishMember();

            virtual void finishObject();

            virtual void finish();

        private:
            void toCsvData(String& ret, const std::string& type, const String& value);
            void dataOut();

            bool _firstline;
            bool _collectTitles;
            unsigned _level;

            String _delimiter;
            Char _quote;
            String _lineEnding;

            // titles and member names
            struct Title
            {
                std::string _memberName;
                std::string _title;
            };

            std::vector<Title> _titles;

            std::vector<String> _data;
            std::string _memberName;
            TextOStream* _ts;
            TextOStream& _os;
    };
}

#endif // CXXTOOLS_CSVFORMATTER_H
