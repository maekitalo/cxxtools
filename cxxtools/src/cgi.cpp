/* cgi.cpp
   Copyright (C) 2004 Tommi Mäkitalo

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

#include "cxxtools/cgi.h"

namespace cxxtools
{

cgi::cgi()
{
  char* q = getenv("QUERY_STRING");

  if (q)
    parse_url(q);

  parse_url(std::cin);
}

std::ostream* xmltag::default_out = &std::cout;

xmltag::xmltag(const std::string& tag_, std::ostream& out_)
  : tag(tag_),
    out(out_)
{
  if (!tag.empty())
  {
    if (tag.at(0) == '<' && tag.at(tag.size() - 1) == '>')
      tag = tag.substr(1, tag.size() - 2);

    out << '<' << tag << '>';
  }
}

xmltag::xmltag(const std::string& tag_, const std::string& parameter,
  std::ostream& out_)
  : tag(tag_),
    out(out_)
{
  if (!tag.empty())
  {
    if (tag.at(0) == '<' && tag.at(tag.size() - 1) == '>')
      tag = tag.substr(1, tag.size() - 2);

    out << '<' << tag;

    if (!parameter.empty())
      out << ' ' << parameter;

    out << '>';
  }
}

void xmltag::close()
{
  if (!tag.empty())
  {
    out << "</";

    std::string::size_type p = tag.find(' ');
    if (p != std::string::npos)
      out.write(tag.data(), p);
    else
      out << tag;

    out << '>';

    tag.clear();
  }
}

}
