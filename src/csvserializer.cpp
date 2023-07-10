/*
 * Copyright (C) 2023 Tommi Maekitalo
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

#include <cxxtools/csvserializer.h>
#include <cxxtools/json.h>
#include <cxxtools/log.h>
#include <algorithm>
#include <map>

log_define("cxxtools.csv.serializer")

namespace cxxtools
{
static void collectTitles(std::vector<CsvSerializer::Title>& titles, const SerializationInfo& si, const std::string& prefix)
{
    for (const auto& ssi: si)
    {
        if (ssi.category() == SerializationInfo::Object)
        {
            auto namePrefix = ssi.name();
            if (!namePrefix.empty())
                namePrefix += '.';
            if (!prefix.empty())
                namePrefix = prefix + namePrefix;
            collectTitles(titles, ssi, namePrefix);
        }
        else if (ssi.category() == SerializationInfo::Value)
        {
            auto name = ssi.name();
            if (!name.empty())
            {
                if (!prefix.empty())
                    name = prefix + name;
                if (std::find(titles.begin(), titles.end(), name) == titles.end())
                    titles.emplace_back(name);
            }
        }
    }
}

static void collectData(std::map<std::string, String>& data, const SerializationInfo& si, const std::string& prefix)
{
    for (const auto& ssi: si)
    {
        if (ssi.category() == SerializationInfo::Object)
        {
            auto namePrefix = ssi.name();
            if (!namePrefix.empty())
                namePrefix += '.';
            if (!prefix.empty())
                namePrefix = prefix + namePrefix;
            collectData(data, ssi, namePrefix);
        }
        else if (ssi.category() == SerializationInfo::Value)
        {
            auto name = ssi.name();
            if (!name.empty())
            {
                if (!prefix.empty())
                    name = prefix + name;

                String value;
                ssi >>= value;
                data.emplace(name, value);
            }
        }
    }
}

CsvSerializer::CsvSerializer(std::ostream& os, TextCodec<Char, char>* codec)
    : _delimiter(L","),
      _quote('"'),
      _lineEnding(L"\n"),
      _ts(new TextOStream(os, codec)),
      _os(*_ts)
{ }

CsvSerializer::CsvSerializer(TextOStream& os)
    : _delimiter(L","),
      _quote('"'),
      _lineEnding(L"\n"),
      _os(os)
{ }

void CsvSerializer::csvOut(const String& value) const
{
    if (value.find(Char(_quote)) == String::npos
            && value.find(_delimiter) == String::npos
            && value.find(_lineEnding) == String::npos)
    {
        _os << value;
    }
    else
    {
        _os << _quote;
        for (String::const_iterator it = value.begin(); it != value.end(); ++it)
        {
            if (*it == _quote)
            {
                _os << _quote << _quote;
            }
            else
                _os << *it;
        }
        _os << _quote;
    }
}

const CsvSerializer& CsvSerializer::serialize(const SerializationInfo& si) const
{
    log_debug("serialize(" << Json(si).beautify(true) << ')');

    switch (si.category())
    {
        case SerializationInfo::Void:
            break;

        case SerializationInfo::Value:
        {
            String str;
            si >>= str;
            csvOut(str);
            break;
        }

        case SerializationInfo::Object:
        {
            std::vector<Title> titles = _titles;

            if (titles.empty())
                collectTitles(titles, si, "");

            if (!titles.empty())
            {
                csvOut(titles[0].title());
                for (unsigned n = 1; n < titles.size(); ++n)
                {
                    _os << _delimiter;
                    csvOut(titles[n].title());
                }

                _os << _lineEnding;
            }

            if (titles.empty())
            {
                bool first = true;
                for (const auto& di: si)
                {
                    String value;
                    di >>= value;
                    if (first)
                        first = false;
                    else
                        _os << _delimiter;
                    csvOut(value);
                }
            }
            else
            {
                std::map<std::string, String> data;
                collectData(data, si, "");

                bool first = true;
                for (const auto& title: titles)
                {
                    auto it = data.find(title.memberName());
                    if (it != data.end())
                    {
                        if (first)
                            first = false;
                        else
                            _os << _delimiter;
                        csvOut(it->second);
                    }
                }
            }

            _os << _lineEnding;

            break;
        }

        case SerializationInfo::Array:
        {
            std::vector<Title> titles = _titles;

            if (titles.empty())
                collectTitles(titles, si, "");

            if (!titles.empty())
            {
                csvOut(titles[0].title());
                for (unsigned n = 1; n < titles.size(); ++n)
                {
                    _os << _delimiter;
                    csvOut(titles[n].title());
                }

                _os << _lineEnding;
            }

            for (const auto& ssi: si)
            {
                if (titles.empty())
                {
                    bool first = true;
                    for (const auto& si: ssi)
                    {
                        String value;
                        si >>= value;
                        if (first)
                            first = false;
                        else
                            _os << _delimiter;
                        csvOut(value);
                    }
                }
                else
                {
                    std::map<std::string, String> data;
                    collectData(data, ssi, "");

                    bool first = true;
                    for (const auto& title: titles)
                    {
                        auto it = data.find(title.memberName());
                        if (it != data.end())
                        {
                            if (first)
                                first = false;
                            else
                                _os << _delimiter;
                            csvOut(it->second);
                        }
                    }
                }

                _os << _lineEnding;
            }

            break;
        }
    }

    _os.flush();

    return *this;
}

}
