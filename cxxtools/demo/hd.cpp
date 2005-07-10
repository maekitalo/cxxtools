/* hdstream.cpp
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

#include <iostream>
#include <fstream>
#include "cxxtools/hdstream.h"
#include <algorithm>

void hexdump(const char* file)
{
  std::ifstream in(file);
  cxxtools::hdostream hd;
  hd << in.rdbuf() << std::flush;
}

int main(int argc, char* argv[])
{
  std::for_each(argv + 1, argv + argc, hexdump);
}
