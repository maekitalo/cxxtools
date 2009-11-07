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

#include <exception>
#include <iostream>
#include <cxxtools/cgi.h>
#include <cxxtools/xmltag.h>
#include <cxxtools/arg.h>

int cgidemo()
{
  cxxtools::Cgi q;  // this parses all input-parameters

  // print standard-header
  std::cout << cxxtools::Cgi::header();

  cxxtools::Xmltag html("html");  // start html-block
  cxxtools::Xmltag body("body");  // start body-block

  cxxtools::Xmltag form("form");  // start form
  std::cout << "<input type=\"text\" name=\"v\">"
               "<br>"
               "<input type=\"submit\">";
  form.close(); // alternative 1: explicitly trigger closing form-tag

  std::cout << "<hr>\n"
               "you entered: ";

  {
    cxxtools::Xmltag bold("b");  // bold here in an embedded scope
    std::cout << q["v"];
  } // alternative 2: implicitly closing bold-tag by leaving scope

  // automatically close body and html by leaving scope

  return 0;
}

int tagstest()
{
  cxxtools::Xmltag("html");  // prints <html></html>
  std::cout << '\n';

  cxxtools::Xmltag("test param=1");  // prints <test param=1></test>
  std::cout << '\n';

  cxxtools::Xmltag("<noparam>");     // prints <noparam></noparam>
  std::cout << '\n';

  cxxtools::Xmltag("<param p=1>");   // prints <param p=1></param>
  std::cout << '\n';

  cxxtools::Xmltag("<param>", "p=1");   // prints <param p=1></param>
  std::cout << '\n';

  cxxtools::Xmltag("<br>").clear();  // prints <br>
  std::cout << '\n';

  std::cout << std::endl;
  return 0;
}

int main(int argc, char* argv[])
{
  cxxtools::Arg<bool> do_tagstest(argc, argv, 't');
  return  do_tagstest ? tagstest()
                      : cgidemo();
}
