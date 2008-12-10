/*
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
 */


#include <cxxtools/arg.h>
#include <iostream>

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
    using cxxtools::Arg;

    Arg<int> int_param(argc, argv, 'i');
    Arg<int> long_int_param(argc, argv, "--int");
    Arg<int> int_param_def(argc, argv, 'I', 5);

    print_int("option -i", int_param);
    print_int("option --int", long_int_param);
    print_int("option -I", int_param_def);

    Arg<const char*> charp_param(argc, argv, 'p');
    Arg<const char*> long_charp_param(argc, argv, "--charp");
    Arg<const char*> charp_param_def(argc, argv, 'P', "hi");

    print_charp("option -p", charp_param);
    print_charp("option --charp", long_charp_param);
    print_charp("option -P", charp_param_def);

    Arg<std::string> string_param(argc, argv, 's');
    Arg<std::string> long_string_param(argc, argv, "--string");
    Arg<std::string> string_param_def(argc, argv, 'S', "hi");
    Arg<std::string> string_arg(argc, argv);

    print_string("option -s", string_param);
    print_string("option --string", long_string_param);
    print_string("option -S", string_param_def);
    print_string("string-Arg", string_arg);

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
