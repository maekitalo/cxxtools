/* iconv.cpp
 *
 * cxxtools - general purpose C++-toolbox
 * Copyright (C) 2006 Tommi Maekitalo
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

#include <cxxtools/arg.h>
#include <cxxtools/iconverter.h>
#include <iostream>
#include <fstream>

/**
 * synopsis:
 *   iconv -f UTF8 -t LATIN1 -i file
 *     prints UTF8-file to std::cout as LATIN1
 *
 *   iconf -f LATIN1 -t ISO8859-1 'some text'
 *     prints 'some text' in ISO8859-1 to std::cout
 *       (bad example - same as <echo 'some text'>, because
 *        conversion does not change anything, but it is difficult to
 *        show a better one)
 *
 *   iconv <file
 *     prints UTF8-file to std::cout as LATIN1 (default conversion)
 */

int main(int argc, char* argv[])
{
  try
  {
	cxxtools::Arg<const char*> from(argc, argv, 'f', "UTF8");
	cxxtools::Arg<const char*> to(argc, argv, 't', "LATIN1");
	cxxtools::Arg<const char*> infile(argc, argv, 'i');  // inputfile

	if (infile.isSet())
	{
	  // read from file
	  std::ifstream in(infile);
	  cxxtools::iconvstream ic(std::cout, to, from);
	  ic << in.rdbuf();
	}

	if (argc > 1)
	{
	  // convert arguments to cout
	  cxxtools::IConverter ic(to.getValue(), from.getValue());
	  for (int a = 1; a < argc; ++a)
		std::cout << ic.convert(argv[a]);
	}

	if (!infile.isSet() && argc <= 1)
	{
	  // filter cin to cout
	  cxxtools::iconvstream ic(std::cout, to, from);
	  ic << std::cin.rdbuf();
	}
  }
  catch (const std::exception& e)
  {
    std::cerr << e.what() << std::endl;
    return 1;
  }
}
