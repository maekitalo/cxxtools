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

using cxxtools::Arg;

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
    Arg<int> int_param(5); // default-value 5
    int_param.set(argc, argv, 'i');
    int_param.set(argc, argv, "--int");

    print_int("option -i|--int", int_param);

    Arg<const char*> charp_param("hi"); // default-value
    charp_param.set(argc, argv, 'p');
    charp_param.set(argc, argv, "--charp");

    print_charp("option -p|--charp", charp_param);

    Arg<std::string> string_param("hi"); // default-value
    string_param.set(argc, argv, 's');
    string_param.set(argc, argv, "--string");

    Arg<std::string> string_arg;
    string_arg.setNoOpt(argc, argv);

    print_string("option -s|--string", string_param);
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
