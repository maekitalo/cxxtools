/*
 * Copyright (C) 2019 Tommi Maekitalo
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

#include <cxxtools/iniserializer.h>

namespace cxxtools
{

static bool isplain(const String& str)
{
    if (str.empty())
        return true;

    if (std::isspace(str[0])
      || std::isspace(str[str.size() - 1]))
        return false;

    return true;
}

static void stringOut(TextOStream& _os, const cxxtools::String& str)
{
    for (cxxtools::String::const_iterator it = str.begin(); it != str.end(); ++it)
    {
        if (*it == L'"')
            _os << "\\\"";
        else if (*it == L'\\')
            _os << "\\\\";
        else if (*it == L'\b')
            _os << "\\b";
        else if (*it == L'\f')
            _os << "\\f";
        else if (*it == L'\n')
            _os << "\\n";
        else if (*it == L'\r')
            _os << "\\r";
        else if (*it == L'\t')
            _os << "\\t";
        else
            _os << *it;
    }
}

IniSerializer& IniSerializer::serialize(const SerializationInfo& si)
{
    // TODO escaping
    for (SerializationInfo::const_iterator section = si.begin(); section != si.end(); ++section)
    {
        _os << L'[' << String(section->name()) << L"]\n";
        for (SerializationInfo::const_iterator value = section->begin(); value != section->end(); ++value)
        {
            cxxtools::String v;
            value->getValue(v);
            _os << String(value->name()) << L'=';
            if (isplain(v))
                _os << v << L'\n';
            else
            {
                _os << '"';
                stringOut(_os, v);
                _os << "\"\n";
            }
        }
    }

    return *this;
}

}
