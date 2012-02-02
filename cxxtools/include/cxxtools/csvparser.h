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

#include <cxxtools/api.h>
#include <cxxtools/string.h>
#include <vector>

namespace cxxtools
{
    class DeserializerBase;

    class CXXTOOLS_API CsvParser
    {
        public:
            CsvParser()
                : _deserializer(0),
                  _delimiter(autoDelimiter),
                  _readTitle(true),
                  _noColumns(0),
                  _lineNo(0)
            { }

            Char delimiter() const
            { return _delimiter; }

            void delimiter(Char ch)
            { _delimiter = ch; }

            bool readTitle() const
            { return _readTitle; }

            void readTitle(bool sw)
            { _readTitle = sw; }

            static const Char autoDelimiter;

            void begin(DeserializerBase& handler);
            void advance(Char ch);
            void finish();

        private:
            enum
            {
                state_detectDelim,
                state_detectDelim_q,
                state_detectDelim_postq,
                state_title,
                state_qtitle,
                state_qtitlep,
                state_cr,
                state_rowstart,
                state_datastart,
                state_data0,
                state_data,
                state_qdata,
                state_qdata_end,
            } _state;

            DeserializerBase* _deserializer;
            Char _delimiter;
            bool _readTitle;

            typedef std::vector<std::string> rowType;
            rowType _titles;
            String _value;
            rowType::size_type _column;
            unsigned _noColumns;
            unsigned _lineNo;
            Char _quote;
    };
}

#endif // CXXTOOLS_CSVPARSER_H
