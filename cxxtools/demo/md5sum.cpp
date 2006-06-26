/* md5sum.cpp
 *
 * cxxtools - general purpose C++-toolbox
 * Copyright (C) 2003 Tommi Maekitalo
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
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

#include <fstream>
#include <iostream>
#include <algorithm>
#include <cxxtools/md5stream.h>

int main(int argc, char* argv[])
{
  cxxtools::Md5stream s;
  for (int i = 1; i < argc; ++i)
  {
    std::ifstream in(argv[i]);
    if (in)
    {
      // copy file to md5stream:
      s << in.rdbuf();
      std::cout << s.getHexDigest() << "  " << argv[i] << std::endl;
    }
  }
}

