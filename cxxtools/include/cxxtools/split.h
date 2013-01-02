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

#ifndef CXXTOOLS_SPLIT_H
#define CXXTOOLS_SPLIT_H

#include <string>
#include <cxxtools/regex.h>

namespace cxxtools
{
  /** @brief Splits a std::string into tokens using a delimiter character

      This is a little helper functions, which splits a string into tokens.
      A delimiter character is passed as a character and the resulting tokens
      are written to a output iterator.

      The most common output iterator is a std::back_inserter.

      Example (prints username and uid on a unix system:

      \code
        std::ifstream passwd("/etc/passwd");
        std::string line;
        while (std::getline(passwd, line))
        {
          std::vector<std::string> tokens;
          cxxtools::split(':', line, std::back_inserter(tokens));
          if (tokens.size() > 2)
            std::cout << "username: " << tokens[0] << " uid=" << tokens[2] << std::endl;
        }
      \endcode

   */
  template <typename outputIterator, typename characterType>
  void split(characterType ch, const std::basic_string<characterType>& line, outputIterator it)
  {
    std::basic_string<characterType> s(line);
    typename std::basic_string<characterType>::size_type pos;

    while ((pos = s.find(ch)) != std::basic_string<characterType>::npos)
    {
      *it = s.substr(0, pos);
      ++it;
      s.erase(0, pos + 1);
    }

    *it = s;
    ++it;
  }

  /** @brief Splits a std::string into tokens using a set of delimiter characters

      This is a little helper functions, which splits a string into tokens.
      A set of delimiter characters is passed as a zero terminated char array and
      the resulting tokens are written to a output iterator.

      The most common output iterator is a std::back_inserter.

      Example (splits a line on white space):
      \code
        std::string line = ...;
        std::vector<std::string> tokens;
        cxxtools::split("[ \t]+", line, std::back_inserter(tokens));
      \endcode
   

   */
  template <typename outputIterator, typename characterType>
  void split(const characterType* chars, const std::basic_string<characterType>& line, outputIterator it)
  {
    std::basic_string<characterType> s(line);
    typename std::basic_string<characterType>::size_type pos;

    while ((pos = s.find_first_of(chars)) != std::basic_string<characterType>::npos)
    {
      *it = s.substr(0, pos);
      ++it;
      s.erase(0, pos + 1);
    }

    *it = s;
    ++it;
  }

  /** @brief Splits a std::string into tokens using a set of delimiter characters

      This is a little helper functions, which splits a string into tokens.
      A set of delimiter characters is passed as a string and the resulting tokens
      are written to a output iterator.

      The most common output iterator is a std::back_inserter.

      Example (splits a line on white space):
      \code
        std::string line = ...;
        std::vector<std::string> tokens;
        cxxtools::split("[ \t]+", line, std::back_inserter(tokens));
      \endcode
   

   */
  template <typename outputIterator, typename characterType>
  void split(const std::basic_string<characterType>& chars, const std::basic_string<characterType>& line, outputIterator it)
  {
    std::basic_string<characterType> s(line);
    typename std::basic_string<characterType>::size_type pos;

    while ((pos = s.find_first_of(chars)) != std::basic_string<characterType>::npos)
    {
      *it = s.substr(0, pos);
      ++it;
      s.erase(0, pos + 1);
    }

    *it = s;
    ++it;
  }

  /** @brief Splits a std::string into tokens using a regular expression.

      This function is much like the other split function, but uses a regular
      expression to find a delimiter. This is useful e.g. to split a string
      using white space:

      Example (splits a line on white space):
      \code
        std::string line = ...;
        std::vector<std::string> tokens;
        cxxtools::split(cxxtools::Regex("[ \t]+"), line, std::back_inserter(tokens));
      \endcode
   
   */
  template <typename outputIterator>
  void split(const Regex& re, const std::string& line, outputIterator it)
  {
    std::string s(line);
    RegexSMatch sm;

    while (re.match(s, sm))
    {
      *it = s.substr(0, sm.offsetBegin(0));
      ++it;
      s.erase(0, sm.offsetEnd(0));
    }

    *it = s;
    ++it;
  }

}

#endif // CXXTOOLS_SPLIT_H
