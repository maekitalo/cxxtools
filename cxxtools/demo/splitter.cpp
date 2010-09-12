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
#include <cxxtools/arg.h>
#include <cxxtools/split.h>
#include <cxxtools/join.h>
#include <vector>

int main(int argc, char* argv[])
{
  try
  {
    cxxtools::Arg<char> inputFieldSeparator(argc, argv, 'f', '\t');
    cxxtools::Arg<std::string> fieldRegex(argc, argv, 'r');
    cxxtools::Arg<char> outputFieldSeparator(argc, argv, 'o', '\t');

    cxxtools::Regex re;
    if (fieldRegex.isSet())
      re = cxxtools::Regex(fieldRegex);

    std::string line;
    while (std::getline(std::cin, line))
    {
      std::vector<std::string> tokens;

      if (fieldRegex.isSet())
        cxxtools::split(re, line, std::back_inserter(tokens));
      else
        cxxtools::split(inputFieldSeparator.getValue(), line, std::back_inserter(tokens));

      std::string l = cxxtools::join(tokens.begin(), tokens.end(), outputFieldSeparator.getValue());
      std::cout << l << '\n';
    }
  }
  catch (const std::exception& e)
  {
    std::cerr << e.what() << std::endl;
  }
}

