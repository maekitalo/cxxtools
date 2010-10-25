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
  std::string ltrim(const std::string& s, const std::string& ws = " \t")
  {
    std::string::size_type p = 0;
    std::string::size_type pp;
    while (p < s.size() && (pp = ws.find(s[p])) != std::string::npos)
      ++p;
    return s.substr(p);
  }

  std::string rtrim(const std::string& s, const std::string& ws = " \t")
  {
    std::string::size_type p = s.size();
    while (p > 0 && ws.find(s[p - 1]) != std::string::npos)
      --p;
    return s.substr(0, p);
  }

  std::string trim(const std::string& s, const std::string& ws = " \t")
  {
    std::string::size_type pl = 0;
    while (pl < s.size() && ws.find(s[pl]) != std::string::npos)
      ++pl;

    if (pl == s.size())
      return std::string();

    std::string::size_type pr = s.size();
    while (pr > 0 && ws.find(s[pr - 1]) != std::string::npos)
      --pr;

    return s.substr(pl, pr - pl);
  }

}

#endif // CXXTOOLS_TRIM_H
