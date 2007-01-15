/* hdstream.cpp
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

#include <iostream>
#include <fstream>
#include "cxxtools/hdstream.h"
#include "cxxtools/arg.h"
#include <algorithm>

class Hexdumper
{
    unsigned offset;

  public:
    explicit Hexdumper(unsigned offset_)
      : offset(offset_)
        { }
    void operator() (const char* file)
    {
      std::ifstream in(file);
      cxxtools::Hdostream hd;
      if (offset)
      {
        in.seekg(offset);
        hd.setOffset(offset);
      }
      hd << in.rdbuf() << std::flush;
    }
};

int main(int argc, char* argv[])
{
  cxxtools::Arg<unsigned> offset(argc, argv, 'o', 0);

  std::for_each(argv + 1, argv + argc, Hexdumper(offset));
}
