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

#include <cxxtools/csvformatter.h>
#include <cxxtools/log.h>

log_define("cxxtools.csv.formatter")

namespace cxxtools
{
    CsvFormatter::CsvFormatter(std::ostream& os, TextCodec<Char, char>* codec)
        : _firstline(true),
          _collectTitles(true),
          _level(0),
          _delimiter(','),
          _quote('"'),
          _lineEnding(L"\n"),
          _ts(new TextOStream(os, codec)),
          _os(*_ts)
    { }

    CsvFormatter::CsvFormatter(TextOStream& os)
        : _firstline(true),
          _collectTitles(true),
          _level(0),
          _delimiter(','),
          _quote('"'),
          _lineEnding(L"\n"),
          _ts(0),
          _os(os)
    { }

    CsvFormatter::~CsvFormatter()
    {
        delete _ts;
    }

    void CsvFormatter::selectColumn(const std::string& title)
    {
        _titles.resize(_titles.size() + 1);
        _titles.back()._memberName = title;
        _titles.back()._title = title;
        _collectTitles = false;
    }

    void CsvFormatter::selectColumn(const std::string& memberName, const std::string& title)
    {
        _titles.resize(_titles.size() + 1);
        _titles.back()._memberName = memberName;
        _titles.back()._title = title;
        _collectTitles = false;
    }

    void CsvFormatter::toCsvData(String& ret, const std::string& /*type*/, const String& value)
    {
        if (value.find(Char(_quote)) == String::npos
                && value.find(Char(_delimiter)) == String::npos
                && value.find(_lineEnding) == String::npos)
        {
            ret = value;
        }
        else
        {
            ret.clear();
            ret += _quote;
            for (String::const_iterator it = value.begin(); it != value.end(); ++it)
            {
                if (*it == _quote)
                {
                    ret += _quote;
                    ret += _quote;
                }
                else
                    ret += *it;
            }
            ret += _quote;
        }
    }

    void CsvFormatter::dataOut()
    {
        if (_firstline)
        {
            if (!_titles.empty())
            {
                log_debug("print " << _titles.size() << " titles");
                for (unsigned n = 0; n < _titles.size(); ++n)
                {
                    if (n > 0)
                        _os << _delimiter;
                    _os << String(_titles[n]._title);
                }

                _os << _lineEnding;
            }

            _firstline = false;
            _collectTitles = false;
        }

        log_debug("output " << _data.size() << " columns");
        for (unsigned n = 0; n < _data.size(); ++n)
        {
            if (n > 0)
                _os << _delimiter;
            _os << _data[n];
        }
        _os << _lineEnding;

        _data.clear();
    }

    void CsvFormatter::addValueString(const std::string& /*name*/, const std::string& type,
                          const String& value)
    {
        if (_memberName.empty())
        {
            log_debug("addValue plain value \"" << value << '"');
            _data.push_back(String());
            toCsvData(_data.back(), type, value);
        }
        else
        {
            log_debug("addValue member \"" << _memberName << "\" value \"" << value << '"');
            for (unsigned n = 0; n < _titles.size(); ++n)
            {
                if (_titles[n]._memberName == _memberName)
                {
                    log_debug("column " << n);
                    if (_data.size() <= n)
                        _data.resize(n + 1);
                    toCsvData(_data[n], type, value);
                    _memberName.clear();
                    break;
                }
            }
        }
    }

    void CsvFormatter::beginArray(const std::string& /*name*/, const std::string& /*type*/)
    {
        ++_level;
        log_debug("beginArray, level=" << _level);
    }

    void CsvFormatter::finishArray()
    {
        --_level;

        log_debug("finishArray, level=" << _level);

        if (_level == 1)
            dataOut();
    }

    void CsvFormatter::beginObject(const std::string& /*name*/, const std::string& /*type*/)
    {
        ++_level;
        log_debug("beginObject, level=" << _level);
    }

    void CsvFormatter::beginMember(const std::string& name)
    {
        log_debug("beginMember " << name);
        if (_collectTitles && _firstline && _level == 2)
        {
            log_debug("add title " << name);
            _titles.resize(_titles.size() + 1);
            _titles.back()._title = name;
            _titles.back()._memberName = name;
        }

        _memberName = name;
    }

    void CsvFormatter::finishMember()
    {
        log_debug("finishMember");
        _memberName.clear();
    }

    void CsvFormatter::finishObject()
    {
        --_level;

        log_debug("finishObject, level=" << _level);

        if (_level == 1)
            dataOut();
    }

    void CsvFormatter::finish()
    {
        log_debug("finish");

        if (_firstline && !_titles.empty())
        {
            log_debug("print " << _titles.size() << " titles");
            for (unsigned n = 0; n < _titles.size(); ++n)
            {
                if (n > 0)
                    _os << _delimiter;
                _os << String(_titles[n]._title);
            }

            _os << _lineEnding;
        }

        _os.flush();
    }
}
