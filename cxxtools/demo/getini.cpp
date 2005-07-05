/* getini.cpp
   read value from ini-file
   Copyright (C) 2003-2005 Tommi Maekitalo

This file is part of cxxtools.

Cxxtools is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

Cxxtools is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Cxxtools; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330,
Boston, MA  02111-1307  USA
*/


#include <exception>
#include <iostream>
#include "cxxtools/iniclass.h"

int main(int argc, char* argv[])
{
  try
  {
    if (argc < 4)
    {
      std::cerr << "usage: " << argv[0]
        << " inifile section key [default]" << std::endl;
      return -1;
    }

    const char* fname = argv[1];
    const char* section = argv[2];
    const char* key = argv[3];
    const char* defvalue = argv[4] ? argv[4] : "";

    cxxtools::ini_file iniFile(fname);
    std::cout << iniFile.get_value(section, key, defvalue) << std::endl;
  }
  catch (const std::exception& e)
  {
    std::cerr << e.what() << std::endl;
  }
}

