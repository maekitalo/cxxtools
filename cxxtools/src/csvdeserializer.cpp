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

#include <cxxtools/csvdeserializer.h>
#include <stdexcept>
#include <cxxtools/log.h>

log_define("cxxtools.csv.deserializer")

namespace cxxtools
{
    namespace
    {
        const unsigned unknownNoColumns = std::numeric_limits<unsigned>::max();

        void checkNoColumns(unsigned column, unsigned& noColumns, unsigned lineNo)
        {
            if (noColumns == unknownNoColumns)
            {
                column = noColumns;
            }
            else if (column + 1 != noColumns)
            {
                std::ostringstream msg;
                msg << "number of columns " << (column + 1) << " in line " << lineNo << " does not match expected number of columns " << noColumns;
                throw std::runtime_error(msg.str());
            }
        }

    }

    const Char CsvDeserializer::autoDelimiter = L'\0';

    CsvDeserializer::CsvDeserializer(std::istream& in, TextCodec<Char, char>* codec)
        : _ts(new TextIStream(in, codec)),
          _in(*_ts),
          _delimiter(autoDelimiter),
          _readTitle(true)
    { }

    CsvDeserializer::CsvDeserializer(TextIStream& in)
        : _ts(0),
          _in(in),
          _delimiter(autoDelimiter),
          _readTitle(true)
    { }

    CsvDeserializer::~CsvDeserializer()
    {
        delete _ts;
    }

    void CsvDeserializer::get(IDeserializer* deserializer)
    {
        enum
        {
            state_detectDelim,
            state_title,
            state_rowstart,
            state_datastart,
            state_data0,
            state_data,
            state_qdata,
            state_qdata_end,
        } state = (_readTitle ? state_detectDelim : state_rowstart);

        if (_delimiter == autoDelimiter && !_readTitle)
            throw std::runtime_error("can't read csv data with auto delimiter but without title");

        Char delimiter = _delimiter;

        typedef std::vector<std::string> rowType;
        rowType titles;
        String value;
        rowType::size_type column = 0;
        unsigned noColumns = unknownNoColumns;
        unsigned lineNo = 0;

        titles.push_back(std::string());

        Char ch;
        Char quote;
        while (_in.get(ch))
        {
            if (ch == L'\n')
                ++lineNo;

            switch (state)
            {
                case state_detectDelim:
                    if (isalnum(ch) || ch == L'_')
                    {
                        titles.back() += ch.narrow();
                    }
                    else if (ch == L'\n')
                    {
                        log_debug("title=\"" << titles.back() << '"');
                        state = state_rowstart;
                    }
                    else
                    {
                        delimiter = ch;
                        log_debug("delimiter=" << delimiter.narrow());
                        log_debug("title=\"" << titles.back() << '"');
                        titles.push_back(std::string());
                        state = state_title;
                    }
                    break;

                case state_title:
                    if (ch == L'\n')
                    {
                        log_debug("title=\"" << titles.back() << '"');
                        state = state_rowstart;
                        noColumns = titles.size();
                    }
                    else if (ch == delimiter)
                    {
                        log_debug("title=\"" << titles.back() << '"');
                        titles.push_back(std::string());
                    }
                    else
                    {
                        titles.back() += ch.narrow();
                    }
                    break;

                case state_rowstart:
                    column = 0;
                    log_debug("new row");
                    deserializer->beginMember(std::string(),
                        std::string(), SerializationInfo::Array);
                    // no break

                case state_datastart:
                    if (ch == L'\n')
                    {
                        checkNoColumns(column, noColumns, lineNo);
                        deserializer->leaveMember();
                        state = state_rowstart;
                        break;
                    }

                    log_debug("member \""
                        << (column < titles.size() ? titles[column] : std::string()) << '"');
                    deserializer->beginMember(
                        column < titles.size() ? titles[column] : std::string(),
                        std::string(), SerializationInfo::Value);

                    if (ch == L'"' || ch == L'\'')
                    {
                        quote = ch;
                        state = state_qdata;
                    }
                    else if (ch == delimiter)
                    {
                        deserializer->leaveMember();
                    }
                    else
                    {
                        value += ch;
                        state = state_data;
                    }
                    break;

                case state_data0:
                    if (ch == L'"' || ch == L'\'')
                    {
                        quote = ch;
                        state = state_qdata;
                        break;
                    }

                case state_data:
                    if (ch == L'\n')
                    {
                        log_debug("value \"" << value << '"');
                        deserializer->setValue(value);
                        value.clear();
                        checkNoColumns(column, noColumns, lineNo);
                        deserializer->leaveMember();  // leave data item
                        deserializer->leaveMember();  // leave row
                        state = state_rowstart;
                    }
                    else if (ch == delimiter)
                    {
                        log_debug("value \"" << value << '"');
                        deserializer->setValue(value);
                        value.clear();
                        deserializer->leaveMember();  // leave data item
                        ++column;
                        log_debug("member \""
                            << (column < titles.size() ? titles[column] : std::string()) << '"');
                        deserializer->beginMember(
                            column < titles.size() ? titles[column] : std::string(),
                            std::string(), SerializationInfo::Value);
                        state = state_data0;
                    }
                    else
                    {
                        value  += ch;
                    }
                    break;

                case state_qdata:
                    if (ch == quote)
                    {
                        log_debug("value \"" << value << '"');
                        deserializer->setValue(value);
                        value.clear();
                        deserializer->leaveMember();  // leave data item
                        state = state_qdata_end;
                    }
                    else
                    {
                        value += ch;
                    }
                    break;

                case state_qdata_end:
                    if (ch == L'\n')
                    {
                        checkNoColumns(column, noColumns, lineNo);
                        deserializer->leaveMember();  // leave row
                        state = state_rowstart;
                    }
                    else if (ch == delimiter)
                    {
                        ++column;
                        log_debug("member \""
                            << (column < titles.size() ? titles[column] : std::string()) << '"');
                        deserializer->beginMember(
                            column < titles.size() ? titles[column] : std::string(),
                            std::string(), SerializationInfo::Value);
                        state = state_data0;
                    }
                    else
                    {
                        value = quote + value + ch;
                        state = state_data;
                    }
                    break;
            }

            //log_debug("ch=" << ch.narrow() << " state=" << state);
        }

        //log_debug("end processing");
        switch (state)
        {
            case state_datastart:
                deserializer->leaveMember();  // leave row
                break;

            case state_data0:
            case state_data:
                checkNoColumns(column, noColumns, lineNo);
                deserializer->setValue(value);
                deserializer->leaveMember();  // leave data item
                deserializer->leaveMember();  // leave row
                break;

            case state_qdata:
                checkNoColumns(column, noColumns, lineNo);
                log_debug("value \"" << quote.narrow() << value << '"');
                deserializer->setValue(quote + value);
                deserializer->leaveMember();  // leave data item
                deserializer->leaveMember();  // leave row
                break;

            case state_qdata_end:
                deserializer->leaveMember();  // leave row
                break;

        }
    }
}
