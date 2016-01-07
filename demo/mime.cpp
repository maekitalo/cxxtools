/*
 * Copyright (C) 2007 Tommi Maekitalo
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
#include <fstream>
#include <stdexcept>
#include <cxxtools/mime.h>
#include <cxxtools/arg.h>
#include <cxxtools/log.h>

int main(int argc, char* argv[])
{
  try
  {
    log_init();

    cxxtools::Arg<bool> binary(argc, argv, 'b');

    cxxtools::MimeMultipart mime;

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
      else if (binary)
        mime.addBinaryFile("application/x-binary", fname, ifile);
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
