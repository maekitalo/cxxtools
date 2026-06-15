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

#ifndef CXXTOOLS_CSVPARSER_H
#define CXXTOOLS_CSVPARSER_H

#include <cxxtools/string.h>
#include <vector>

namespace cxxtools
{
    class CsvDeserializer;

    class CsvParser
    {
        public:
            CsvParser()
                : _deserializer(nullptr),
                  _delimiter(autoDelimiter),
                  _readTitle(true),
                  _skipEmptyLines(false),
                  _column(0),
                  _noColumns(0),
                  _lineNo(0),
                  _colNo(0),
                  _quote(L'\0')
            { }

            Char delimiter() const
            { return _delimiter; }

            void delimiter(Char ch)
            { _delimiter = ch; }

            bool readTitle() const
            { return _readTitle; }

            void readTitle(bool sw)
            { _readTitle = sw; }

            bool skipEmptyLines() const
            { return _skipEmptyLines; }

            void skipEmptyLines(bool sw)
            { _skipEmptyLines = sw; }

            unsigned lineNo() const
            { return _lineNo; }

            unsigned colNo() const
            { return _colNo; }

            static const Char autoDelimiter;

            void begin(CsvDeserializer& handler);
            void advance(Char ch);
            void finish();

        private:

            const std::string& titleFor(size_t col) const
            {
                static const std::string empty;
                return col < _titles.size() ? _titles[col] : empty;
            }

            void setValue();

            enum class State
            {
                detectDelim,
                detectDelim_q,
                detectDelim_postq,
                title,
                qtitle,
                qtitlep,
                cr,
                rowstart,
                datastart,
                data0,
                data,
                qdata,
                qdata_end
            } _state;

            CsvDeserializer* _deserializer;
            Char _delimiter;
            bool _readTitle;
            bool _skipEmptyLines;

            typedef std::vector<std::string> rowType;
            rowType _titles;
            String _value;
            rowType::size_type _column;
            unsigned _noColumns;
            unsigned _lineNo;
            unsigned _colNo;
            Char _quote;
    };
}

#endif // CXXTOOLS_CSVPARSER_H
