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

#include <cxxtools/csvparser.h>
#include <cxxtools/csvdeserializer.h>
#include <cxxtools/serializationerror.h>
#include <cxxtools/log.h>

#include <stdexcept>
#include <sstream>

log_define("cxxtools.csv.parser")

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
            msg << "number of columns " << (column + 1) << " in line " << lineNo << " does not match expected number of columns " << noColumns << " in csv";
            SerializationError::doThrow(msg.str());
        }
    }

}
const Char CsvParser::autoDelimiter = L'\0';

void CsvParser::begin(CsvDeserializer& handler)
{
    if (_delimiter == autoDelimiter && !_readTitle)
        throw std::logic_error("can't read csv data with auto delimiter but without title");

    _state = (_readTitle ? _delimiter == autoDelimiter ? state_detectDelim
                                                       : state_title
                         : state_rowstart);

    _deserializer = &handler;
    _titles.clear();
    _titles.push_back(std::string());
    _noColumns = unknownNoColumns;
    _lineNo = 0;
}

void CsvParser::advance(Char ch)
{
    if (ch == L'\n')
        ++_lineNo;

    switch (_state)
    {
        case state_detectDelim:
            if (isalnum(ch) || ch == L'_' || ch == L' ')
            {
                _titles.back() += ch.narrow();
            }
            else if (ch == L'\n' || ch == L'\r')
            {
                log_debug("title=\"" << _titles.back() << '"');
                _noColumns = 1;
                _state = (ch == L'\r' ? state_cr : state_rowstart);
            }
            else if (ch == L'\'' || ch == L'"')
            {
                _quote = ch;
                _state = state_detectDelim_q;
            }
            else
            {
                _delimiter = ch;
                log_debug("delimiter=" << _delimiter.narrow());
                log_debug("title=\"" << _titles.back() << '"');
                _titles.push_back(std::string());
                _state = state_title;
            }
            break;

        case state_detectDelim_q:
            if (ch == _quote)
            {
                _state = state_detectDelim_postq;
            }
            else
            {
                _titles.back() += ch.narrow();
            }
            break;

        case state_detectDelim_postq:
            if (isalnum(ch) || ch == L'_' || ch == L'\'' || ch == L'"' || ch == L' ')
            {
                std::ostringstream msg;
                msg << "invalid character '" << ch.narrow() << "' within csv title of column " << _titles.size();
                SerializationError::doThrow(msg.str());
            }
            else if (ch == L'\n' || ch == L'\r')
            {
                log_debug("title=\"" << _titles.back() << '"');
                _noColumns = 1;
                _state = (ch == L'\r' ? state_cr : state_rowstart);
            }
            else
            {
                _delimiter = ch;
                log_debug("delimiter=" << _delimiter.narrow());
                log_debug("title=\"" << _titles.back() << '"');
                _titles.push_back(std::string());
                _state = state_title;
            }
            break;

        case state_title:
            if (ch == L'\n' || ch == L'\r')
            {
                log_debug("title=\"" << _titles.back() << '"');
                _state = (ch == L'\r' ? state_cr : state_rowstart);
                _noColumns = _titles.size();
            }
            else if (ch == _delimiter)
            {
                log_debug("title=\"" << _titles.back() << '"');
                _titles.push_back(std::string());
            }
            else if (ch == L'\'' || ch == L'\"')
            {
                if (_titles.back().empty())
                {
                    _quote = ch;
                    _state = state_qtitle;
                }
                else
                {
                    std::ostringstream msg;
                    msg << "unexpected quote character within csv title of column " << _titles.size();
                    SerializationError::doThrow(msg.str());
                }
            }
            else
            {
                _titles.back() += ch.narrow();
            }
            break;

        case state_qtitle:
            if (ch == _quote)
            {
                _state = state_qtitlep;
            }
            else
            {
                _titles.back() += ch.narrow();
            }
            break;

        case state_qtitlep:
            if (ch == L'\n' || ch == L'\r')
            {
                log_debug("title=\"" << _titles.back() << '"');
                _state = (ch == L'\r' ? state_cr : state_rowstart);
                _noColumns = _titles.size();
            }
            else if (ch == _delimiter)
            {
                log_debug("title=\"" << _titles.back() << '"');
                _titles.push_back(std::string());
                _state = state_title;
            }
            else
            {
                std::ostringstream msg;
                msg << "invalid character '" << ch.narrow() << "' within csv title of column " << _titles.size();
                SerializationError::doThrow(msg.str());
            }
            break;

        case state_cr:
            _state = state_rowstart;
            if (ch == L'\n')
            {
                break;
            }
            // fallthrough

        case state_rowstart:
            _column = 0;
            if (_skipEmptyLines && (ch == L'\n' || ch == L'\r'))
            {
                if (ch == L'\r')
                    _state = state_cr;
                break;
            }

            log_debug("new row");
            _deserializer->beginMember(std::string(),
                std::string(), SerializationInfo::Object);
            _state = state_datastart;

            // no break

        case state_datastart:
            log_debug("member \""
                << (_column < _titles.size() ? _titles[_column] : std::string()) << '"');
            _deserializer->beginMember(
                _column < _titles.size() ? _titles[_column] : std::string(),
                std::string(), SerializationInfo::Value);

            if (ch == L'\n' || ch == L'\r')
            {
                _deserializer->leaveMember();
                checkNoColumns(_column, _noColumns, _lineNo);
                _deserializer->leaveMember();
                _state = (ch == L'\r' ? state_cr : state_rowstart);
            }
            else if (ch == L'"' || ch == L'\'')
            {
                _quote = ch;
                _state = state_qdata;
            }
            else if (ch == _delimiter)
            {
                ++_column;
                _deserializer->leaveMember();
            }
            else
            {
                _value += ch;
                _state = state_data;
            }
            break;

        case state_data0:
            if (ch == L'"' || ch == L'\'')
            {
                _quote = ch;
                _state = state_qdata;
                break;
            }

            _state = state_data;
            // fallthrough

        case state_data:
            if (ch == L'\n' || ch == L'\r')
            {
                log_debug("value \"" << _value << '"');
                setValue();
                checkNoColumns(_column, _noColumns, _lineNo);
                _deserializer->leaveMember();  // leave data item
                _deserializer->leaveMember();  // leave row
                _state = (ch == L'\r' ? state_cr : state_rowstart);
            }
            else if (ch == _delimiter)
            {
                log_debug("value \"" << _value << '"');
                setValue();
                _deserializer->leaveMember();  // leave data item
                ++_column;
                log_debug("member \""
                    << (_column < _titles.size() ? _titles[_column] : std::string()) << '"');
                _deserializer->beginMember(
                    _column < _titles.size() ? _titles[_column] : std::string(),
                    std::string(), SerializationInfo::Value);
                _state = state_data0;
            }
            else
            {
                _value  += ch;
            }
            break;

        case state_qdata:
            if (ch == _quote)
            {
                //it can be a double _quote, used to allow usage of _quote
                _state = state_qdata_end;
            }
            else
            {
                _value += ch;
            }
            break;

        case state_qdata_end:
            if (ch == _quote)
            {
                //it is indeed a double_quote. Add one.
                _value += ch;
                _state = state_qdata;
                break;
            }

            //it is not a double _quote. End the _value
            log_debug("value \"" << _value << '"');
            setValue();
            _deserializer->leaveMember();  // leave data item

            if (ch == L'\n' || ch == L'\r')
            {
                checkNoColumns(_column, _noColumns, _lineNo);
                _deserializer->leaveMember();  // leave row
                _state = (ch == L'\r' ? state_cr : state_rowstart);
            }
            else if (ch == _delimiter)
            {
                ++_column;
                log_debug("member \""
                    << (_column < _titles.size() ? _titles[_column] : std::string()) << '"');
                _deserializer->beginMember(
                    _column < _titles.size() ? _titles[_column] : std::string(),
                    std::string(), SerializationInfo::Value);
                _state = state_data0;
            }
            else
            {
                _value = _quote + _value + ch;
                _state = state_data;
            }
            break;
    }

}

void CsvParser::finish()
{
    switch (_state)
    {
        case state_datastart:
            _deserializer->leaveMember();  // leave row
            break;

        case state_data0:
        case state_data:
            checkNoColumns(_column, _noColumns, _lineNo);
            setValue();
            _deserializer->leaveMember();  // leave data item
            _deserializer->leaveMember();  // leave row
            break;

        case state_qdata:
            checkNoColumns(_column, _noColumns, _lineNo);
            log_debug("value \"" << _quote.narrow() << _value << '"');
            setValue();
            _deserializer->leaveMember();  // leave data item
            _deserializer->leaveMember();  // leave row
            break;

        case state_qdata_end:
            log_debug("value \"" << _value << '"');
            setValue();
            _deserializer->leaveMember();  // leave data item
            _deserializer->leaveMember();  // leave row
            break;

        default:
            ;
    }
}

void CsvParser::setValue()
{
    if (_value.empty())
    {
        _deserializer->setNull();
    }
    else
    {
        _deserializer->setValue(std::move(_value));
        _value.clear();
    }
}

}
