/* xmltag.cpp
 *
 * cxxtools - general purpose C++-toolbox
 * Copyright (C) 2004 Tommi Maekitalo
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

#include "cxxtools/xmltag.h"
#include <iostream>

namespace cxxtools
{

std::ostream* Xmltag::default_out = &std::cout;

Xmltag::Xmltag(const std::string& tag_, std::ostream& out_)
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

Xmltag::Xmltag(const std::string& tag_, const std::string& parameter,
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

void Xmltag::close()
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
