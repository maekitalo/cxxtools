/*
 * Copyright (C) 2007 Tommi Maekitalo
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * is provided AS IS, WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, and
 * NON-INFRINGEMENT.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 *
 */

#include <iostream>
#include <fstream>
#include <stdexcept>
#include <cxxtools/mime.h>
#include <cxxtools/loginit.h>

int main(int argc, char* argv[])
{
  try
  {
    log_init();

    cxxtools::Mime mime;

    for (int a = 1; a < argc; ++a)
    {
      std::string fname = argv[a];

      std::ifstream ifile(argv[a]);
      if (!ifile)
        throw std::runtime_error("cannot open file " + fname);

      if (fname.size() >= 4 && fname.compare(fname.size() - 4, 4, ".jpg") == 0)
        mime.addBinaryFile("image/jpg", fname, ifile);
      else if (fname.size() >= 4 && fname.compare(fname.size() - 4, 4, ".gif") == 0)
        mime.addBinaryFile("image/gif", fname, ifile);
      else if (fname.size() >= 4 && fname.compare(fname.size() - 4, 4, ".png") == 0)
        mime.addBinaryFile("image/png", fname, ifile);
      else
        mime.addPart(ifile);
    }

    std::cout << mime;
  }
  catch (const std::exception& e)
  {
    std::cerr << e.what() << std::endl;
  }
}

