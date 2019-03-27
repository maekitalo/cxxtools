/*
 * Copyright (C) 2010 Tommi Maekitalo
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

#ifndef CXXTOOLS_TRIM_H
#define CXXTOOLS_TRIM_H

#include <string>

namespace cxxtools
{
    /// Removes white space from left of a string. Returns the modified string.
    template <typename StringType>
    StringType ltrim(const StringType& s, const StringType& ws = StringType(" \t\r\n"))
    {
        typename StringType::size_type p = s.find_first_not_of(ws);
        return p == StringType::npos ? StringType() : s.substr(p);
    }

    /// Removes white space from right of a string. Returns the modified string.
    template <typename StringType>
    StringType rtrim(const StringType& s, const StringType& ws = StringType(" \t\r\n"))
    {
        typename StringType::size_type p = s.find_last_not_of(ws);
        return p == StringType::npos ? StringType() : s.substr(0, p + 1);
    }

    /// Removes white space from left and right of a string. Returns the modified string.
    template <typename StringType>
    StringType trim(const StringType& s, const StringType& ws = StringType(" \t\r\n"))
    {
        typename StringType::size_type pl = s.find_first_not_of(ws);

        if (pl == StringType::npos)
          return StringType();

        typename StringType::size_type pr = s.find_last_not_of(ws);

        return s.substr(pl, pr - pl + 1);
    }

    /// Removes white space from left of a string. Returns the modified string.
    /// The passed string is modified.
    template <typename StringType>
    StringType& ltrimi(StringType& s, const StringType& ws = StringType(" \t\r\n"))
    {
        typename StringType::size_type p = s.find_first_not_of(ws);
        s.erase(0, p);
        return s;
    }

    /// Removes white space from right of a string. Returns the modified string.
    /// The passed string is modified.
    template <typename StringType>
    StringType& rtrimi(StringType& s, const StringType& ws = StringType(" \t\r\n"))
    {
        typename StringType::size_type p = s.find_last_not_of(ws);
        if (p == StringType::npos)
            s.clear();
        else
            s.erase(p + 1);
        return s;
    }

    /// Removes white space from left and right of a string. Returns the modified string.
    /// The passed string is modified.
    template <typename StringType>
    StringType& trimi(StringType& s, const StringType& ws = StringType(" \t\r\n"))
    {
        return ltrimi(rtrimi(s, ws), ws);
    }
}

#endif // CXXTOOLS_TRIM_H
