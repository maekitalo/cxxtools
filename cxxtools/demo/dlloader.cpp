/* dlloader.cpp
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
#include <cxxtools/dlloader.h>

typedef double (*function_type)(double);

// to run the program you may have to set LTDL_LIBRARY_PATH
// to the path of libm.so

int main(int argc, char* argv[])
{
  try
  {
    if (argc == 1)
    {
      std::cout << "load libm.so" << std::endl;
      cxxtools::dl::Library lib("libm");

      std::cout << "sym cos" << std::endl;
      function_type cosine = (function_type)(lib["cos"]);

      std::cout << "call cos" << std::endl;
      std::cout << "cos(2.0) = " << cosine(2.0) << std::endl;
    }
    else
    {
      std::cout << "load " << argv[1] << std::endl;
      cxxtools::dl::Library lib(argv[1]);
      std::cout << " => " << lib.getHandle() << std::endl;

      for (int a = 2; argv[a]; ++a)
      {
        std::cout << "sym " << argv[a] << std::endl;
        cxxtools::dl::Symbol sym = lib.sym(argv[a]);
        std::cout << " => " << static_cast<void*>(sym) << std::endl;
      }
    }
  }
  catch (const std::exception& e)
  {
    std::cerr << e.what() << std::endl;
  }
}

