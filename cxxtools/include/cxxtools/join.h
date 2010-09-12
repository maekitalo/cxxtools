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

#include <iostream>
#include <sstream>

namespace cxxtools
{
  /** @brief Joins a list of tokens into a output stream using a delimiter.

      A list of tokens read from a range is printed to a out stream. The tokens
      are separated by a token.

      Example (prints "4, 17, 12"):
      \code
        std::vector<unsigned> v;
        v.push_back(4);
        v.push_back(17);
        v.push_back(12);
        join(v.begin(), v.end(), ", ", std::cout);
      \endcode
   */
  template <typename inputIterator, typename separatorType, typename characterType>
  void join(inputIterator b, inputIterator e, const separatorType& sep, std::basic_ostream<characterType>& out)
  {
    bool first = true;
    for (inputIterator it = b; it != e; ++it)
    {
      if (first)
        first = false;
      else
        out << sep;
      out << *it;
    }
  }

  /** @brief Joins a list of tokens into a std::string using a delimiter.

      This function is just like the other join function, but returns the
      result as a std::string.

      Example (prints "4, 17, 12"):
      \code
        std::vector<unsigned> v;
        v.push_back(4);
        v.push_back(17);
        v.push_back(12);
        std::cout << join(v.begin(), v.end(), ", ");
      \endcode
   */
  template <typename inputIterator, typename separatorType>
  std::string join(inputIterator b, inputIterator e, const separatorType& sep)
  {
    std::ostringstream ret;
    join(b, e, sep, ret);
    return ret.str();
  }

}
