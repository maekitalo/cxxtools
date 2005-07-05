/* arg-set.cpp
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


#include <cxxtools/arg.h>
#include <iostream>

using cxxtools::arg;

void print_int(const char* txt, int i)
{
  std::cout << txt << ": " << i << std::endl;
}

void print_charp(const char* txt, const char* p)
{
  std::cout << txt << ": " << (p ? p : "NULL") << std::endl;
}

void print_string(const char* txt, const std::string& s)
{
  std::cout << txt << ": " << s << std::endl;
}

int main(int argc, char* argv[])
{
  try
  {
    arg<int> int_param(5); // default-value 5
    int_param.set(argc, argv, 'i');
    int_param.set(argc, argv, "--int");

    print_int("option -i|--int", int_param);

    arg<const char*> charp_param("hi"); // default-value
    charp_param.set(argc, argv, 'p');
    charp_param.set(argc, argv, "--charp");

    print_charp("option -p|--charp", charp_param);

    arg<std::string> string_param("hi"); // default-value
    string_param.set(argc, argv, 's');
    string_param.set(argc, argv, "--string");

    arg<std::string> string_arg;
    string_arg.setNoOpt(argc, argv);

    print_string("option -s|--string", string_param);
    print_string("string-arg", string_arg);

    std::cout << "unprocessed arguments:\n";
    for (int i = 1; i < argc; ++i)
      std::cout << '\t' << i << ": " << argv[i] << std::endl;
  }
  catch (const std::exception& e)
  {
    std::cerr << e.what() << std::endl;
    return 1;
  }
}
