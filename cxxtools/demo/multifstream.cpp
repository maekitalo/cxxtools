/* multifstream.cpp
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


#include <cxxtools/multifstream.h>
#include <iostream>
#include <algorithm>

int main(int argc, char* argv[])
{
  try
  {
    cxxtools::multi_ifstream in;
    std::copy(argv + 1,
              argv + argc,
              in.back_inserter());

    std::cout << in.rdbuf();
  }
  catch (const std::exception& e)
  {
    std::cerr << e.what() << std::endl;
  }
}
