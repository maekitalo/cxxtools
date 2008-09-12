/* getini.cpp
 * read value from ini-file
 *
 * cxxtools - general purpose C++-toolbox
 * Copyright (C) 2003 Tommi Maekitalo
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
 *
 */

#include <exception>
#include <iostream>
#include <fstream>
#include <iterator>
#include <cxxtools/inifile.h>
#include <cxxtools/loginit.h>
#include <cxxtools/arg.h>

log_define("cxxtools.getini")

int main(int argc, char* argv[])
{
  log_init();

  try
  {
    cxxtools::Arg<bool> quiet(argc, argv, 'q');

    if (argc <= 1)
    {
      std::cerr << "usage: " << argv[0]
        << " inifile [-q] [section [key [default]]]" << std::endl;
      return -1;
    }

    const char* fname = argv[1];
    std::ifstream in(fname);
    if (!in)
      throw std::runtime_error(std::string("error reading input file \"") + fname + '"');

    cxxtools::IniFile ini(in);

    if (argc <= 2)
    {
      // list sections
      if (!quiet)
        std::cout << "sections in " << fname << ": ";
      ini.getSections(std::ostream_iterator<std::string>(std::cout, "\t"));
      std::cout << std::endl;
      return 0;
    }

    const char* section = argv[2];
    if (argc <= 3)
    {
      // list keys
      if (!quiet)
        std::cout << "keys in " << fname << " [" << section << "]: ";
      ini.getKeys(section, std::ostream_iterator<std::string>(std::cout, "\t"));
      std::cout << std::endl;
      return 0;
    }

    const char* key = argv[3];
    const char* defvalue = argv[4] ? argv[4] : "";
    if (!quiet)
      std::cout << "value of " << fname << " [" << section << "]." << key << ": ";
    std::cout << ini.getValue(section, key, defvalue) << std::endl;
  }
  catch (const std::exception& e)
  {
    std::cerr << e.what() << std::endl;
  }
}

